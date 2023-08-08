from typing import (
    AnyStr,
    ByteString,
    Callable,
    Optional,
    Self,
    Sequence,
    Tuple,
    TypeAlias,
    Union,
)

CH_BAD_ALIGN = -8
CH_BAD_ALLOC = -9
CH_COMPILER_ERROR = -4
CH_DB_MODE_ERROR = -7
CH_DB_PLATFORM_ERROR = -6
CH_DB_VERSION_ERROR = -5
CH_FAIL_INTERNAL = -32
CH_FLAG_CASELESS = 1
CH_FLAG_DOTALL = 2
CH_FLAG_MULTILINE = 4
CH_FLAG_SINGLEMATCH = 8
CH_FLAG_UCP = 64
CH_FLAG_UTF8 = 32
CH_INVALID = -1
CH_MODE_GROUPS = 1048576
CH_MODE_NOGROUPS = 0
CH_NOMEM = -2
CH_SCAN_TERMINATED = -3
CH_SCRATCH_IN_USE = -10
CH_SUCCESS = 0
HS_CPU_FEATURES_AVX2 = 4
HS_EXT_FLAG_EDIT_DISTANCE = 8
HS_EXT_FLAG_HAMMING_DISTANCE = 16
HS_EXT_FLAG_MAX_OFFSET = 2
HS_EXT_FLAG_MIN_LENGTH = 4
HS_EXT_FLAG_MIN_OFFSET = 1
HS_FLAG_ALLOWEMPTY = 16
HS_FLAG_CASELESS = 1
HS_FLAG_COMBINATION = 512
HS_FLAG_DOTALL = 2
HS_FLAG_MULTILINE = 4
HS_FLAG_PREFILTER = 128
HS_FLAG_QUIET = 1024
HS_FLAG_SINGLEMATCH = 8
HS_FLAG_SOM_LEFTMOST = 256
HS_FLAG_UCP = 64
HS_FLAG_UTF8 = 32
HS_MODE_BLOCK = 1
HS_MODE_NOSTREAM = 1
HS_MODE_SOM_HORIZON_LARGE = 16777216
HS_MODE_SOM_HORIZON_MEDIUM = 33554432
HS_MODE_SOM_HORIZON_SMALL = 67108864
HS_MODE_STREAM = 2
HS_MODE_VECTORED = 4
HS_OFFSET_PAST_HORIZON = -1
HS_SCRATCH_IN_USE = -10
HS_SUCCESS = 0
HS_TUNE_FAMILY_BDW = 5
HS_TUNE_FAMILY_GENERIC = 0
HS_TUNE_FAMILY_GLM = 8
HS_TUNE_FAMILY_HSW = 3
HS_TUNE_FAMILY_IVB = 2
HS_TUNE_FAMILY_SKL = 6
HS_TUNE_FAMILY_SKX = 7
HS_TUNE_FAMILY_SLM = 4
HS_TUNE_FAMILY_SNB = 1

match_event_callback = Callable[[int, int, int, int, object], Optional[bool]]

def dumpb(database: "Database") -> bytes:
    """Serializes a Hyperscan database.

    Args:
        database (:class:`Database`): A Hyperscan database.

    Returns:
        bytes: A serialized representation of the database.

    """

def loadb(buf: ByteString, mode: int) -> "Database":
    """Deserializes a Hyperscan database.

    Args:
        buf (bytes): A serialized Hyperscan database.
        mode (int): The expected mode of the database.

    Returns:
        :class:`Database`: The deserialized database instance.

    """

class error(Exception):
    """Base exception class for Hyperscan errors."""

HyperscanError: TypeAlias = error

class ArchitectureError(error):
    """Unsupported CPU architecture."""

class BadAlignError(error):
    """A parameter passed to this function was not correctly aligned."""

class BadAllocationError(error):
    """The memory allocator failed."""

class CompilerError(error):
    """Pattern compilation failed."""

class DatabaseModeError(error):
    """The given database was built for a different mode of operation."""

class DatabasePlatformError(error):
    """The given database was built for a different platform."""

class DatabaseVersionError(error):
    """The given database was built for a different version of Hyperscan."""

class InsufficientSpaceError(error):
    """Provided buffer was too small."""

class InternalPCREError(error):
    """Unexpected internal error."""

class InvalidError(error):
    """Parameter passed to this function was invalid."""

class NoMemoryError(error):
    """Memory allocation failed."""

class ScratchInUseError(error):
    """The scratch region was already in use."""

class UnknownError(error):
    """Unexpected internal error."""

class ScanTerminated(error):
    """The engine was terminated by callback."""

class Scratch:
    """Represents Hyperscan 'scratch space.

    Args:
        database (:class:`Database`, optional): A database instance.

    """

    def __init__(self, database: Optional["Database"] = None) -> None: ...
    def clone(self) -> "Scratch":
        """Clones a scratch space.

        Returns:
            :class:`Scratch`: The cloned scratch space.

        """
    def set_database(self, database: "Database") -> None:
        """Allocates a scratch with the given database.

        Args:
            :obj:`database`: A hyperscan Database.

        """

