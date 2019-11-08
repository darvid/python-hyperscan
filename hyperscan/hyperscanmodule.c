#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <bytesobject.h>
#include <structmember.h>
#include <stdio.h>
#include <hs/hs.h>

#define ADD_INT_CONSTANT(module, name) PyModule_AddIntConstant( \
  module, #name, name);
#define HANDLE_HYPERSCAN_ERR(err, rv) if (err != HS_SUCCESS) { \
    char serr[80]; \
    sprintf(serr, "error code %i", err); \
    PyGILState_STATE gstate = PyGILState_Ensure(); \
    PyErr_SetString(HyperscanError, serr); \
    PyGILState_Release(gstate); \
    return rv; \
  }

#if PY_MAJOR_VERSION >= 3
  #define MOD_ERROR_VAL NULL
  #define MOD_SUCCESS_VAL(val) val
  #define MOD_INIT(name) PyMODINIT_FUNC PyInit_##name(void)
  #define MOD_DEF(ob, name, doc, methods) \
          static struct PyModuleDef moduledef = { \
            PyModuleDef_HEAD_INIT, name, doc, -1, methods, }; \
          ob = PyModule_Create(&moduledef);
#else
  #define MOD_ERROR_VAL
  #define MOD_SUCCESS_VAL(val)
  #define MOD_INIT(name) void init##name(void)
  #define MOD_DEF(ob, name, doc, methods) \
          ob = Py_InitModule3(name, methods, doc);
#endif

const char* DEFAULT_VERSION = "unknown";

static PyObject *HyperscanError;
static PyTypeObject DatabaseType;
static PyTypeObject ScratchType;
static PyTypeObject StreamType;

typedef struct {
  PyObject *callback;
  PyObject *ctx;
  int success;
} py_scan_callback_ctx;

typedef struct {
  PyObject_HEAD
  PyObject *scratch;
  hs_database_t *db;
  unsigned int mode;
} Database;

typedef struct {
  PyObject_HEAD
  hs_stream_t *identifier;
  PyObject *database;
  PyObject *scratch;
  unsigned int flags;
  py_scan_callback_ctx *cctx;
} Stream;

typedef struct {
  PyObject_HEAD
  PyObject *database;
  hs_scratch_t *scratch;
} Scratch;

static int match_handler(unsigned int id, unsigned long long from,
                         unsigned long long to, unsigned int flags,
                         void *context) {
  py_scan_callback_ctx *cctx = context;
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  PyObject *rv = PyObject_CallFunction(cctx->callback, "IIIIO", id,
                                       from, to, flags, cctx->ctx);
  int halt = 1;
  if (rv == NULL) {
    cctx->success = 0;
  } else {
    halt = rv == Py_None ? 0 : PyObject_IsTrue(rv);
    cctx->success = 1;
  }
  PyGILState_Release(gstate);
  Py_XDECREF(rv);
  return halt;
}

static void Database_dealloc(Database* self) {
  hs_free_database(self->db);
  hs_scratch_t *scratch = ((Scratch*)self->scratch)->scratch;
  if (scratch != NULL)
    hs_free_scratch(scratch);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* Database_new(PyTypeObject *type, PyObject *args,
                              PyObject *kwds) {
  Database *self;

  self = (Database*)type->tp_alloc(type, 0);
  if (self != NULL) {
    self->mode = HS_MODE_BLOCK;
  }

  return (PyObject*)self;
}

static int Database_init(Database *self, PyObject *args,
                         PyObject *kwds) {
  static char *kwlist[] = {"scratch", "mode", NULL};
  self->scratch = Py_None;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|Oi", kwlist,
                                   &self->scratch, &self->mode))
    return -1;
  return 0;
}

