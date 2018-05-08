import sys

import pytest

import hyperscan


patterns = (
    (br'fo+',      0, 0),
    (br'^foobar$', 1, hyperscan.HS_FLAG_CASELESS),
    (br'BAR',      2, hyperscan.HS_FLAG_CASELESS |
                      hyperscan.HS_FLAG_SOM_LEFTMOST),
)


@pytest.fixture(scope='module')
def database_block():
    db = hyperscan.Database()
    expressions, ids, flags = zip(*patterns)
    db.compile(expressions=expressions, ids=ids,
               elements=len(patterns), flags=flags)
    return db


@pytest.fixture(scope='module')
def database_stream():
    db = hyperscan.Database(mode=(hyperscan.HS_MODE_STREAM |
                                  hyperscan.HS_MODE_SOM_HORIZON_LARGE))
    expressions, ids, flags = zip(*patterns)
    db.compile(expressions=expressions, ids=ids,
               elements=len(patterns), flags=flags)
    return db


def test_block_scan(database_block, mocker):
    callback = mocker.Mock()

    database_block.scan(b'foobar', match_event_handler=callback)
    callback.assert_has_calls([
        mocker.call(0, 0, 2, 0, None),
        mocker.call(0, 0, 3, 0, None),
        mocker.call(1, 0, 6, 0, None),
        mocker.call(2, 3, 6, 0, None),
    ], any_order=True)


def test_stream_scan(database_stream, mocker):
    callback = mocker.Mock()

    with database_stream.stream(match_event_handler=callback) as stream:
        stream.scan(b'foo')
        stream.scan(b'bar')
    callback.assert_has_calls([
        mocker.call(0, 0, 2, 0, None),
        mocker.call(0, 0, 3, 0, None),
        mocker.call(1, 0, 6, 0, None),
        mocker.call(2, 3, 6, 0, None),
    ], any_order=True)


def test_database_serialize(database_stream):
    serialized = hyperscan.dumps(database_stream)
    assert len(serialized) == 6200


def test_database_deserialize(database_stream):
    serialized = hyperscan.dumps(database_stream)
    db = hyperscan.loads(bytearray(serialized))
    assert id(db) != id(database_stream)
