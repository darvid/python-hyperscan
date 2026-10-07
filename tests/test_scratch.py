import pytest

import hyperscan


@pytest.mark.parametrize(
    "chimera, mode",
    [
        (False, hyperscan.HS_MODE_BLOCK),
        (True, hyperscan.CH_MODE_NOGROUPS),
        (True, hyperscan.CH_MODE_GROUPS),
    ],
)
@pytest.mark.parametrize("depth", [1, 2, 3])
def test_scratch_clone_chain(chimera, mode, depth):
    db = hyperscan.Database(chimera=chimera, mode=mode)
    db.compile(expressions=[b"foo"], ids=[7])
    scratch = db.scratch
    for _ in range(depth):
        scratch = scratch.clone()

    matches = []
    db.scan(
        b"foo",
        scratch=scratch,
        match_event_handler=lambda *args: matches.append(args[0]),
    )
    assert matches == [7]
    assert scratch.database is None


def test_unallocated_scratch_clone():
    scratch = hyperscan.Scratch()
    with pytest.raises(hyperscan.InvalidError):
        scratch.clone()
    assert scratch.database is None