static PyObject* Database_compile(Database *self, PyObject *args,
                                  PyObject *kwds) {
  PyObject *oexpressions = Py_None,
           *oflags       = Py_None,
           *oflag        = Py_None,
           *oids         = Py_None,
           *oliteral     = Py_False;
  int elements = -1;

  static char *kwlist[] = {"expressions", "ids", "elements", "flags",
                           "literal", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|OIOO", kwlist,
                                   &oexpressions, &oids, &elements,
                                   &oflags, &oliteral))
    return NULL;

  if (elements == -1) {
    elements = PySequence_Size(oexpressions);
    if (elements == -1) {
      PyErr_SetString(PyExc_TypeError, "expressions must be a sequence");
      return NULL;
    }
  }

  PyObject *oexpr = NULL, *oid = NULL;
  const char *expressions[elements];
  unsigned int ids[elements],
               flags[elements],
               globalflag = (oflags == Py_None
                             ? 0
                             : PyLong_AsUnsignedLong(oflags));

  PyErr_Clear();

  for (int i = 0; i < elements; i++) {
    oexpr = PySequence_ITEM(oexpressions, i);
    expressions[i] = PyBytes_AsString(oexpr);
    if (PyErr_Occurred())
        break;

    if (PyObject_IsTrue(oids)) {
      oid = PySequence_ITEM(oids, i);
      ids[i] = PyLong_AsUnsignedLong(oid);
      if (PyErr_Occurred())
        break;
    } else {
      ids[i] = i;
    }

    if (PySequence_Check(oflags)) {
      oflag = PySequence_ITEM(oflags, i);
      if (PyErr_Occurred())
        break;
      flags[i] = PyLong_AsUnsignedLong(oflag);
      if (PyErr_Occurred())
        break;
    } else {
      flags[i] = globalflag;
    }
  }

  Py_XDECREF(oexpr);
  Py_XDECREF(oflag);
  Py_XDECREF(oid);

  if (PyErr_Occurred())
    return NULL;

  hs_error_t err;
  hs_compile_error_t *compile_err;

  Py_BEGIN_ALLOW_THREADS
  if (PyObject_IsTrue(oliteral)) {
    size_t lens[elements];
    for (int i = 0; i < elements; i++) {
      lens[i] = PySequence_Length(PySequence_ITEM(oexpressions, i)) - 1;
    }
    err = hs_compile_lit_multi(expressions, flags, ids, lens, elements,
                               self->mode, NULL, &self->db, &compile_err);
  } else {
    err = hs_compile_ext_multi(expressions, flags, ids, NULL, elements,
                               self->mode, NULL, &self->db, &compile_err);
  }
  Py_END_ALLOW_THREADS

  if (err != HS_SUCCESS) {
    PyErr_SetString(HyperscanError, compile_err->message);
    hs_free_compile_error(compile_err);
    return NULL;
  }

  if (self->scratch == Py_None)
    self->scratch = PyObject_CallFunction((PyObject*)&ScratchType,
                                          "O", (PyObject*)self, 0);
  else {
    err = hs_alloc_scratch(self->db, &((Scratch*)self->scratch)->scratch);
    HANDLE_HYPERSCAN_ERR(err, NULL);
  }
  Py_RETURN_NONE;
}

static PyObject* Database_info(Database *self, PyObject *args) {
  char *info;
  hs_error_t err = hs_database_info(self->db, &info);
  HANDLE_HYPERSCAN_ERR(err, NULL);
  PyObject *oinfo = PyBytes_FromString(info);
  Py_INCREF(oinfo);
  free(info);
  return oinfo;
}

static PyObject* Database_size(Database *self, PyObject *args) {
  size_t database_size;
  hs_error_t err = hs_database_size(self->db, &database_size);
  HANDLE_HYPERSCAN_ERR(err, NULL);
  PyObject *odatabase_size = PyLong_FromSize_t(database_size);
  Py_INCREF(odatabase_size);
  return odatabase_size;
}

static PyObject* Database_scan(Database *self, PyObject *args, PyObject *kwds) {
  char *data;
  Py_ssize_t length;
  hs_error_t err;
  unsigned int flags = 0;
  PyObject *ocallback = Py_None,
            *oscratch = Py_None,
                *octx = Py_None;
  static char *kwlist[] = {"data", "match_event_handler", "flags", "context",
                           "scratch", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#|OIOO", kwlist,
                                   &data, &length, &ocallback, &flags,
                                   &octx, &oscratch))
    return NULL;
  py_scan_callback_ctx cctx = {ocallback, octx, 1};
  Py_BEGIN_ALLOW_THREADS
  err = hs_scan(
    self->db,
    data,
    length,
    flags,
    oscratch == Py_None ? ((Scratch*)self->scratch)->scratch
                        : ((Scratch*)oscratch)->scratch,
    ocallback == Py_None ? NULL : match_handler,
    ocallback == Py_None ? NULL : (void*)&cctx
  );
  Py_END_ALLOW_THREADS
  if (!cctx.success) {
    return NULL;
  }
  HANDLE_HYPERSCAN_ERR(err, NULL);
  Py_RETURN_NONE;
}

