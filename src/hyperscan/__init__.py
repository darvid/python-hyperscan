import typing

from hyperscan._hyperscan import *

__version__ = "0.2.0"


class ExpressionExt(typing.NamedTuple):
    flags: int
    min_offset: int = 0
    max_offset: int = 0
    min_length: int = 0
    edit_distance: int = 0
    hamming_distance: int = 0
