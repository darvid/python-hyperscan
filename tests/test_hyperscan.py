import pytest

import hyperscan


patterns = (
    (br'fo+', 0, 0),
    (br'^foobar', 1, hyperscan.HS_FLAG_CASELESS),
    (br'BAR', 2, hyperscan.HS_FLAG_CASELESS | hyperscan.HS_FLAG_SOM_LEFTMOST),
)


@pytest.fixture(scope='module')
def database_block():
    db = hyperscan.Database()
    expressions, ids, flags = zip(*patterns)
    db.compile(
        expressions=expressions, ids=ids, elements=len(patterns), flags=flags
    )
    return db


@pytest.fixture(scope='module')
def database_stream():
    db = hyperscan.Database(
        mode=(hyperscan.HS_MODE_STREAM | hyperscan.HS_MODE_SOM_HORIZON_LARGE)
    )
    expressions, ids, flags = zip(*patterns)
    db.compile(
        expressions=expressions, ids=ids, elements=len(patterns), flags=flags
    )
    return db


def test_block_scan(database_block, mocker):
    callback = mocker.Mock(return_value=None)

    database_block.scan(b'foobar', match_event_handler=callback)
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
        stream.scan(b'foo')
        stream.scan(b'bar')
        stream.scan(b'foo', context=1234)
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


@pytest.mark.parametrize('return_value', [1, True, 42])
def test_stream_scan_halt(database_stream, mocker, return_value):
    callback = mocker.Mock(return_value=return_value)

    with pytest.raises(hyperscan.error):
        with database_stream.stream(match_event_handler=callback) as stream:
            stream.scan(b'foo')

    assert callback.call_count == 1


def test_database_info(database_block):
    info_string = database_block.info()
    for field in (b'Version', b'Features', b'Mode'):
        assert field + b': ' in info_string


def test_database_serialize(database_stream):
    serialized = hyperscan.dumpb(database_stream)
    assert len(serialized) >= 6000


def test_database_deserialize(database_stream):
    serialized = hyperscan.dumpb(database_stream)
    db = hyperscan.loadb(serialized)
    assert id(db) != id(database_stream)


def test_database_exception_in_callback(database_block, mocker):
    callback = mocker.Mock(side_effect=RuntimeError('oops'))

    with pytest.raises(RuntimeError, match=r'^oops$'):
        database_block.scan(b'foobar', match_event_handler=callback)


def test_literal_expressions(mocker):
    db = hyperscan.Database()
    expressions, ids, _ = zip(*patterns)
    expressions = [e + b'\0' for e in expressions]
    db.compile(expressions=expressions, ids=ids, literal=True)
    callback = mocker.Mock(return_value=None)
    expected = []
    for i, expression in enumerate(expressions):
        expression = expression[:-1]
        db.scan(expression, match_event_handler=callback, context=expression)
        expected.append(mocker.call(ids[i], 0, len(expression), 0, expression))
    assert callback.mock_calls == expected