static PyObject* Database_stream(Database *self, PyObject *args,
                                 PyObject *kwds) {
  unsigned int flags = 0;
  PyObject *ocallback = Py_None,
                *octx = Py_None;
  static char *kwlist[] = {"flags", "match_event_handler", "context", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|IOO", kwlist,
                                   &flags, &ocallback, &octx))
    return NULL;
  PyObject *stream = PyObject_CallFunction(
    (PyObject*)&StreamType, "OIOO", (PyObject*)self, flags, ocallback, octx);
  if (PyErr_Occurred())
    return NULL;
  Py_INCREF(stream);
  return stream;
}

static PyMemberDef Database_members[] = {
  {"mode", T_INT, offsetof(Database, mode), 0,
   ":obj:`int`: Scanning mode."},
  {"scratch", T_OBJECT_EX, offsetof(Database, scratch), 0,
   ":class:`~.Scratch`: Scratch space object."},
  {NULL}
};

static PyMethodDef Database_methods[] = {
  {"compile", (PyCFunction)Database_compile, METH_VARARGS|METH_KEYWORDS,
   "compile(expressions, ids=None, elements=None, flags=0, literal=False)\n\n"
   "    Compiles regular expressions.\n\n"
   "    Args:\n"
   "        expressions (sequence of :obj:`str`): A sequence of regular\n"
   "            expressions.\n"
   "        ids (sequence of :obj:`int`, optional): A sequence of expression\n"
   "            identifiers.\n"
   "        elements (:obj:`int`, optional): Length of the expressions\n"
   "            sequence.\n"
   "        flags (sequence of :obj:`int` or :obj:`int`, optional): Sequence\n"
   "            of flags associated with each expression, or a single value\n"
   "            which is applied to all expressions.\n"
   "        literal (bool, optional): If True, uses the pure literal\n"
   "            expression compiler introduced in Hyperscan 5.2.0.\n\n"},
  {"info", (PyCFunction)Database_info, METH_VARARGS,
   "info()\n\n"
   "    Returns database information.\n\n"
   "    Returns:\n"
   "        str: Provides version and platform information for the\n"
   "        database.\n\n"},
  {"size", (PyCFunction)Database_size, METH_VARARGS,
   "size()\n\n"
   "    Returns the size of the database in bytes.\n\n"
   "    Returns:\n"
   "        int: The size of the database in bytes.\n\n"},
  {"scan", (PyCFunction)Database_scan, METH_VARARGS|METH_KEYWORDS,
   "scan(data, match_event_handler, flags=0, context=None, scratch=None)\n\n"
   "    Scans a block of text.\n\n"
   "    Args:\n"
   "        data (:obj:`str`): The block of text to scan.\n"
   "        match_event_handler (callable): The match callback, which is\n"
   "            invoked for each match result, and passed the expression id,\n"
   "            start offset, end offset, flags, and a context object.\n"
   "        flags (:obj:`int`): Currently unused.\n"
   "        context (:obj:`object`): A context object passed as the last arg\n"
   "            to **match_event_handler**.\n"
   "        scratch (:class:`~.Scratch`): A scratch object.\n\n"},
  {"stream", (PyCFunction)Database_stream, METH_VARARGS|METH_KEYWORDS,
   "stream(flags=0, match_event_handler=None, context=None)\n\n"
   "    Returns a new stream context manager.\n\n"
   "    Args:\n"
   "        flags (:obj:`int`): Currently unused.\n"
   "        match_event_handler (callable, optional): The match callback,\n"
   "            which is invoked for each match result, and passed the\n"
   "            expression id, start offset, end offset, flags, and a\n"
   "            context object. Note that this callback will override\n"
   "            the match event handler defined in the\n"
   "            :class:`~.Database` instance.\n"
   "        context (:obj:`object`): A context object passed as the last arg\n"
   "            to **match_event_handler**.\n\n"},
  {NULL}
};