class Stream:
    """Provides a context manager for scanning streams of text.

    Args:
        database (:class:`Database`): A database initialized with
            :const:`HS_MODE_STREAM`.
        flags (int, optional): Currently unused.
        match_event_handler (callable, optional): The match callback,
            which is invoked for each match result, and passed the
            expression id, start offset, end offset, flags, and a
            context object.

    """

    def close(
        self,
        scratch: Optional[Scratch] = None,
        match_event_handler: Optional[match_event_callback] = None,
        context: Optional[object] = None,
    ) -> None:
        """Closes the stream.

        Args:
            scratch (:class:`Scratch`, optional): Scratch space.
                If a **match_event_handler** is provided and a scratch
                space is not, the scratch space of the associated
                database will be used.
            match_event_handler (callable, optional): The match
                callback, which is invoked for each match result, and
                passed the expression id, start offset, end offset,
                flags, and a context object.
            context (:obj:`object`, optional): A context object passed
                as the last arg to **match_event_handler**.

        """
    def scan(
        self,
        data: AnyStr,
        flags: int = 0,
        scratch=None,
        match_event_handler: Optional[match_event_callback] = None,
        context: Optional[object] = None,
    ) -> None:
        """Scans streaming text.

        Args:
            data (str): The block of text to scan.
            flags (int, optional): Currently unused.
            scratch (:obj:`Scratch`, optional): Scratch space.
            match_event_handler (callable, optional): The match
                callback, which is invoked for each match result, and
                passed the expression id, start offset, end offset,
                flags, and a context object.
            context (object, optional): A context object passed
                as the last arg to **match_event_handler**.

        """
    def size(self) -> int:
        """Return the size of the stream state in bytes"""
    def __enter__(self) -> Self: ...
    def __exit__(self, exc_type, exc_value, exc_traceback) -> None: ...

class Database:
    """Represents a Hyperscan database.

    Args:
        scratch (:class:`Scratch`, optional): Thread-specific
            scratch space.
        mode (int, optional): One of :const:`HS_MODE_BLOCK`,
            :const:`HS_MODE_STREAM`, or :const:`HS_MODE_VECTORED`.
        chimera (bool): Enable Chimera support.

    Attributes:
        mode (int): Scanning mode.
        chimera (bool): Indicates if Chimera support is enabled.
        scratch (:class:`Scratch`): Scratch space.

    """

    mode: int
    chimera: bool
    scratch: Scratch

    def __init__(
        self,
        scratch: Optional[Scratch] = None,
        mode: int = HS_MODE_BLOCK,
        chimera: bool = False,
    ) -> None: ...
    def compile(
        self,
        expressions: Sequence[AnyStr],
        ids: Optional[Sequence[int]] = None,
        elements: Union[Optional[Sequence[int]], int] = None,
        flags: Union[Optional[Sequence[int]], int] = 0,
        literal: bool = False,
        ext: Optional[Sequence[Tuple[int, int, int, int, int, int]]] = None,
    ):
        """Compiles regular expressions
        Args:
            expressions (sequence of str): A sequence of regular
                expressions.
            ids (sequence of int, optional): A sequence of
                expression identifiers.
            elements (int, optional): Length of the expressions
                sequence.
            flags (sequence of int or int, optional):
                Sequence of flags associated with each expression, or a
                single value which is applied to all expressions.
            literal (bool, optional): If True, uses the pure literal
                expression compiler introduced in Hyperscan 5.2.0
            ext (sequence of tuple, optional): A list of tuples used to
                define extended behavior for each pattern. Tuples must
                contain **flags**, **min_offset**, **max_offset**,
                **min_length**, **edit_distance**, and
                **hamming_distance**. See hyperscan documentation for
                more information. **Note:** this parameter if
                **literal** is True

        """
    def info(self) -> str:
        """Returns database information.

        Returns:
            str: Provides version and platform information for the
            database.

        """
    def size(self) -> int:
        """Returns the size of the database in bytes.

        Returns:
            int: The size of the database in bytes.

        """
    def scan(
        self,
        data: AnyStr,
        match_event_handler: match_event_callback,
        flags: int = 0,
        context: object = None,
        scratch: Optional[Scratch] = None,
    ) -> None:
        """Scans a block of text.

        Args:
            data (str): The block of text to scan, if the database
                was opened with streaming or block mode, or a list of
                buffers (i.e. :obj:`bytearray`) if the database was
                opened with vectored mode.
            match_event_handler (callable): The match callback, which is
                invoked for each match result, and passed the expression
                id, start offset, end offset, flags, and a context
                object.
            flags (int): Currently unused.
            context (object, optional): A context object passed as the
                last arg to **match_event_handler**.
            scratch (:class:`Scratch`, optional): A scratch object.

        """
    def stream(
        self,
        match_event_handler: match_event_callback,
        flags: int = 0,
        context: Optional[object] = None,
    ) -> Stream:
        """Returns a new stream context manager.

        Args:
            match_event_handler (callable, optional): The match callback,
                which is invoked for each match result, and passed the
                expression id, start offset, end offset, flags, and a
                context object. Note that this callback will override
                the match event handler defined in the :class:`Database`
                instance.
            flags (int): Currently unused.
            context (object): A context object passed as the last
                arg to **match_event_handler**

        """
