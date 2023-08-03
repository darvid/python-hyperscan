#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <bytesobject.h>
#include <ch.h>
#include <hs.h>
#include <stdbool.h>
#include <stdio.h>
#include <structmember.h>

#define ADD_INT_CONSTANT(module, name)                    \
  if (PyModule_AddIntConstant(module, #name, name) < 0) { \
    Py_XDECREF(module);                                   \
    return NULL;                                          \
  };
#define HANDLE_CHIMERA_ERR(err, rv)                   \
  if (err != CH_SUCCESS) {                            \
    char serr[80];                                    \
    sprintf(serr, "error code %i", err);              \
    PyGILState_STATE gstate = PyGILState_Ensure();    \
    PyErr_SetString(HyperscanErrors[abs(err)], serr); \
    PyGILState_Release(gstate);                       \
    return rv;                                        \
  }
#define HANDLE_HYPERSCAN_ERR(err, rv)                 \
  if (err != HS_SUCCESS) {                            \
    char serr[80];                                    \
    sprintf(serr, "error code %i", err);              \
    PyGILState_STATE gstate = PyGILState_Ensure();    \
    PyErr_SetString(HyperscanErrors[abs(err)], serr); \
    PyGILState_Release(gstate);                       \
    return rv;                                        \
  }
#define ADD_HYPERSCAN_ERROR(module, errors, base, name, hs_err, doc) \
  if (PyModule_AddIntConstant(module, #hs_err, hs_err) < 0) {        \
    Py_XDECREF(module);                                              \
    return NULL;                                                     \
  }                                                                  \
  PyObject *name =                                                   \
    PyErr_NewExceptionWithDoc("hyperscan." #name, doc, base, NULL);  \
  if (name == NULL) {                                                \
    Py_XDECREF(module);                                              \
    return NULL;                                                     \
  } else {                                                           \
    if (PyModule_AddObject(module, #name, name) < 0) {               \
      Py_XDECREF(module);                                            \
      return NULL;                                                   \
    }                                                                \
    errors[abs(hs_err)] = name;                                      \
  }

static PyObject *HyperscanErrors[33] = {NULL};
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
  PyObject_HEAD PyObject *scratch;
  hs_database_t *hs_db;
  ch_database_t *ch_db;
  uint32_t mode;
  uint32_t chimera;
} Database;

typedef struct {
  PyObject_HEAD hs_stream_t *identifier;
  PyObject *database;
  PyObject *scratch;
  uint32_t flags;
  py_scan_callback_ctx *cctx;
} Stream;

typedef struct {
  PyObject_HEAD PyObject *database;
  hs_scratch_t *hs_scratch;
  ch_scratch_t *ch_scratch;
} Scratch;

static int hs_match_handler(
  unsigned int id,
  long long unsigned int from,
  long long unsigned int to,
  unsigned int flags,
  void *context)
{
  py_scan_callback_ctx *cctx = context;
  PyGILState_STATE gstate;
  gstate = PyGILState_Ensure();
  PyObject *rv = PyObject_CallFunction(
    cctx->callback, "IIIIO", id, from, to, flags, cctx->ctx);
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

static int ch_match_handler(
  unsigned int id,
  long long unsigned int from,
  long long unsigned int to,
  unsigned int flags,
  unsigned int size,
  const ch_capture_t *captured,
  void *context)
{
  py_scan_callback_ctx *cctx = context;
  PyGILState_STATE gstate;

  gstate = PyGILState_Ensure();
  PyObject *ocapture = NULL;
  PyObject *ocaptured = PyList_New((Py_ssize_t)size);
  for (unsigned int i = 0; i < size; i++) {
    ocapture = Py_BuildValue(
      "(I, K, K)", captured[i].flags, captured[i].from, captured[i].to);
    PyList_SetItem(ocaptured, i, ocapture);
  }
  PyObject *rv = PyObject_CallFunction(
    cctx->callback,
    "IIIIOO",
    id,
    from,
    to,
    flags,
    (PyObject *)ocaptured,
    cctx->ctx);
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

static void Database_dealloc(Database *self)
{
  if (self->chimera) {
    ch_free_database(self->ch_db);
    ch_scratch_t *scratch = ((Scratch *)self->scratch)->ch_scratch;
    if (scratch != NULL)
      ch_free_scratch(scratch);
  } else {
    hs_free_database(self->hs_db);
    hs_scratch_t *scratch = ((Scratch *)self->scratch)->hs_scratch;
    if (scratch != NULL)
      hs_free_scratch(scratch);
  }

  Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *Database_new(
  PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  Database *self;

  self = (Database *)type->tp_alloc(type, 0);
  if (self != NULL) {
    self->mode = HS_MODE_BLOCK;
    self->chimera = 0;
  }

  return (PyObject *)self;
}

static int Database_init(Database *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"scratch", "mode", "chimera", NULL};
  self->scratch = Py_None;
  if (!PyArg_ParseTupleAndKeywords(
        args,
        kwds,
        "|OIp",
        kwlist,
        &self->scratch,
        &self->mode,
        &self->chimera))
    return -1;
  return 0;
}

static PyObject *Database_compile(
  Database *self, PyObject *args, PyObject *kwds)
{
  PyObject *oexpressions;
  PyObject *oflags = Py_None;
  PyObject *oflag = Py_None;
  PyObject *oids = Py_None;
  PyObject *oext = Py_None;
  uint32_t literal = 0;
  uint64_t elements = 0;

  static char *kwlist[] = {
    "expressions",
    "ids",
    "elements",
    "flags",
    "literal",
    "ext",
    NULL,
  };
  if (!PyArg_ParseTupleAndKeywords(
        args,
        kwds,
        "O|OKOpO",
        kwlist,
        &oexpressions,
        &oids,
        &elements,
        &oflags,
        &literal,
        &oext))
    return NULL;

  if (elements == 0) {
    Py_ssize_t expressions_size = PySequence_Size(oexpressions);
    if (expressions_size == -1) {
      PyErr_SetString(PyExc_TypeError, "expressions must be a sequence");
      return NULL;
    } else {
      elements = (uint64_t)expressions_size;
    }
  }

  PyObject *oexpr = NULL;
  PyObject *oid = NULL;
  const char **expressions;
  uint32_t *flags;
  uint32_t *ids;
  size_t *lens;
  uint32_t globalflag;

  if (self->chimera && self->ch_db != NULL)
    ch_free_database(self->ch_db);
  else if (!self->chimera && self->hs_db != NULL) {
    hs_free_database(self->hs_db);
  }

  expressions = malloc(elements * sizeof(char *));
  if (expressions == NULL)
    goto memory_error;

  flags = malloc(elements * sizeof(uint32_t));
  if (flags == NULL)
    goto memory_error;

  ids = malloc(elements * sizeof(uint32_t));
  if (ids == NULL)
    goto memory_error;

  globalflag = (oflags == Py_None ? 0 : PyLong_AsUnsignedLong(oflags));

  PyErr_Clear();

  for (uint64_t i = 0; i < elements; i++) {
    const char *expression;
    uint32_t expr_flags;
    uint32_t expr_id;

    oexpr = PySequence_ITEM(oexpressions, i);
    expression = PyBytes_AsString(oexpr);

    if (PyErr_Occurred())
      break;

    if (PyObject_IsTrue(oids)) {
      oid = PySequence_ITEM(oids, i);
      expr_id = PyLong_AsUnsignedLong(oid);
      if (PyErr_Occurred())
        break;
    } else {
      expr_id = i;
    }

    if (PySequence_Check(oflags)) {
      oflag = PySequence_ITEM(oflags, i);
      if (PyErr_Occurred())
        break;
      expr_flags = PyLong_AsUnsignedLong(oflag);
      if (PyErr_Occurred())
        break;
    } else {
      expr_flags = globalflag;
    }

    expressions[i] = expression;
    ids[i] = expr_id;
    flags[i] = expr_flags;

    Py_XDECREF(oexpr);
  }

  if (oflag != Py_None)
    Py_XDECREF(oflag);
  if (oid != Py_None)
    Py_XDECREF(oid);

  if (PyErr_Occurred()) {
    goto python_error;
  }

  struct hs_expr_ext **ext = NULL;
  hs_compile_error_t *hs_compile_err;
  ch_compile_error_t *ch_compile_err;

  if (literal) {
    if (self->chimera) {
      PyErr_Format(
        PyExc_RuntimeError,
        "chimera does not support pure literal expressions");
      goto python_error;
    }
    lens = malloc(elements * sizeof(size_t));
    if (lens == NULL)
      goto memory_error;
    for (uint64_t i = 0; i < elements; i++) {
      lens[i] = strlen(expressions[i]);
    }
    hs_error_t hs_err;
    Py_BEGIN_ALLOW_THREADS;
    hs_err = hs_compile_lit_multi(
      expressions,
      flags,
      ids,
      lens,
      elements,
      self->mode,
      NULL,
      &self->hs_db,
      &hs_compile_err);
    free(lens);
    Py_END_ALLOW_THREADS;
    free(expressions);
    free(flags);
    free(ids);

    if (hs_err != HS_SUCCESS) {
      PyErr_Format(
        HyperscanError,
        "%s (id:%d)",
        hs_compile_err->message,
        hs_compile_err->expression);
      hs_free_compile_error(hs_compile_err);
      return NULL;
    }
  } else {
    if (self->chimera) {
      ch_error_t ch_err;
      Py_BEGIN_ALLOW_THREADS;
      ch_err = ch_compile_ext_multi(
        expressions,
        flags,
        ids,
        elements,
        self->mode,
        0,
        0,
        NULL,
        &self->ch_db,
        &ch_compile_err);
      Py_END_ALLOW_THREADS;
      free(expressions);
      free(flags);
      free(ids);
      HANDLE_CHIMERA_ERR(ch_err, NULL);
    } else {
      if (oext != Py_None) {
        ext = malloc(elements * sizeof(struct hs_expr_ext *));
        for (uint64_t i = 0; i < elements; i++) {
          ext[i] = malloc(sizeof(struct hs_expr_ext));
          PyObject *oext_item = PySequence_GetItem(oext, i);
          if (oext_item == NULL) {
            PyErr_Format(
              PyExc_RuntimeError, "failed to get ext item at index: %d", i);
            goto python_error;
          }
          if (!PyArg_ParseTuple(
                oext_item,
                "KKKKII",
                &(ext[i]->flags),
                &(ext[i]->min_offset),
                &(ext[i]->max_offset),
                &(ext[i]->min_length),
                &(ext[i]->edit_distance),
                &(ext[i]->hamming_distance))) {
            PyErr_SetString(PyExc_TypeError, "invalid ext info");
            Py_XDECREF(oext_item);
            goto python_error;
          }
          Py_XDECREF(oext_item);
        }
      }
      hs_error_t hs_err;
      Py_BEGIN_ALLOW_THREADS;
      hs_err = hs_compile_ext_multi(
        expressions,
        flags,
        ids,
        (const struct hs_expr_ext *const *)ext,
        elements,
        self->mode,
        NULL,
        &self->hs_db,
        &hs_compile_err);
      Py_END_ALLOW_THREADS;

      free(expressions);
      free(flags);
      free(ids);
      if (hs_err != HS_SUCCESS) {
        PyErr_SetString(HyperscanError, hs_compile_err->message);
        hs_free_compile_error(hs_compile_err);
        return NULL;
      }
    }
    free(ext);
  }

  if (self->scratch == Py_None) {
    self->scratch =
      PyObject_CallFunction((PyObject *)&ScratchType, "O", (PyObject *)self, 0);
  }

  Scratch *scratch = ((Scratch *)self->scratch);

  if (self->chimera) {
    ch_error_t ch_err;
    if (scratch->ch_scratch != NULL) {
      ch_err = ch_free_scratch(scratch->ch_scratch);
      HANDLE_CHIMERA_ERR(ch_err, NULL);
      scratch->ch_scratch = NULL;
    }
    ch_err = ch_alloc_scratch(self->ch_db, &scratch->ch_scratch);
    HANDLE_CHIMERA_ERR(ch_err, NULL);
  } else {
    hs_error_t hs_err;
    if (scratch->hs_scratch != NULL) {
      hs_err = hs_free_scratch(scratch->hs_scratch);
      HANDLE_HYPERSCAN_ERR(hs_err, NULL);
      scratch->hs_scratch = NULL;
    }
    hs_err = hs_alloc_scratch(self->hs_db, &scratch->hs_scratch);
    HANDLE_HYPERSCAN_ERR(hs_err, NULL);
  }

  Py_RETURN_NONE;

memory_error:
  return PyErr_NoMemory();

python_error:
  free(expressions);
  free(flags);
  free(ids);
  return NULL;
}

static PyObject *Database_info(Database *self, PyObject *args)
{
  char *info;
  if (self->chimera) {
    ch_error_t ch_err = ch_database_info(self->ch_db, &info);
    HANDLE_CHIMERA_ERR(ch_err, NULL);
  } else {
    hs_error_t hs_err = hs_database_info(self->hs_db, &info);
    HANDLE_HYPERSCAN_ERR(hs_err, NULL);
  }

  PyObject *oinfo = PyBytes_FromString(info);
  Py_INCREF(oinfo);
  free(info);
  return oinfo;
}

static PyObject *Database_size(Database *self, PyObject *args)
{
  size_t database_size;
  if (self->chimera) {
    ch_error_t ch_err = ch_database_size(self->ch_db, &database_size);
    HANDLE_CHIMERA_ERR(ch_err, NULL);
  } else {
    hs_error_t hs_err = hs_database_size(self->hs_db, &database_size);
    HANDLE_HYPERSCAN_ERR(hs_err, NULL);
  }

  PyObject *odatabase_size = PyLong_FromSize_t(database_size);
  Py_INCREF(odatabase_size);
  return odatabase_size;
}

static PyObject *Database_scan(Database *self, PyObject *args, PyObject *kwds)
{
  uint32_t flags = 0;

  PyObject *odata;
  PyObject *ocallback = Py_None;
  PyObject *oscratch = Py_None;
  PyObject *octx = Py_None;

  static char *kwlist[] = {
    "data",
    "match_event_handler",
    "flags",
    "context",
    "scratch",
    NULL,
  };
  if (!PyArg_ParseTupleAndKeywords(
        args,
        kwds,
        "O|OIOO",
        kwlist,
        &odata,
        &ocallback,
        &flags,
        &octx,
        &oscratch))
    return NULL;
  py_scan_callback_ctx cctx = {ocallback, octx, 1};

  if (self->mode == HS_MODE_VECTORED) {
    char **data;
    PyObject *fast_seq;
    Py_ssize_t num_buffers;
    Py_ssize_t *lengths;

    fast_seq = PySequence_Fast(odata, "expected a sequence of buffers");
    num_buffers = PySequence_Fast_GET_SIZE(fast_seq);
    data = PyMem_RawMalloc(num_buffers * sizeof(char *));
    lengths = PyMem_RawMalloc(num_buffers * sizeof(Py_ssize_t));

    for (uint32_t i = 0; i < num_buffers; i++) {
      PyObject *o = PySequence_Fast_GET_ITEM(fast_seq, i);
      if (!PyObject_CheckBuffer(o)) {
        PyErr_SetString(
          PyExc_TypeError, "obj doesn't support buffer interface");
        break;
      }

      Py_buffer view;
      if (PyObject_GetBuffer(o, &view, PyBUF_SIMPLE) != -1) {
        data[i] = (char *)view.buf;
        lengths[i] = view.len;
      } else {
        PyErr_SetString(PyExc_BufferError, "failed to get buffer");
        break;
      }
      PyBuffer_Release(&view);
    }

    if (PyErr_Occurred()) {
      PyMem_RawFree(data);
      PyMem_RawFree(lengths);
      Py_XDECREF(fast_seq);
      return NULL;
    }

    if (self->chimera) {
      PyErr_SetString(
        PyExc_RuntimeError, "chimera does not support vectored scanning");
      return NULL;
    }

    hs_error_t hs_err;
    Py_BEGIN_ALLOW_THREADS;
    hs_err = hs_scan_vector(
      self->hs_db,
      (const char *const *)data,
      (const uint32_t *)lengths,
      num_buffers,
      flags,
      oscratch == Py_None ? ((Scratch *)self->scratch)->hs_scratch
                          : ((Scratch *)oscratch)->hs_scratch,
      ocallback == Py_None ? NULL : hs_match_handler,
      ocallback == Py_None ? NULL : (void *)&cctx);
    Py_END_ALLOW_THREADS;
    PyMem_RawFree(data);
    PyMem_RawFree(lengths);
    Py_XDECREF(fast_seq);
    HANDLE_HYPERSCAN_ERR(hs_err, NULL);
  } else {
    if (!PyBytes_CheckExact(odata)) {
      PyErr_SetString(PyExc_TypeError, "a bytes-like object is required");
      return NULL;
    }

    char *data = PyBytes_AsString(odata);
    if (data == NULL)
      return NULL;
    Py_ssize_t length = PyBytes_Size(odata);

    if (self->chimera) {
      ch_error_t ch_err;
      Py_BEGIN_ALLOW_THREADS;
      ch_err = ch_scan(
        self->ch_db,
        data,
        length,
        flags,
        oscratch == Py_None ? ((Scratch *)self->scratch)->ch_scratch
                            : ((Scratch *)oscratch)->ch_scratch,
        ocallback == Py_None ? NULL : ch_match_handler,
        NULL,
        ocallback == Py_None ? NULL : (void *)&cctx);
      Py_END_ALLOW_THREADS;
      if (PyErr_Occurred()) {
        return NULL;
      }
      HANDLE_CHIMERA_ERR(ch_err, NULL);
    } else {
      hs_error_t hs_err;
      Py_BEGIN_ALLOW_THREADS;
      hs_err = hs_scan(
        self->hs_db,
        data,
        length,
        flags,
        oscratch == Py_None ? ((Scratch *)self->scratch)->hs_scratch
                            : ((Scratch *)oscratch)->hs_scratch,
        ocallback == Py_None ? NULL : hs_match_handler,
        ocallback == Py_None ? NULL : (void *)&cctx);
      Py_END_ALLOW_THREADS;
      if (PyErr_Occurred()) {
        return NULL;
      }
      HANDLE_HYPERSCAN_ERR(hs_err, NULL);
    }
  }
  if (!cctx.success) {
    return NULL;
  }
  Py_RETURN_NONE;
}

static PyObject *Database_stream(Database *self, PyObject *args, PyObject *kwds)
{
  uint32_t flags = 0;
  PyObject *ocallback = Py_None;
  PyObject *octx = Py_None;
  static char *kwlist[] = {
    "match_event_handler",
    "flags",
    "context",
    NULL,
  };
  if (!PyArg_ParseTupleAndKeywords(
        args, kwds, "O|IO", kwlist, &ocallback, &flags, &octx))
    return NULL;
  PyObject *stream = PyObject_CallFunction(
    (PyObject *)&StreamType, "OIOO", (PyObject *)self, flags, ocallback, octx);
  if (PyErr_Occurred())
    return NULL;
  Py_INCREF(stream);
  return stream;
}

static PyMemberDef Database_members[] = {
  {"mode", T_INT, offsetof(Database, mode), 0, "int: Scanning mode."},
  {"scratch",
   T_OBJECT_EX,
   offsetof(Database, scratch),
   0,
   ":class:`Scratch`: Scratch space object."},
  {NULL}};

static PyMethodDef Database_methods[] = {
  {"compile",
   (PyCFunction)Database_compile,
   METH_VARARGS | METH_KEYWORDS,
   "compile(expressions, ids=None, elements=None, flags=0, literal=False)\n\n"
   "    Compiles regular expressions.\n\n"
   "    Args:\n"
   "        expressions (sequence of str): A sequence of regular\n"
   "            expressions.\n"
   "        ids (sequence of int, optional): A sequence of\n"
   "            expression identifiers.\n"
   "        elements (int, optional): Length of the expressions\n"
   "            sequence.\n"
   "        flags (sequence of int or int, optional):\n"
   "            Sequence of flags associated with each expression, or a\n"
   "            single value which is applied to all expressions.\n"
   "        literal (bool, optional): If True, uses the pure literal\n"
   "            expression compiler introduced in Hyperscan 5.2.0.\n\n"
   "        ext (sequence of tuple, optional): A list of tuples used to "
   "            define extended behavior for each pattern. Tuples must \n"
   "            contain **flags**, **min_offset**, **max_offset**, \n"
   "            **min_length**, **edit_distance**, and **hamming_distance**.\n"
   "            See hyperscan documentation for more information.\n\n"},
  {"info",
   (PyCFunction)Database_info,
   METH_VARARGS,
   "info()\n\n"
   "    Returns database information.\n\n"
   "    Returns:\n"
   "        str: Provides version and platform information for the\n"
   "        database.\n\n"},
  {"size",
   (PyCFunction)Database_size,
   METH_VARARGS,
   "size()\n\n"
   "    Returns the size of the database in bytes.\n\n"
   "    Returns:\n"
   "        int: The size of the database in bytes.\n\n"},
  {"scan",
   (PyCFunction)Database_scan,
   METH_VARARGS | METH_KEYWORDS,
   "scan(data, match_event_handler, flags=0, context=None, scratch=None)\n\n"
   "    Scans a block of text.\n\n"
   "    Args:\n"
   "        data (str): The block of text to scan, if the database\n"
   "            was opened with streaming or block mode, or a list of\n"
   "            buffers (i.e. :obj:`bytearray`) if the database was\n"
   "            opened with vectored mode.\n"
   "        match_event_handler (callable): The match callback, which is\n"
   "            invoked for each match result, and passed the expression\n"
   "            id, start offset, end offset, flags, and a context object.\n"
   "        flags (int): Currently unused.\n"
   "        context (:obj:`object`): A context object passed as the last\n"
   "            arg to **match_event_handler**.\n"
   "        scratch (:class:`Scratch`): A scratch object.\n\n"},
  {"stream",
   (PyCFunction)Database_stream,
   METH_VARARGS | METH_KEYWORDS,
   "stream(match_event_handler=None, flags=0, context=None)\n\n"
   "    Returns a new stream context manager.\n\n"
   "    Args:\n"
   "        match_event_handler (callable, optional): The match callback,\n"
   "            which is invoked for each match result, and passed the\n"
   "            expression id, start offset, end offset, flags, and a\n"
   "            context object. Note that this callback will override\n"
   "            the match event handler defined in the\n"
   "            :class:`Database` instance.\n"
   "        flags (int): Currently unused.\n"
   "        context (:obj:`object`): A context object passed as the last\n"
   "            arg to **match_event_handler**.\n\n"},
  {NULL}};

static PyTypeObject DatabaseType = {
  PyVarObject_HEAD_INIT(NULL, 0) "hyperscan.Database", /* tp_name */
  sizeof(Database),                                    /* tp_basicsize */
  0,                                                   /* tp_itemsize */
  (destructor)Database_dealloc,                        /* tp_dealloc */
  0,                                                   /* tp_print */
  0,                                                   /* tp_getattr */
  0,                                                   /* tp_setattr */
  0,                                                   /* tp_reserved */
  0,                                                   /* tp_repr */
  0,                                                   /* tp_as_number */
  0,                                                   /* tp_as_sequence */
  0,                                                   /* tp_as_mapping */
  0,                                                   /* tp_hash  */
  0,                                                   /* tp_call */
  0,                                                   /* tp_str */
  0,                                                   /* tp_getattro */
  0,                                                   /* tp_setattro */
  0,                                                   /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,            /* tp_flags */
  "Database(scratch=None, mode=0)\n\n"
  "    Represents a Hyperscan database.\n\n"
  "    Args:\n"
  "        scratch (:class:`Scratch`, optional): Thread-specific\n"
  "            scratch space.\n"
  "        mode (int, optional): One of :const:`HS_MODE_BLOCK`,\n"
  "            :const:`HS_MODE_STREAM`, or :const:`HS_MODE_VECTORED`.\n"
  "        chimera (bool): Enable Chimera support."
  "\n\n",                  /* tp_doc */
  0,                       /* tp_traverse */
  0,                       /* tp_clear */
  0,                       /* tp_richcompare */
  0,                       /* tp_weaklistoffset */
  0,                       /* tp_iter */
  0,                       /* tp_iternext */
  Database_methods,        /* tp_methods */
  Database_members,        /* tp_members */
  0,                       /* tp_getset */
  0,                       /* tp_base */
  0,                       /* tp_dict */
  0,                       /* tp_descr_get */
  0,                       /* tp_descr_set */
  0,                       /* tp_dictoffset */
  (initproc)Database_init, /* tp_init */
  0,                       /* tp_alloc */
  Database_new,            /* tp_new */
};

static void Stream_dealloc(Stream *self)
{
  if (self->cctx != NULL)
    free(self->cctx);
  Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *Stream_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
  Stream *self;

  self = (Stream *)type->tp_alloc(type, 0);
  if (self != NULL) {
    self->flags = 0;
    self->database = Py_None;
    self->scratch = Py_None;
  }

  return (PyObject *)self;
}

static int Stream_init(Stream *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {
    "database",
    "flags",
    "match_event_handler",
    "context",
    "scratch",
    NULL,
  };
  self->cctx = malloc(sizeof(py_scan_callback_ctx));
  if (!PyArg_ParseTupleAndKeywords(
        args,
        kwds,
        "O|IOOO!",
        kwlist,
        &self->database,
        &self->flags,
        &self->cctx->callback,
        &self->cctx->ctx,
        &self->scratch,
        &ScratchType))
    return -1;
  if (!PyObject_IsInstance(self->database, (PyObject *)&DatabaseType)) {
    PyErr_SetString(
      PyExc_TypeError, "database must be a hyperscan.Database instance");
    return -1;
  }
  return 0;
}

static PyObject *Stream_close(Stream *self, PyObject *args, PyObject *kwds)
{
  py_scan_callback_ctx cctx;
  PyObject *oscratch = Py_None, *ocallback = Py_None, *octx = Py_None;
  static char *kwlist[] = {"scratch", "match_event_handler", "context", NULL};
  if (!PyArg_ParseTupleAndKeywords(
        args,
        kwds,
        "|O!OO",
        kwlist,
        &oscratch,
        &ScratchType,
        &ocallback,
        &octx))
    return NULL;
  Database *db = (Database *)self->database;
  Scratch *scratch;
  if (PyObject_Not(oscratch))
    oscratch = ((Database *)self->database)->scratch;
  cctx.callback = PyObject_IsTrue(ocallback) ? ocallback : self->cctx->callback;
  cctx.ctx = PyObject_IsTrue(octx) ? octx : self->cctx->ctx;
  if (PyObject_IsTrue(oscratch) && cctx.callback != NULL)
    scratch = (Scratch *)oscratch;
  else
    scratch = (Scratch *)db->scratch;

  hs_scratch_t *hs_scratch = scratch->hs_scratch;
  hs_error_t hs_err = hs_close_stream(
    self->identifier, hs_scratch, hs_match_handler, (void *)&cctx);
  HANDLE_HYPERSCAN_ERR(hs_err, NULL);

  Py_RETURN_NONE;
}

static long Stream_len(PyObject *self)
{
  size_t stream_size;
  Stream *stream = (Stream *)self;
  Database *db = (Database *)stream->database;
  if (db->chimera) {
    PyErr_SetString(PyExc_RuntimeError, "chimera does not support streams");
    return 0;
  }
  hs_error_t err = hs_stream_size(db->hs_db, &stream_size);
  HANDLE_HYPERSCAN_ERR(err, 0);
  return stream_size;
}

static PyObject *Stream_enter(Stream *self)
{
  Stream *stream = (Stream *)self;
  Database *db = (Database *)stream->database;
  if (db->chimera) {
    PyErr_SetString(PyExc_RuntimeError, "chimera does not support streams");
    return NULL;
  }
  hs_error_t err = hs_open_stream(db->hs_db, 0, &self->identifier);
  HANDLE_HYPERSCAN_ERR(err, NULL);
  return (PyObject *)self;
}

static PyObject *Stream_exit(Stream *self)
{
  PyObject_CallMethod((PyObject *)self, "close", NULL);
  if (PyErr_Occurred())
    return NULL;
  Py_RETURN_NONE;
}

static PyObject *Stream_scan(Stream *self, PyObject *args, PyObject *kwds)
{
  char *data;
  Py_ssize_t length;
  uint32_t flags;
  PyObject *ocallback = Py_None, *octx = Py_None, *oscratch = Py_None;

  static char *kwlist[] = {
    "data", "flags", "scratch", "match_event_handler", "context", NULL};
  if (!PyArg_ParseTupleAndKeywords(
        args,
        kwds,
        "s#|IOOO",
        kwlist,
        &data,
        &length,
        &flags,
        &oscratch,
        &ocallback,
        &octx))
    return NULL;

  if (PyObject_Not(ocallback))
    ocallback = self->cctx->callback;
  if (PyObject_Not(octx))
    octx = self->cctx->ctx;

  Database *db = (Database *)self->database;
  Scratch *scratch;

  if (PyObject_Not(oscratch))
    scratch = (Scratch *)db->scratch;
  else {
    if (!PyObject_IsInstance(oscratch, (PyObject *)&ScratchType)) {
      PyErr_SetString(
        PyExc_TypeError, "scratch must be a hyperscan.Scratch instance");
      return NULL;
    }
    scratch = (Scratch *)oscratch;
  }

  py_scan_callback_ctx cctx = {ocallback, octx};

  if (db->chimera) {
    PyErr_SetString(PyExc_RuntimeError, "chimera does not support streams");
    return NULL;
  } else {
    hs_error_t hs_err;
    Py_BEGIN_ALLOW_THREADS;
    hs_err = hs_scan_stream(
      self->identifier,
      data,
      length,
      flags,
      scratch->hs_scratch,
      ocallback == Py_None ? NULL : hs_match_handler,
      ocallback == Py_None ? NULL : (void *)&cctx);
    Py_END_ALLOW_THREADS;
    HANDLE_HYPERSCAN_ERR(hs_err, NULL);
  }

  Py_RETURN_NONE;
}

static PyMemberDef Stream_members[] = {
  {"database",
   T_OBJECT_EX,
   offsetof(Stream, database),
   0,
   ":class:`Database`: Database instance."},
  {"flags", T_INT, offsetof(Stream, flags), 0, "int: Stream flags."},
  {NULL}};

static PyMethodDef Stream_methods[] = {
  {"__enter__", (PyCFunction)Stream_enter, METH_NOARGS},
  {"__exit__", (PyCFunction)Stream_exit, METH_VARARGS},
  {"close",
   (PyCFunction)Stream_close,
   METH_VARARGS | METH_KEYWORDS,
   "close(scratch=None, match_event_handler=None, context=None)\n\n"
   "    Closes the stream.\n\n"
   "    Args:\n"
   "        scratch (:class:`Scratch`, optional): Scratch space.\n"
   "            If a **match_event_handler** is provided and a scratch\n"
   "            space is not, the scratch space of the associated\n"
   "            database will be used.\n"
   "        match_event_handler (callable, optional): The match \n"
   "            callback, which is invoked for each match result, and\n"
   "            passed the expression id, start offset, end offset,\n"
   "            flags, and a context object.\n"
   "        context (:obj:`object`, optional): A context object passed\n"
   "            as the last arg to **match_event_handler**.\n\n"},
  {"scan",
   (PyCFunction)Stream_scan,
   METH_VARARGS | METH_KEYWORDS,
   "scan(data, flags=0, scratch=None, match_event_handler=None, "
   "context=None)\n\n"
   "    Scans streaming text.\n\n"
   "    Args:\n"
   "        data (str): The block of text to scan.\n"
   "        flags (int, optional): Currently unused.\n"
   "        scratch (:obj:`Scratch`, optional): Scratch space.\n"
   "        match_event_handler (callable, optional): The match \n"
   "            callback, which is invoked for each match result, and\n"
   "            passed the expression id, start offset, end offset,\n"
   "            flags, and a context object.\n"
   "        context (:obj:`object`, optional): A context object passed\n"
   "            as the last arg to **match_event_handler**.\n\n"},
  {"size",
   (PyCFunction)Stream_len,
   METH_NOARGS,
   "Return the size of the stream state in bytes."},
  {NULL}};

static PySequenceMethods Stream_sequence_methods = {
  Stream_len, /* sq_length */
};

static PyTypeObject StreamType = {
  PyVarObject_HEAD_INIT(NULL, 0) "hyperscan.Stream", /* tp_name */
  sizeof(Stream),                                    /* tp_basicsize */
  0,                                                 /* tp_itemsize */
  (destructor)Stream_dealloc,                        /* tp_dealloc */
  0,                                                 /* tp_print */
  0,                                                 /* tp_getattr */
  0,                                                 /* tp_setattr */
  0,                                                 /* tp_reserved */
  0,                                                 /* tp_repr */
  0,                                                 /* tp_as_number */
  &Stream_sequence_methods,                          /* tp_as_sequence */
  0,                                                 /* tp_as_mapping */
  0,                                                 /* tp_hash  */
  0,                                                 /* tp_call */
  0,                                                 /* tp_str */
  0,                                                 /* tp_getattro */
  0,                                                 /* tp_setattro */
  0,                                                 /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,          /* tp_flags */
  "Stream(database=None, flags=0, match_event_handler=None)\n\n"
  "    Provides a context manager for scanning streams of text.\n\n"
  "    Args:\n"
  "        database (:class:`Database`): A database initialized with\n"
  "            :const:`HS_MODE_STREAM`.\n"
  "        flags (int, optional): Currently unused.\n"
  "        match_event_handler (callable, optional): The match callback,\n"
  "            which is invoked for each match result, and passed the\n"
  "            expression id, start offset, end offset, flags, and a\n"
  "            context object."
  "\n\n",                /* tp_doc */
  0,                     /* tp_traverse */
  0,                     /* tp_clear */
  0,                     /* tp_richcompare */
  0,                     /* tp_weaklistoffset */
  0,                     /* tp_iter */
  0,                     /* tp_iternext */
  Stream_methods,        /* tp_methods */
  Stream_members,        /* tp_members */
  0,                     /* tp_getset */
  0,                     /* tp_base */
  0,                     /* tp_dict */
  0,                     /* tp_descr_get */
  0,                     /* tp_descr_set */
  0,                     /* tp_dictoffset */
  (initproc)Stream_init, /* tp_init */
  0,                     /* tp_alloc */
  Stream_new,            /* tp_new */
};

static void Scratch_dealloc(Scratch *self)
{
  if (self->hs_scratch != NULL)
    hs_free_scratch(self->hs_scratch);
  if (self->ch_scratch != NULL)
    ch_free_scratch(self->ch_scratch);
  Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *Scratch_set_database(
  Scratch *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"database", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &self->database))
    return NULL;
  if (self->hs_scratch != NULL || self->ch_scratch != NULL) {
    PyErr_SetString(HyperscanError, "scratch objects cannot be re-allocated");
    return NULL;
  }
  Database *db = (Database *)self->database;
  if (db->chimera) {
    ch_database_t *ch_db = db->ch_db;
    ch_error_t ch_err = ch_alloc_scratch(ch_db, &self->ch_scratch);
    HANDLE_CHIMERA_ERR(ch_err, NULL);
  } else {
    hs_database_t *hs_db = db->hs_db;
    hs_error_t hs_err = hs_alloc_scratch(hs_db, &self->hs_scratch);
    HANDLE_HYPERSCAN_ERR(hs_err, NULL);
  }
  Py_RETURN_NONE;
}

static int Scratch_init(Scratch *self, PyObject *args, PyObject *kwds)
{
  static char *kwlist[] = {"database", NULL};
  self->database = Py_None;
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "|O", kwlist, &self->database))
    return -1;
  if (self->database != Py_None && !Scratch_set_database(self, args, kwds))
    return -1;
  return 0;
}

static PyObject *Scratch_clone(Scratch *self)
{
  PyObject *odest = PyObject_CallFunction((PyObject *)&ScratchType, NULL);
  Scratch *dest = (Scratch *)odest;
  bool chimera;
  if (self->database == Py_None) {
    // XXX: Assume chimera mode is false if no db
    chimera = false;
  } else {
    chimera = ((Database *)self->database)->chimera;
  }

  if (chimera) {
    ch_error_t ch_err = ch_clone_scratch(self->ch_scratch, &dest->ch_scratch);
    HANDLE_CHIMERA_ERR(ch_err, NULL);
  } else {
    hs_error_t hs_err = hs_clone_scratch(self->hs_scratch, &dest->hs_scratch);
    HANDLE_HYPERSCAN_ERR(hs_err, NULL);
  }

  return odest;
}

static PyMemberDef Scratch_members[] = {
  {"database",
   T_OBJECT_EX,
   offsetof(Scratch, database),
   0,
   ":class:`Database`: The database associated with this scratch space."},
  {NULL}};

static PyMethodDef Scratch_methods[] = {
  {"clone",
   (PyCFunction)Scratch_clone,
   METH_NOARGS,
   "clone()\n\n"
   "    Clones a scratch space.\n\n"
   "    Returns:\n"
   "        :class:`Scratch`: The cloned scratch space.\n\n"},
  {"set_database",
   (PyCFunction)Scratch_set_database,
   METH_VARARGS | METH_KEYWORDS,
   "set_database(database)\n\n"
   "    Allocates a scratch with the given database.\n\n"},
  {NULL}};

static PyTypeObject ScratchType = {
  PyVarObject_HEAD_INIT(NULL, 0) "hyperscan.Scratch", /* tp_name */
  sizeof(Scratch),                                    /* tp_basicsize */
  0,                                                  /* tp_itemsize */
  (destructor)Scratch_dealloc,                        /* tp_dealloc */
  0,                                                  /* tp_print */
  0,                                                  /* tp_getattr */
  0,                                                  /* tp_setattr */
  0,                                                  /* tp_reserved */
  0,                                                  /* tp_repr */
  0,                                                  /* tp_as_number */
  0,                                                  /* tp_as_sequence */
  0,                                                  /* tp_as_mapping */
  0,                                                  /* tp_hash  */
  0,                                                  /* tp_call */
  0,                                                  /* tp_str */
  0,                                                  /* tp_getattro */
  0,                                                  /* tp_setattro */
  0,                                                  /* tp_as_buffer */
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,           /* tp_flags */
  "Scratch(database=None)\n\n"
  "    Represents Hyperscan 'scratch space.'\n\n"
  "    Args:\n"
  "        database (:class:`Database`, optional): A database instance."
  "\n\n",                 /* tp_doc */
  0,                      /* tp_traverse */
  0,                      /* tp_clear */
  0,                      /* tp_richcompare */
  0,                      /* tp_weaklistoffset */
  0,                      /* tp_iter */
  0,                      /* tp_iternext */
  Scratch_methods,        /* tp_methods */
  Scratch_members,        /* tp_members */
  0,                      /* tp_getset */
  0,                      /* tp_base */
  0,                      /* tp_dict */
  0,                      /* tp_descr_get */
  0,                      /* tp_descr_set */
  0,                      /* tp_dictoffset */
  (initproc)Scratch_init, /* tp_init */
};

static PyObject *dumpb(PyObject *self, PyObject *args, PyObject *kwds)
{
  Database *db;
  char *buf;
  size_t length;
  static char *kwlist[] = {"database", NULL};

  if (!PyArg_ParseTupleAndKeywords(
        args, kwds, "O!", kwlist, &DatabaseType, &db))
    return NULL;
  if (db->chimera) {
    PyErr_SetString(
      PyExc_RuntimeError, "chimera does not support serialization");
    return NULL;
  }
  hs_error_t err = hs_serialize_database(db->hs_db, &buf, &length);
  HANDLE_HYPERSCAN_ERR(err, NULL);
  PyObject *bytes = PyBytes_FromStringAndSize(buf, length);
  if (!bytes) {
    PyErr_SetString(HyperscanError, "failed to serialize database");
    return NULL;
  }
  PyMem_Free(buf);
  return bytes;
}

static PyObject *loadb(PyObject *self, PyObject *args, PyObject *kwds)
{
  char *buf;
  PyObject *obuf = Py_None;
  PyObject *odb;
  odb = PyObject_CallFunctionObjArgs((PyObject *)&DatabaseType, NULL);
  Database *db = (Database *)odb;

  static char *kwlist[] = {"buf", "mode", NULL};
  if (!PyArg_ParseTupleAndKeywords(args, kwds, "OI", kwlist, &obuf, &db->mode))
    return NULL;
  if (!PyBytes_Check(obuf)) {
    PyErr_SetString(PyExc_TypeError, "buf must be a bytestring");
    return NULL;
  }

  Py_ssize_t length = PyBytes_Size(obuf);
  buf = PyBytes_AsString(obuf);
  hs_error_t err = hs_deserialize_database(buf, length, &db->hs_db);
  HANDLE_HYPERSCAN_ERR(err, NULL);
  if (PyErr_Occurred())
    return NULL;
  return odb;
}

static PyMethodDef HyperscanMethods[] = {
  {"dumpb",
   (PyCFunction)dumpb,
   METH_VARARGS | METH_KEYWORDS,
   "dumpb(database)\n"
   "    Serializes a Hyperscan database.\n\n"
   "    Args:\n"
   "        database (:class:`Database`): A Hyperscan database.\n\n"
   "    Returns:\n"
   "        bytes: A serialized representation of the database.\n\n"},
  {"loadb",
   (PyCFunction)loadb,
   METH_VARARGS | METH_KEYWORDS,
   "loadb(buf, mode)\n"
   "    Deserializes a Hyperscan database.\n\n"
   "    Args:\n"
   "        buf (:obj:`bytearray`): A serialized Hyperscan database.\n"
   "        mode (int): The expected mode of the database.\n\n"
   "    Returns:\n"
   "        :class:`Database`: The deserialized database instance.\n\n"},
  {NULL}};

static struct PyModuleDef hyperscanmodule = {
  PyModuleDef_HEAD_INIT,
  "_ext",
  "Hyperscan bindings for CPython.",
  -1,
  HyperscanMethods,
};

PyMODINIT_FUNC PyInit__ext(void)
{
  PyObject *m;

  m = PyModule_Create(&hyperscanmodule);
  if (m == NULL)
    return NULL;

  ADD_INT_CONSTANT(m, CH_BAD_ALIGN);
  ADD_INT_CONSTANT(m, CH_BAD_ALLOC);
  ADD_INT_CONSTANT(m, CH_COMPILER_ERROR);
  ADD_INT_CONSTANT(m, CH_DB_MODE_ERROR);
  ADD_INT_CONSTANT(m, CH_DB_PLATFORM_ERROR);
  ADD_INT_CONSTANT(m, CH_DB_VERSION_ERROR);
  ADD_INT_CONSTANT(m, CH_FAIL_INTERNAL);
  ADD_INT_CONSTANT(m, CH_FLAG_CASELESS);
  ADD_INT_CONSTANT(m, CH_FLAG_DOTALL);
  ADD_INT_CONSTANT(m, CH_FLAG_MULTILINE);
  ADD_INT_CONSTANT(m, CH_FLAG_SINGLEMATCH);
  ADD_INT_CONSTANT(m, CH_FLAG_UCP);
  ADD_INT_CONSTANT(m, CH_FLAG_UTF8);
  ADD_INT_CONSTANT(m, CH_INVALID);
  ADD_INT_CONSTANT(m, CH_MODE_GROUPS);
  ADD_INT_CONSTANT(m, CH_MODE_NOGROUPS);
  ADD_INT_CONSTANT(m, CH_NOMEM);
  ADD_INT_CONSTANT(m, CH_SCAN_TERMINATED);
  ADD_INT_CONSTANT(m, CH_SCRATCH_IN_USE);
  ADD_INT_CONSTANT(m, CH_SUCCESS);
  ADD_INT_CONSTANT(m, HS_CPU_FEATURES_AVX2);
  ADD_INT_CONSTANT(m, HS_EXT_FLAG_EDIT_DISTANCE);
  ADD_INT_CONSTANT(m, HS_EXT_FLAG_HAMMING_DISTANCE);
  ADD_INT_CONSTANT(m, HS_EXT_FLAG_MAX_OFFSET);
  ADD_INT_CONSTANT(m, HS_EXT_FLAG_MIN_LENGTH);
  ADD_INT_CONSTANT(m, HS_EXT_FLAG_MIN_OFFSET);
  ADD_INT_CONSTANT(m, HS_FLAG_ALLOWEMPTY);
  ADD_INT_CONSTANT(m, HS_FLAG_CASELESS);
  ADD_INT_CONSTANT(m, HS_FLAG_COMBINATION);
  ADD_INT_CONSTANT(m, HS_FLAG_DOTALL);
  ADD_INT_CONSTANT(m, HS_FLAG_MULTILINE);
  ADD_INT_CONSTANT(m, HS_FLAG_PREFILTER);
  ADD_INT_CONSTANT(m, HS_FLAG_QUIET);
  ADD_INT_CONSTANT(m, HS_FLAG_SINGLEMATCH);
  ADD_INT_CONSTANT(m, HS_FLAG_SOM_LEFTMOST);
  ADD_INT_CONSTANT(m, HS_FLAG_UCP);
  ADD_INT_CONSTANT(m, HS_FLAG_UTF8);
  ADD_INT_CONSTANT(m, HS_MODE_BLOCK);
  ADD_INT_CONSTANT(m, HS_MODE_NOSTREAM);
  ADD_INT_CONSTANT(m, HS_MODE_SOM_HORIZON_LARGE);
  ADD_INT_CONSTANT(m, HS_MODE_SOM_HORIZON_MEDIUM);
  ADD_INT_CONSTANT(m, HS_MODE_SOM_HORIZON_SMALL);
  ADD_INT_CONSTANT(m, HS_MODE_STREAM);
  ADD_INT_CONSTANT(m, HS_MODE_VECTORED);
  ADD_INT_CONSTANT(m, HS_OFFSET_PAST_HORIZON);
  ADD_INT_CONSTANT(m, HS_SCRATCH_IN_USE);
  ADD_INT_CONSTANT(m, HS_SUCCESS);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_BDW);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_GENERIC);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_GLM);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_HSW);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_IVB);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_SKL);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_SKX);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_SLM);
  ADD_INT_CONSTANT(m, HS_TUNE_FAMILY_SNB);

  HyperscanError = PyErr_NewExceptionWithDoc(
    "hyperscan.error",
    "Base exception class for Hyperscan errors.",
    NULL,
    NULL);
  if (!HyperscanError) {
    goto cleanup_module;
  }
  Py_XINCREF(HyperscanError);
  if (PyModule_AddObject(m, "error", HyperscanError) < 0) {
    Py_XDECREF(HyperscanError);
    Py_CLEAR(HyperscanError);
    goto cleanup_module;
  }
  if (PyModule_AddObject(m, "HyperscanError", HyperscanError) < 0) {
    Py_XDECREF(HyperscanError);
    Py_CLEAR(HyperscanError);
    goto cleanup_module;
  }

  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    InvalidError,
    HS_INVALID,
    "Parameter passed to this function was invalid.");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    NoMemoryError,
    HS_NOMEM,
    "Memory allocation failed.");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    CompilerError,
    HS_COMPILER_ERROR,
    "Pattern compilation failed.");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    ScanTerminated,
    HS_SCAN_TERMINATED,
    "The engine was terminated by callback.");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    DatabaseVersionError,
    HS_DB_VERSION_ERROR,
    "The given database was built for a different version of Hyperscan.");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    DatabasePlatformError,
    HS_DB_PLATFORM_ERROR,
    "The given database was built for a different platform (i.e., CPU type).");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    DatabaseModeError,
    HS_DB_MODE_ERROR,
    "The given database was built for a different mode of operation.");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    BadAlignError,
    HS_BAD_ALIGN,
    "A parameter passed to this function was not correctly aligned.");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    BadAllocationError,
    HS_BAD_ALLOC,
    "The memory allocator failed.");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    ScratchInUseError,
    HS_SCRATCH_IN_USE,
    "The scratch region was already in use.");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    ArchitectureError,
    HS_ARCH_ERROR,
    "Unsupported CPU architecture.");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    InsufficientSpaceError,
    HS_INSUFFICIENT_SPACE,
    "Provided buffer was too small.");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    UnknownError,
    HS_UNKNOWN_ERROR,
    "Unexpected internal error.");
  ADD_HYPERSCAN_ERROR(
    m,
    HyperscanErrors,
    HyperscanError,
    InternalPCREError,
    CH_FAIL_INTERNAL,
    "Unexpected internal error.");

  if (
    (PyType_Ready(&DatabaseType) < 0) || (PyType_Ready(&ScratchType) < 0) ||
    (PyType_Ready(&StreamType) < 0)) {
    goto cleanup_module;
  }

  Py_XINCREF(&DatabaseType);
  if (PyModule_AddObject(m, "Database", (PyObject *)&DatabaseType) < 0) {
    Py_XDECREF(&DatabaseType);
    goto cleanup_module;
  }

  ScratchType.tp_new = PyType_GenericNew;
  Py_XINCREF(&ScratchType);
  if (PyModule_AddObject(m, "Scratch", (PyObject *)&ScratchType) < 0) {
    Py_XDECREF(&ScratchType);
    goto cleanup_module;
  }

  Py_XINCREF(&StreamType);
  if (PyModule_AddObject(m, "Stream", (PyObject *)&StreamType) < 0) {
    Py_XDECREF(&StreamType);
    goto cleanup_module;
  }

  if (PyModule_AddStringConstant(m, "__version__", hs_version()) < 0) {
    goto cleanup_module;
  }

  return m;

cleanup_module:
  Py_DECREF(m);
  return NULL;
}