static PyTypeObject DatabaseType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "hyperscan.Database",         /* tp_name */
  sizeof(Database),             /* tp_basicsize */
  0,                            /* tp_itemsize */
  (destructor)Database_dealloc, /* tp_dealloc */
  0,                            /* tp_print */
  0,                            /* tp_getattr */
  0,                            /* tp_setattr */
  0,                            /* tp_reserved */
  0,                            /* tp_repr */
  0,                            /* tp_as_number */
  0,                            /* tp_as_sequence */
  0,                            /* tp_as_mapping */
  0,                            /* tp_hash  */
  0,                            /* tp_call */
  0,                            /* tp_str */
  0,                            /* tp_getattro */
  0,                            /* tp_setattro */
  0,                            /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,        /* tp_flags */
  "Database(scratch=None, mode=0)\n\n"
  "    Represents a Hyperscan database.\n\n"
  "    Args:\n"
  "        scratch (:class:`~.Scratch`, optional): Thread-specific\n"
  "            scratch space.\n"
  "        mode (:obj:`int`, optional): One of :const:`HS_MODE_BLOCK`,\n"
  "            :const:`HS_MODE_STREAM`, or :const:`HS_MODE_VECTORED`."
  "\n\n",                       /* tp_doc */
  0,                            /* tp_traverse */
  0,                            /* tp_clear */
  0,                            /* tp_richcompare */
  0,                            /* tp_weaklistoffset */
  0,                            /* tp_iter */
  0,                            /* tp_iternext */
  Database_methods,             /* tp_methods */
  Database_members,             /* tp_members */
  0,                            /* tp_getset */
  0,                            /* tp_base */
  0,                            /* tp_dict */
  0,                            /* tp_descr_get */
  0,                            /* tp_descr_set */
  0,                            /* tp_dictoffset */
  (initproc)Database_init,      /* tp_init */
  0,                            /* tp_alloc */
  Database_new,                 /* tp_new */
};

static void Stream_dealloc(Stream* self) {
  if (self->cctx != NULL)
    free(self->cctx);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* Stream_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
  Stream *self;

  self = (Stream*)type->tp_alloc(type, 0);
  if (self != NULL) {
    self->flags = 0;
    self->database = Py_None;
    self->scratch = Py_None;
  }

  return (PyObject*)self;
}

static int Stream_init(Stream *self, PyObject *args, PyObject *kwds) {
  static char *kwlist[] = {"database", "flags", "match_event_handler",
                           "context", "scratch", NULL};
  self->cctx = malloc(sizeof(py_scan_callback_ctx));
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|IOOO!", kwlist,
                                   &self->database, &self->flags,
                                   &self->cctx->callback, &self->cctx->ctx,
                                   &self->scratch, &ScratchType))
    return -1;
  if (!PyObject_IsInstance(self->database, (PyObject*)&DatabaseType)) {
    PyErr_SetString(PyExc_TypeError,
                    "database must be a hyperscan.Database instance");
    return -1;
  }
  return 0;
}

static PyObject* Stream_close(Stream *self, PyObject *args, PyObject *kwds) {
  hs_scratch_t *scratch = NULL;
  py_scan_callback_ctx cctx;
  PyObject *oscratch = Py_None, *ocallback = Py_None, *octx = Py_None;
  static char *kwlist[] = {"scratch", "match_event_handler", "context", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O!OO", kwlist,
                                   &oscratch, &ScratchType, &ocallback, &octx))
    return NULL;
  if (PyObject_Not(oscratch))
    oscratch = ((Database*)self->database)->scratch;
  cctx.callback = PyObject_IsTrue(ocallback) ? ocallback : self->cctx->callback;
  cctx.ctx = PyObject_IsTrue(octx) ? octx : self->cctx->ctx;
  if (PyObject_IsTrue(oscratch) && cctx.callback != NULL)
    scratch = ((Scratch*)oscratch)->scratch;
  else
    scratch = ((Scratch*)((Database*)self->database)->scratch)->scratch;

  hs_error_t err = hs_close_stream(self->identifier, scratch, match_handler,
                                   (void*)&cctx);
  HANDLE_HYPERSCAN_ERR(err, NULL);
  Py_RETURN_NONE;
}

static long Stream_len(PyObject *self) {
  size_t stream_size;
  Stream *stream = (Stream*)self;
  hs_error_t err = hs_stream_size(((Database*)stream->database)->db, &stream_size);
  HANDLE_HYPERSCAN_ERR(err, 0);
  return stream_size;
}

