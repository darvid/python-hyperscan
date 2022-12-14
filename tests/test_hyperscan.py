import memunit
import pytest

import hyperscan

patterns = (
    (rb"fo+", 0, 0, 0),
    (
        rb"^foobar",
        1,
        hyperscan.HS_FLAG_CASELESS,
        hyperscan.CH_FLAG_CASELESS,
    ),
    (
        rb"BAR",
        2,
        hyperscan.HS_FLAG_CASELESS | hyperscan.HS_FLAG_SOM_LEFTMOST,
        hyperscan.CH_FLAG_CASELESS,
    ),
)


@pytest.fixture(scope="module")
def database_chimera():
    db = hyperscan.Database(chimera=True, mode=hyperscan.CH_MODE_GROUPS)
    expressions, ids, _, flags = zip(*patterns)
    db.compile(
        expressions=expressions,
        ids=ids,
        elements=len(patterns),
        flags=flags,
    )
    return db


@pytest.fixture(scope="module")
def database_block():
    db = hyperscan.Database()
    expressions, ids, flags, _ = zip(*patterns)
    db.compile(
        expressions=expressions,
        ids=ids,
        elements=len(patterns),
        flags=flags,
    )
    return db


@pytest.fixture(scope="module")
def database_stream():
    db = hyperscan.Database(
        mode=(hyperscan.HS_MODE_STREAM | hyperscan.HS_MODE_SOM_HORIZON_LARGE)
    )
    expressions, ids, flags, _ = zip(*patterns)
    db.compile(
        expressions=expressions,
        ids=ids,
        elements=len(patterns),
        flags=flags,
    )
    return db


@memunit.assert_lt_mb(50)
def test_compilation_memory(database_block):
    expressions, ids, flags, _ = zip(*patterns)
    for i in range(100_000):
        database_block.compile(
            expressions=expressions,
            ids=ids,
            elements=len(patterns),
            flags=flags,
        )


@pytest.fixture(scope="module")
def database_vector():
    db = hyperscan.Database(mode=hyperscan.HS_MODE_VECTORED)
    expressions, ids, flags, _ = zip(*patterns)
    db.compile(
        expressions=expressions,
        ids=ids,
        elements=len(patterns),
        flags=flags,
    )
    return db


def test_chimera_scan(database_chimera, mocker):
    callback = mocker.Mock(return_value=None)

    database_chimera.scan(b"foobar", match_event_handler=callback)
    callback.assert_has_calls(
        [
            mocker.call(0, 0, 3, 0, [(1, 0, 3)], None),
            mocker.call(1, 0, 6, 0, [(1, 0, 6)], None),
            mocker.call(2, 3, 6, 0, [(1, 3, 6)], None),
        ],
        any_order=True,
    )


def test_block_scan(database_block, mocker):
    callback = mocker.Mock(return_value=None)

    database_block.scan(b"foobar", match_event_handler=callback)
    callback.assert_has_calls(
        [
            mocker.call(0, 0, 2, 0, None),
            mocker.call(0, 0, 3, 0, None),
            mocker.call(1, 0, 6, 0, None),
            mocker.call(2, 3, 6, 0, None),
        ],
        any_order=True,
    )


def test_stream_scan(database_stream, mocker):
    callback = mocker.Mock(return_value=None)

    with database_stream.stream(match_event_handler=callback) as stream:
        stream.scan(b"foo")
        stream.scan(b"bar")
        stream.scan(b"foo", context=1234)
    callback.assert_has_calls(
        [
            mocker.call(0, 0, 2, 0, None),
            mocker.call(0, 0, 3, 0, None),
            mocker.call(1, 0, 6, 0, None),
            mocker.call(2, 3, 6, 0, None),
            mocker.call(0, 0, 8, 0, 1234),
            mocker.call(0, 0, 9, 0, 1234),
        ],
        any_order=True,
    )


def test_vectored_scan(database_vector, mocker):
    callback = mocker.Mock(return_value=None)

    buffers = [
        bytearray(b"xxxfooxxx"),
        bytearray(b"xxfoxbarx"),
        bytearray(b"barxxxxxx"),
    ]
    database_vector.scan(buffers, match_event_handler=callback)
    callback.assert_has_calls(
        [
            mocker.call(0, 0, 5, 0, None),
            mocker.call(0, 0, 6, 0, None),
            mocker.call(2, 9, 12, 0, None),
        ],
        any_order=True,
    )


def test_ext_multi_min_offset(mocker):
    callback = mocker.Mock(return_value=None)
    db = hyperscan.Database()
    db.compile(
        expressions=[b"foobar"],
        flags=hyperscan.HS_FLAG_SOM_LEFTMOST,
        ext=[
            hyperscan.ExpressionExt(
                flags=hyperscan.HS_EXT_FLAG_MIN_OFFSET, min_offset=12
            )
        ],
    )
    db.scan(b"foobarfoobar", match_event_handler=callback)
    callback.assert_has_calls([mocker.call(0, 6, 12, 0, None)])


