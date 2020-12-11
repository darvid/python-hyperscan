import typing

import pkg_resources

from hyperscan._hyperscan import *

__version__ = pkg_resources.get_distribution('hyperscan').version


class ExpressionExt(typing.NamedTuple):
    flags: int
    min_offset: int = 0
    max_offset: int = 0
    min_length: int = 0
    edit_distance: int = 0
    hamming_distance: int = 0