static PyObject* Stream_enter(Stream *self) {
  hs_error_t err = hs_open_stream(((Database*)self->database)->db,
                                  0, &self->identifier);
  HANDLE_HYPERSCAN_ERR(err, NULL);
  return (PyObject*)self;
}

static PyObject* Stream_exit(Stream *self) {
  PyObject_CallMethod((PyObject*)self, "close", NULL);
  if (PyErr_Occurred())
    return NULL;
  Py_RETURN_NONE;
}

static PyObject* Stream_scan(Stream *self, PyObject *args, PyObject *kwds) {
  char *data;
  Py_ssize_t length;
  hs_error_t err;
  unsigned int flags;
  PyObject *ocallback = Py_None, *octx = Py_None, *oscratch = Py_None;
  hs_scratch_t *scratch = NULL;

  static char *kwlist[] = {"data", "flags", "scratch", "match_event_handler",
                           "context", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "s#|IOOO", kwlist,
                                   &data, &length, &flags, &oscratch,
                                   &ocallback, &octx))
    return NULL;

  if (PyObject_Not(ocallback))
    ocallback = self->cctx->callback;
  if (PyObject_Not(octx))
    octx = self->cctx->ctx;

  if (PyObject_Not(oscratch))
    scratch = (hs_scratch_t*)((Scratch*)((Database*)self->database)->scratch)->scratch;
  else {
    if (!PyObject_IsInstance(oscratch, (PyObject*)&ScratchType)) {
      PyErr_SetString(PyExc_TypeError, "scratch must be a hyperscan.Scratch instance");
      return NULL;
    }
    scratch = ((Scratch*)oscratch)->scratch;
  }

  py_scan_callback_ctx cctx = {ocallback, octx};

  Py_BEGIN_ALLOW_THREADS;
  err = hs_scan_stream(
    self->identifier,
    data,
    length,
    flags,
    scratch,
    ocallback == Py_None ? NULL : match_handler,
    ocallback == Py_None ? NULL : (void*)&cctx
  );
  Py_END_ALLOW_THREADS;
  HANDLE_HYPERSCAN_ERR(err, NULL);
  Py_RETURN_NONE;
}

static PyMemberDef Stream_members[] = {
  {"database", T_OBJECT_EX, offsetof(Stream, database), 0,
   ":class:`~.Database`: Database instance."},
  {"flags", T_INT, offsetof(Stream, flags), 0,
   ":obj:`int`: Stream flags."},
  {NULL}
};

static PyMethodDef Stream_methods[] = {
  {"__enter__", (PyCFunction)Stream_enter, METH_NOARGS},
  {"__exit__", (PyCFunction)Stream_exit, METH_VARARGS},
  {"close", (PyCFunction)Stream_close, METH_VARARGS|METH_KEYWORDS,
   "close(scratch=None, match_event_handler=None, context=None)\n\n"
   "    Closes the stream.\n\n"
   "    Args:\n"
   "        scratch (:class:`~.Scratch`, optional): Scratch space.\n"
   "            If a **match_event_handler** is provided and a scratch\n"
   "            space is not, the scratch space of the associated\n"
   "            database will be used.\n"
   "        match_event_handler (callable, optional): The match \n"
   "            callback, which is invoked for each match result, and\n"
   "            passed the expression id, start offset, end offset,\n"
   "            flags, and a context object.\n"
   "        context (:obj:`object`, optional): A context object passed\n"
   "            as the last arg to **match_event_handler**.\n\n"},
  {"scan", (PyCFunction)Stream_scan, METH_VARARGS|METH_KEYWORDS,
   "scan(data, flags=0, scratch=None, match_event_handler, context=None)\n\n"
   "    Scans streaming text.\n\n"
   "    Args:\n"
   "        data (:obj:`str`): The block of text to scan.\n"
   "        flags (:obj:`int`, optional): Currently unused.\n"
   "        scratch (:obj:`~.Scratch`, optional): Scratch space.\n"
   "        match_event_handler (callable, optional): The match \n"
   "            callback, which is invoked for each match result, and\n"
   "            passed the expression id, start offset, end offset,\n"
   "            flags, and a context object.\n"
   "        context (:obj:`object`, optional): A context object passed\n"
   "            as the last arg to **match_event_handler**.\n\n"},
  {"size", (PyCFunction)Stream_len, METH_NOARGS,
   "Return the size of the stream state in bytes."},
  {NULL}
};

