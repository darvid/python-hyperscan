"""Run native lifetime regressions in children so a crash does not kill pytest."""

import subprocess
import sys
import textwrap

import pytest


def run_stream_case(code):
    setup = """
        import gc
        import sys
        import weakref
        import hyperscan

        db = hyperscan.Database(mode=hyperscan.HS_MODE_STREAM)
        db.compile(expressions=[b"a$"])
    """
    result = subprocess.run(
        [sys.executable, "-c", textwrap.dedent(setup) + textwrap.dedent(code)],
        capture_output=True,
        text=True,
        timeout=10,
    )
    assert result.returncode == 0, result.stdout + result.stderr


@pytest.mark.parametrize("argument", ["database", "callback", "context", "scratch"])
def test_stream_retains_constructor_objects(argument):
    run_stream_case(
        f"""
        class Context:
            pass

        callback = lambda *args: None
        context = Context()
        scratch = hyperscan.Scratch(db)
        objects = dict(database=db, callback=callback, context=context, scratch=scratch)
        value = objects[{argument!r}]
        kwargs = dict(match_event_handler=callback, context=context)
        if {argument!r} == "scratch":
            kwargs["scratch"] = scratch
        before = sys.getrefcount(value)
        stream = hyperscan.Stream(db, **kwargs)
        assert sys.getrefcount(value) == before + 1
        del stream
        assert sys.getrefcount(value) == before
        """
    )


def test_enter_returns_owned_reference():
    run_stream_case(
        """
        stream = db.stream(match_event_handler=None)
        before = sys.getrefcount(stream)
        entered = stream.__enter__()
        assert entered is stream
        assert sys.getrefcount(stream) == before + 1
        stream.close()
        del entered
        assert sys.getrefcount(stream) == before
        """
    )


def test_factory_returns_one_owned_reference():
    run_stream_case(
        """
        manager = db.stream(match_event_handler=None)
        assert sys.getrefcount(manager) == 2
        """
    )


def test_exit_releases_close_result():
    run_stream_case(
        """
        marker = object()
        class CustomStream(hyperscan.Stream):
            def close(self):
                return marker
        manager = CustomStream(db)
        before = sys.getrefcount(marker)
        assert manager.__exit__(None, None, None) is None
        assert sys.getrefcount(marker) == before
        """
    )


@pytest.mark.parametrize("invalid", ["database", "scratch"])
def test_invalid_constructor_argument_is_rejected(invalid):
    run_stream_case(
        f"""
        try:
            if {invalid!r} == "database":
                hyperscan.Stream(object())
            else:
                hyperscan.Stream(db, scratch=object())
        except TypeError:
            pass
        else:
            raise AssertionError("invalid constructor argument was accepted")
        """
    )


def test_stream_survives_context_exit_and_can_reopen():
    run_stream_case(
        """
        matches = []
        manager = db.stream(match_event_handler=lambda *args: matches.append(args))
        for _ in range(2):
            with manager as stream:
                stream.scan(b"a")
            assert stream is manager
            assert stream.database is db
        assert len(matches) == 2
        """
    )


def test_stream_retains_temporary_database_and_callback():
    run_stream_case(
        """
        matches = []
        def make_stream():
            database = hyperscan.Database(mode=hyperscan.HS_MODE_STREAM)
            database.compile(expressions=[b"a$"])
            return database.stream(
                match_event_handler=lambda *args: matches.append(args)
            )
        manager = make_stream()
        gc.collect()
        with manager as stream:
            stream.scan(b"a")
        assert len(matches) == 1
        """
    )


@pytest.mark.parametrize("opened", [False, True])
def test_stream_context_cycle_is_collectible(opened):
    run_stream_case(
        f"""
        class Context:
            pass
        context = Context()
        context_ref = weakref.ref(context)
        manager = db.stream(match_event_handler=None, context=context)
        if {opened!r}:
            entered = manager.__enter__()
            del entered
        context.manager = manager
        del context, manager
        gc.collect()
        assert context_ref() is None
        """
    )


@pytest.mark.parametrize("phase", ["before_enter", "after_close"])
def test_scan_requires_open_stream(phase):
    run_stream_case(
        f"""
        manager = db.stream(match_event_handler=None)
        if {phase!r} == "after_close":
            entered = manager.__enter__()
            del entered
            manager.close()
        try:
            manager.scan(b"a")
        except RuntimeError as exc:
            assert "not open" in str(exc)
        else:
            raise AssertionError("scan accepted a closed stream")
        """
    )


def test_close_is_idempotent_and_flushes_once():
    run_stream_case(
        """
        matches = []
        manager = db.stream(match_event_handler=lambda *args: matches.append(args))
        manager.close()
        with manager as stream:
            stream.scan(b"a")
            stream.close()
            stream.close()
        assert len(matches) == 1
        """
    )


@pytest.mark.parametrize("operation", ["enter", "init"])
def test_open_stream_rejects_reinitialization(operation):
    run_stream_case(
        f"""
        manager = db.stream(match_event_handler=None)
        with manager:
            try:
                if {operation!r} == "enter":
                    manager.__enter__()
                else:
                    manager.__init__(db)
            except RuntimeError:
                pass
            else:
                raise AssertionError("open stream was replaced")
            manager.scan(b"a")
        """
    )


def test_close_preserves_callback_exception_and_consumes_stream():
    run_stream_case(
        """
        def callback(*args):
            raise ValueError("end-of-data callback")
        manager = db.stream(match_event_handler=callback)
        entered = manager.__enter__()
        del entered
        manager.scan(b"a")
        try:
            manager.close()
        except ValueError as exc:
            assert str(exc) == "end-of-data callback"
        else:
            raise AssertionError("callback error was lost")
        manager.close()
        """
    )


def test_stored_and_explicit_scratch():
    run_stream_case(
        """
        scratch = hyperscan.Scratch(db)
        matches = []
        manager = hyperscan.Stream(
            db, scratch=scratch, match_event_handler=lambda *args: matches.append(args)
        )
        with manager as stream:
            stream.scan(b"a", scratch=scratch)
            stream.close(scratch=scratch)
        assert len(matches) == 1
        """
    )


def test_invalid_close_scratch_keeps_stream_open():
    run_stream_case(
        """
        manager = db.stream(match_event_handler=None)
        with manager as stream:
            try:
                stream.close(scratch=object())
            except TypeError:
                pass
            else:
                raise AssertionError("invalid scratch was accepted")
            stream.scan(b"a")
        """
    )
