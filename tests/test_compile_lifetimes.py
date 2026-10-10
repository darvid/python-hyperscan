import gc
import sys

import pytest

import hyperscan


@pytest.fixture
def reference_count():
    getrefcount = getattr(sys, "getrefcount", None)
    if getrefcount is None:
        pytest.skip("Reference counts are specific to CPython")
    return getrefcount


@pytest.mark.parametrize(
    "chimera,literal", [(False, False), (False, True), (True, False)]
)
@pytest.mark.parametrize(
    "expressions",
    [
        ["alpha", "omega"],
        [b"alpha", "omega"],
        ["alpha", b"omega"],
        [b"alpha", b"omega"],
        ["caf\u00e9", "th\u00e9"],
    ],
)
def test_compile_keeps_each_encoded_expression(chimera, literal, expressions):
    database = hyperscan.Database(
        chimera=chimera,
        mode=hyperscan.CH_MODE_GROUPS if chimera else hyperscan.HS_MODE_BLOCK,
    )
    database.compile(expressions=expressions, ids=[7, 11], literal=literal)
    matches = []
    data = (
        b"alpha omega"
        if expressions[0] != "caf\u00e9"
        else "caf\u00e9 th\u00e9".encode()
    )
    database.scan(
        data,
        match_event_handler=lambda id, start, end, *args: matches.append((id, end)),
    )
    first_end = 5
    second_end = len(data)
    assert sorted(matches) == [(7, first_end), (11, second_end)]


class GeneratedExpressions:
    def __init__(self, as_bytes):
        self.as_bytes = as_bytes

    def __len__(self):
        return 2

    def __getitem__(self, index):
        expression = ["alpha", "omega"][index]
        return expression.encode() if self.as_bytes else expression


@pytest.mark.parametrize(
    "chimera,literal", [(False, False), (False, True), (True, False)]
)
@pytest.mark.parametrize("as_bytes", [False, True])
def test_compile_keeps_temporary_sequence_items(chimera, literal, as_bytes):
    database = hyperscan.Database(
        chimera=chimera,
        mode=hyperscan.CH_MODE_GROUPS if chimera else hyperscan.HS_MODE_BLOCK,
    )
    database.compile(expressions=GeneratedExpressions(as_bytes), literal=literal)
    matches = []
    database.scan(
        b"alpha omega",
        match_event_handler=lambda id, start, end, *args: matches.append((id, end)),
    )
    assert sorted(matches) == [(0, 5), (1, 11)]


class CompileItemError(Exception):
    pass


@pytest.mark.parametrize("argument", ["expressions", "ids", "flags"])
def test_compile_propagates_sequence_item_errors(argument):
    class BrokenSequence:
        def __len__(self):
            return 2

        def __getitem__(self, index):
            if index == 1:
                raise CompileItemError("second item is unavailable")
            return b"alpha" if argument == "expressions" else 0

    kwargs = {"expressions": [b"alpha", b"omega"], argument: BrokenSequence()}
    database = hyperscan.Database()
    with pytest.raises(CompileItemError, match="second item is unavailable"):
        database.compile(**kwargs)


@pytest.mark.parametrize("invalid_expression", [None, 42, "\ud800"])
def test_compile_releases_expression_references_on_preparation_error(
    invalid_expression,
    reference_count,
):
    expression = bytes(bytearray(b"alpha"))
    before = reference_count(expression)
    database = hyperscan.Database()
    error = UnicodeEncodeError if isinstance(invalid_expression, str) else TypeError
    with pytest.raises(error):
        database.compile(expressions=[expression, invalid_expression])
    gc.collect()
    assert reference_count(expression) == before


@pytest.mark.parametrize(
    "chimera,literal", [(False, False), (False, True), (True, False)]
)
def test_compile_releases_expression_references_after_success(
    chimera, literal, reference_count
):
    expression = bytes(bytearray(b"alpha"))
    before = reference_count(expression)
    database = hyperscan.Database(
        chimera=chimera,
        mode=hyperscan.CH_MODE_GROUPS if chimera else hyperscan.HS_MODE_BLOCK,
    )
    database.compile(expressions=[expression, "omega"], literal=literal)
    gc.collect()
    assert reference_count(expression) == before


@pytest.mark.parametrize("chimera", [False, True])
def test_compile_releases_expression_references_after_native_error(
    chimera, reference_count
):
    expression = bytes(bytearray(b"alpha"))
    before = reference_count(expression)
    database = hyperscan.Database(
        chimera=chimera,
        mode=hyperscan.CH_MODE_GROUPS if chimera else hyperscan.HS_MODE_BLOCK,
    )
    with pytest.raises(hyperscan.error):
        database.compile(expressions=[expression, "("])
    gc.collect()
    assert reference_count(expression) == before


@pytest.mark.parametrize("argument", ["ids", "flags"])
def test_compile_releases_each_metadata_item(argument, reference_count):
    first = int("1048576") if argument == "ids" else int("1024")
    second = int("1048577") if argument == "ids" else int("1025")
    before = (reference_count(first), reference_count(second))
    database = hyperscan.Database()
    database.compile(expressions=[b"alpha", b"omega"], **{argument: [first, second]})
    assert (reference_count(first), reference_count(second)) == before