static PySequenceMethods Stream_sequence_methods = {
  Stream_len,                   /* sq_length */
};

static PyTypeObject StreamType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "hyperscan.Stream",           /* tp_name */
  sizeof(Stream),               /* tp_basicsize */
  0,                            /* tp_itemsize */
  (destructor)Stream_dealloc,   /* tp_dealloc */
  0,                            /* tp_print */
  0,                            /* tp_getattr */
  0,                            /* tp_setattr */
  0,                            /* tp_reserved */
  0,                            /* tp_repr */
  0,                            /* tp_as_number */
  &Stream_sequence_methods,     /* tp_as_sequence */
  0,                            /* tp_as_mapping */
  0,                            /* tp_hash  */
  0,                            /* tp_call */
  0,                            /* tp_str */
  0,                            /* tp_getattro */
  0,                            /* tp_setattro */
  0,                            /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,        /* tp_flags */
  "Stream(database=None, flags=0, match_event_handler=None)\n\n"
  "    Provides a context manager for scanning streams of text.\n\n"
  "    Args:\n"
  "        database (:class:`~.Database`): A database initialized with\n"
  "            :const:`HS_MODE_STREAM`.\n"
  "        flags (:obj:`int`, optional): Currently unused.\n"
  "        match_event_handler (callable, optional): The match callback,\n"
  "            which is invoked for each match result, and passed the\n"
  "            expression id, start offset, end offset, flags, and a\n"
  "            context object."
  "\n\n",                       /* tp_doc */
  0,                            /* tp_traverse */
  0,                            /* tp_clear */
  0,                            /* tp_richcompare */
  0,                            /* tp_weaklistoffset */
  0,                            /* tp_iter */
  0,                            /* tp_iternext */
  Stream_methods,               /* tp_methods */
  Stream_members,               /* tp_members */
  0,                            /* tp_getset */
  0,                            /* tp_base */
  0,                            /* tp_dict */
  0,                            /* tp_descr_get */
  0,                            /* tp_descr_set */
  0,                            /* tp_dictoffset */
  (initproc)Stream_init,        /* tp_init */
  0,                            /* tp_alloc */
  Stream_new,                   /* tp_new */
};

static void Scratch_dealloc(Scratch* self) {
  if (self->scratch != NULL)
    hs_free_scratch(self->scratch);
  Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* Scratch_set_database(Scratch *self, PyObject *args, PyObject *kwds) {
  static char *kwlist[] = {"database", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &self->database))
    return NULL;
  if (self->scratch != NULL) {
    PyErr_SetString(HyperscanError, "scratch objects cannot be re-allocated");
    return NULL;
  }
  hs_database_t *db = ((Database*)self->database)->db;
  hs_error_t err = hs_alloc_scratch(db, &self->scratch);
  HANDLE_HYPERSCAN_ERR(err, NULL);
  Py_RETURN_NONE;
}

static int Scratch_init(Scratch *self, PyObject *args, PyObject *kwds) {
  static char *kwlist[] = {"database", NULL};
  self->database = Py_None;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &self->database))
    return -1;
  if (self->database != Py_None && !Scratch_set_database(self, args, kwds))
    return -1;
  return 0;
}

static PyObject* Scratch_clone(Scratch *self) {
  PyObject *dest = PyObject_CallFunction((PyObject*)&ScratchType, NULL);
  hs_scratch_t *s1 = self->scratch;
  hs_scratch_t **s2 = &(((Scratch*)dest)->scratch);
  hs_error_t err = hs_clone_scratch(s1, s2);
  HANDLE_HYPERSCAN_ERR(err, NULL);
  return dest;
}

static PyMemberDef Scratch_members[] = {
  {"database", T_OBJECT_EX, offsetof(Scratch, database), 0,
   ":class:`~.Database`: The database associated with this scratch space."},
  {NULL}
};