def test_ext_multi_max_offset(mocker):
    callback = mocker.Mock(return_value=None)
    db = hyperscan.Database()
    db.compile(
        expressions=[b"foobar"],
        flags=hyperscan.HS_FLAG_SOM_LEFTMOST,
        ext=[
            hyperscan.ExpressionExt(
                flags=hyperscan.HS_EXT_FLAG_MAX_OFFSET, max_offset=6
            )
        ],
    )
    db.scan(b"foobarfoobar", match_event_handler=callback)
    callback.assert_has_calls([mocker.call(0, 0, 6, 0, None)])


def test_ext_multi_min_length(mocker):
    callback = mocker.Mock(return_value=None)
    db = hyperscan.Database()
    db.compile(
        expressions=[b"fo+"],
        flags=hyperscan.HS_FLAG_SOM_LEFTMOST,
        ext=[
            hyperscan.ExpressionExt(
                flags=hyperscan.HS_EXT_FLAG_MIN_LENGTH, min_length=3
            )
        ],
    )
    db.scan(b"fo", match_event_handler=callback)
    callback.assert_has_calls([])
    db.scan(b"foo", match_event_handler=callback)
    callback.assert_has_calls([mocker.call(0, 0, 3, 0, None)])


def test_ext_multi_edit_distance(mocker):
    callback = mocker.Mock(return_value=None)
    db = hyperscan.Database()
    db.compile(
        expressions=[b"foobar"],
        flags=hyperscan.HS_FLAG_SOM_LEFTMOST,
        ext=[
            hyperscan.ExpressionExt(
                flags=hyperscan.HS_EXT_FLAG_EDIT_DISTANCE, edit_distance=1
            )
        ],
    )
    db.scan(b"fxxxar", match_event_handler=callback)
    callback.assert_has_calls([])

    db.compile(
        expressions=[b"foobar"],
        flags=hyperscan.HS_FLAG_SOM_LEFTMOST,
        ext=[
            hyperscan.ExpressionExt(
                flags=hyperscan.HS_EXT_FLAG_EDIT_DISTANCE, edit_distance=3
            )
        ],
    )
    db.scan(b"fxxxar", match_event_handler=callback)
    callback.assert_has_calls([mocker.call(0, 0, 6, 0, None)])


def test_ext_multi_hamming_distance(mocker):
    callback = mocker.Mock(return_value=None)
    db = hyperscan.Database()
    db.compile(
        expressions=[b"foobar"],
        flags=hyperscan.HS_FLAG_SOM_LEFTMOST,
        ext=[
            hyperscan.ExpressionExt(
                flags=hyperscan.HS_EXT_FLAG_HAMMING_DISTANCE,
                hamming_distance=1,
            )
        ],
    )
    db.scan(b"fxxxar", match_event_handler=callback)
    callback.assert_has_calls([])

    db.compile(
        expressions=[b"foobar"],
        flags=hyperscan.HS_FLAG_SOM_LEFTMOST,
        ext=[
            hyperscan.ExpressionExt(
                flags=hyperscan.HS_EXT_FLAG_HAMMING_DISTANCE,
                hamming_distance=3,
            )
        ],
    )
    db.scan(b"fxxxar", match_event_handler=callback)
    callback.assert_has_calls([mocker.call(0, 0, 6, 0, None)])


@pytest.mark.parametrize("return_value", [1, True, 42])
def test_stream_scan_halt(database_stream, mocker, return_value):
    callback = mocker.Mock(return_value=return_value)

    with pytest.raises(hyperscan._hyperscan.ScanTerminated):
        with database_stream.stream(match_event_handler=callback) as stream:
            stream.scan(b"foo")

    assert callback.call_count == 1


def test_database_info(database_block):
    info_string = database_block.info()
    for field in (b"Version", b"Features", b"Mode"):
        assert field + b": " in info_string


def test_database_serialize(database_stream):
    serialized = hyperscan.dumpb(database_stream)
    assert len(serialized) >= 6000


def test_database_deserialize(database_stream):
    serialized = hyperscan.dumpb(database_stream)
    db = hyperscan.loadb(serialized)
    assert id(db) != id(database_stream)


def test_database_exception_in_callback(database_block, mocker):
    callback = mocker.Mock(side_effect=RuntimeError("oops"))

    with pytest.raises(RuntimeError, match=r"^oops$"):
        database_block.scan(b"foobar", match_event_handler=callback)


def test_literal_expressions(mocker):
    db = hyperscan.Database()
    expressions, ids, _, __ = zip(*patterns)
    db.compile(expressions=list(expressions), ids=ids, literal=True)
    callback = mocker.Mock(return_value=None)
    expected = []
    for i, expression in enumerate(expressions):
        db.scan(expression, match_event_handler=callback, context=expression)
        expected.append(mocker.call(ids[i], 0, len(expression), 0, expression))
    assert callback.mock_calls == expected
