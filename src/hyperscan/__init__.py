import typing

from hyperscan._ext import *  # noqa: F401, F403

try:
    from hyperscan._version import __version__  # pyright: ignore
except ImportError:
    __version__ = "unknown"


class ExpressionExt(typing.NamedTuple):
    flags: int
    min_offset: int = 0
    max_offset: int = 0
    min_length: int = 0
    edit_distance: int = 0
    hamming_distance: int = 0