static PyMethodDef Scratch_methods[] = {
  {"clone", (PyCFunction)Scratch_clone, METH_NOARGS,
   "clone()\n\n"
   "    Clones a scratch space.\n\n"
   "    Returns:\n"
   "        :class:`~.Scratch`: The cloned scratch space.\n\n"},
  {"set_database", (PyCFunction)Scratch_set_database, METH_VARARGS|METH_KEYWORDS,
   "set_database(database)\n\n"
   "    Allocates a scratch with the given database.\n\n"},
  {NULL}
};

static PyTypeObject ScratchType = {
  PyVarObject_HEAD_INIT(NULL, 0)
  "hyperscan.Scratch",          /* tp_name */
  sizeof(Scratch),              /* tp_basicsize */
  0,                            /* tp_itemsize */
  (destructor)Scratch_dealloc,  /* tp_dealloc */
  0,                            /* tp_print */
  0,                            /* tp_getattr */
  0,                            /* tp_setattr */
  0,                            /* tp_reserved */
  0,                            /* tp_repr */
  0,                            /* tp_as_number */
  0,                            /* tp_as_sequence */
  0,                            /* tp_as_mapping */
  0,                            /* tp_hash  */
  0,                            /* tp_call */
  0,                            /* tp_str */
  0,                            /* tp_getattro */
  0,                            /* tp_setattro */
  0,                            /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT |
    Py_TPFLAGS_BASETYPE,        /* tp_flags */
  "Scratch(database=None)\n\n"
  "    Represents Hyperscan 'scratch space.'\n\n"
  "    Args:\n"
  "        database (:class:`~.Database`, optional): A database instance."
  "\n\n",                       /* tp_doc */
  0,                            /* tp_traverse */
  0,                            /* tp_clear */
  0,                            /* tp_richcompare */
  0,                            /* tp_weaklistoffset */
  0,                            /* tp_iter */
  0,                            /* tp_iternext */
  Scratch_methods,              /* tp_methods */
  Scratch_members,              /* tp_members */
  0,                            /* tp_getset */
  0,                            /* tp_base */
  0,                            /* tp_dict */
  0,                            /* tp_descr_get */
  0,                            /* tp_descr_set */
  0,                            /* tp_dictoffset */
  (initproc)Scratch_init,       /* tp_init */
};

static PyObject* dumps(PyObject *self, PyObject *args, PyObject *kwds) {
  Database *database;
  char *buf;
  size_t length;
  static char *kwlist[] = {"database", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!", kwlist, &DatabaseType,
                                   &database))
    return NULL;
  hs_error_t err = hs_serialize_database(database->db, &buf, &length);
  HANDLE_HYPERSCAN_ERR(err, NULL);
  PyObject *bytes = PyBytes_FromStringAndSize(buf, length);
  if (!bytes) {
    PyErr_SetString(HyperscanError, "failed to serialize database");
    return NULL;
  }
  Py_INCREF(bytes);
  return bytes;
}

static PyObject* loads(PyObject *self, PyObject *args, PyObject *kwds) {
  char *buf;
  PyObject *obuf, *ocreatescratch = Py_True;
  static char *kwlist[] = {"buf", "create_scratch", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O|O", kwlist, &obuf,
                                   &ocreatescratch))
    return NULL;
  if (!PyByteArray_Check(obuf)) {
    PyErr_SetString(PyExc_TypeError, "buf must be a bytearray");
    return NULL;
  }
  PyObject *odatabase;
  odatabase = PyObject_CallFunctionObjArgs((PyObject*)&DatabaseType, NULL);
  Py_INCREF(odatabase);
  Py_ssize_t length = PyByteArray_Size(obuf);
  buf = PyByteArray_AsString(obuf);
  hs_error_t err = hs_deserialize_database(
    buf, length, &(((Database*)odatabase)->db));
  HANDLE_HYPERSCAN_ERR(err, NULL);
  if (PyObject_IsTrue(ocreatescratch))
    ((Database*)odatabase)->scratch = PyObject_CallFunction(
      (PyObject*)&ScratchType, "O", odatabase, 0);
  if (PyErr_Occurred())
    return NULL;
  return odatabase;
}

