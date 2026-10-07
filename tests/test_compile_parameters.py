import pytest

import hyperscan


@pytest.fixture(params=["regex", "literal", "chimera"])
def compiler(request):
    chimera = request.param == "chimera"
    db = hyperscan.Database(
        chimera=chimera,
        mode=hyperscan.CH_MODE_GROUPS if chimera else hyperscan.HS_MODE_BLOCK,
    )

    def compile(**kwargs):
        db.compile(expressions=[b"foo"], literal=request.param == "literal", **kwargs)

    return (
        db,
        compile,
        hyperscan.CH_FLAG_CASELESS if chimera else hyperscan.HS_FLAG_CASELESS,
    )


@pytest.mark.parametrize(
    "flags, error",
    [
        (0.0, TypeError),
        (-1, OverflowError),
        (1 << 32, OverflowError),
        ([1 << 32], OverflowError),
        ([0.0], TypeError),
        (object(), TypeError),
    ],
)
def test_invalid_compile_flags(compiler, flags, error):
    _, compile, _ = compiler
    with pytest.raises(error):
        compile(flags=flags)


@pytest.mark.parametrize("identifier", [-1, 1 << 32, (1 << 32) + 7])
def test_invalid_compile_ids(compiler, identifier):
    _, compile, _ = compiler
    with pytest.raises(OverflowError):
        compile(ids=[identifier])


@pytest.mark.parametrize("identifier", [0, (1 << 32) - 1])
@pytest.mark.parametrize("sequence_flags", [False, True])
def test_valid_compile_parameter_boundaries(compiler, identifier, sequence_flags):
    db, compile, flag = compiler
    compile(ids=[identifier], flags=[flag] if sequence_flags else flag)
    matches = []
    db.scan(b"FOO", match_event_handler=lambda *args: matches.append(args[0]))
    assert matches == [identifier]