static PyMethodDef Hyperscan_methods[] = {
  {"dumps", (PyCFunction)dumps, METH_VARARGS|METH_KEYWORDS,
   "dumps(database)\n"
   "    Serializes a Hyperscan database.\n\n"
   "    Args:\n"
   "        database (:class:`Database`): A Hyperscan database.\n\n"
   "    Returns:\n"
   "        bytes: A serialized representation of the database.\n\n"},
  {"loads", (PyCFunction)loads, METH_VARARGS|METH_KEYWORDS,
   "loads(buf)\n"
   "    Deserializes a Hyperscan database.\n\n"
   "    Args:\n"
   "        buf (:obj:`bytearray`): A serialized Hyperscan database.\n\n"
   "    Returns:\n"
   "        :class:`Database`: The deserialized database instance.\n\n"},
  {NULL}
};

MOD_INIT(_hyperscan) {
  PyObject *m;

  MOD_DEF(m, "_hyperscan", "Hyperscan bindings for CPython.", Hyperscan_methods);

  if (!m)
    return MOD_ERROR_VAL;

  ADD_INT_CONSTANT(m, HS_ARCH_ERROR);
  ADD_INT_CONSTANT(m, HS_BAD_ALIGN);
  ADD_INT_CONSTANT(m, HS_BAD_ALLOC);
  ADD_INT_CONSTANT(m, HS_COMPILER_ERROR);
  ADD_INT_CONSTANT(m, HS_CPU_FEATURES_AVX2);
  ADD_INT_CONSTANT(m, HS_DB_MODE_ERROR);
  ADD_INT_CONSTANT(m, HS_DB_PLATFORM_ERROR);
  ADD_INT_CONSTANT(m, HS_DB_VERSION_ERROR);
  ADD_INT_CONSTANT(m, HS_FLAG_ALLOWEMPTY);
  ADD_INT_CONSTANT(m, HS_FLAG_CASELESS);
  ADD_INT_CONSTANT(m, HS_FLAG_DOTALL);
  ADD_INT_CONSTANT(m, HS_FLAG_MULTILINE);
  ADD_INT_CONSTANT(m, HS_FLAG_PREFILTER);
  ADD_INT_CONSTANT(m, HS_FLAG_SINGLEMATCH);
  ADD_INT_CONSTANT(m, HS_FLAG_SOM_LEFTMOST);
  ADD_INT_CONSTANT(m, HS_FLAG_UCP);
  ADD_INT_CONSTANT(m, HS_FLAG_UTF8);
  ADD_INT_CONSTANT(m, HS_INVALID);
  ADD_INT_CONSTANT(m, HS_MODE_BLOCK);
  ADD_INT_CONSTANT(m, HS_MODE_NOSTREAM);
  ADD_INT_CONSTANT(m, HS_MODE_SOM_HORIZON_LARGE);
  ADD_INT_CONSTANT(m, HS_MODE_SOM_HORIZON_MEDIUM);
  ADD_INT_CONSTANT(m, HS_MODE_SOM_HORIZON_SMALL);
  ADD_INT_CONSTANT(m, HS_MODE_STREAM);
  ADD_INT_CONSTANT(m, HS_MODE_VECTORED);
  ADD_INT_CONSTANT(m, HS_NOMEM);
  ADD_INT_CONSTANT(m, HS_OFFSET_PAST_HORIZON);
  ADD_INT_CONSTANT(m, HS_SCAN_TERMINATED);
  ADD_INT_CONSTANT(m, HS_SCRATCH_IN_USE);
  ADD_INT_CONSTANT(m, HS_SUCCESS);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_BDW);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_GENERIC);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_HSW);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_IVB);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_SLM);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_SNB);

  HyperscanError = PyErr_NewException("hyperscan.error", NULL, NULL);
  Py_INCREF(HyperscanError);
  PyModule_AddObject(m, "error", HyperscanError);

  if (PyType_Ready(&DatabaseType) < 0 ||
      PyType_Ready(&ScratchType) < 0 ||
      PyType_Ready(&StreamType) < 0)
    return MOD_ERROR_VAL;

  Py_INCREF(&DatabaseType);
  PyModule_AddObject(m, "Database", (PyObject*)&DatabaseType);

  ScratchType.tp_new = PyType_GenericNew;
  Py_INCREF(&ScratchType);
  PyModule_AddObject(m, "Scratch", (PyObject*)&ScratchType);

  Py_INCREF(&StreamType);
  PyModule_AddObject(m, "Stream", (PyObject*)&StreamType);

  PyModule_AddStringConstant(m, "__version__", hs_version());

  return MOD_SUCCESS_VAL(m);
}
