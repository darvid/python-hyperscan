import concurrent.futures
import sys
import threading
from typing import List, Tuple

import pytest

import hyperscan


@pytest.fixture
def threaded_database() -> hyperscan.Database:
    """Compile a simple database that matches the literal 'foobar'."""
    db = hyperscan.Database()
    db.compile(
        expressions=[b"foobar"],
        ids=[0],
        elements=1,
        flags=0,
    )
    return db


def test_shared_scratch_rejected_concurrent_scan(threaded_database):
    """Using the same scratch from multiple threads must raise HS_SCRATCH_IN_USE.

    Hyperscan and Vectorscan require each concurrent scan to run against its own
    scratch space; sharing a scratch between threads is explicitly unsupported.
    """
    if not getattr(sys, "_is_gil_enabled", lambda: True)():
        pytest.skip("Scratch contention not observable under free-threaded CPython yet")

    db = threaded_database
    shared_scratch = hyperscan.Scratch(db)
    # Assign the same scratch so implicit usage also contends.
    db.scratch = shared_scratch

    start_barrier = threading.Barrier(2)
    release_event = threading.Event()
    callback_ready = threading.Event()
    results: List[str] = [None, None]

    def blocking_callback(*args):
        callback_ready.set()
        # The first thread to enter the scan should block here until contention
        # is detected by the other thread.
        assert release_event.wait(
            timeout=5
        ), "expected scratch contention not observed"
        return 0

    def worker(slot: int):
        try:
            start_barrier.wait()
            db.scan(
                b"foobar" * 1024,
                match_event_handler=blocking_callback,
                scratch=shared_scratch,
            )
            results[slot] = "ok"
        except hyperscan.ScratchInUseError:
            results[slot] = "scratch_in_use"
            release_event.set()

    threads = [threading.Thread(target=worker, args=(i,)) for i in range(2)]
    for thread in threads:
        thread.start()

    assert callback_ready.wait(
        timeout=5
    ), "scan callback did not run; scratch contention test invalid"

    for thread in threads:
        thread.join()

    assert sorted(results) == ["ok", "scratch_in_use"]


def test_per_thread_scratch_allows_parallel_scans(threaded_database):
    """Independent scratch instances must permit safe parallel scans.

    The extension wraps the free-threaded CPython support in a global lock, but
    separate scratch regions still allow true parallelism when the GIL is off.
    """

    db = threaded_database
    workers = 4
    start_barrier = threading.Barrier(workers)

    def run_scan(slot: int) -> Tuple[int, int, int]:
        scratch = hyperscan.Scratch(db)
        matches: List[Tuple[int, int, int]] = []

        def on_match(eid, start, end, flags, context):
            matches.append((eid, start, end))
            return 0

        start_barrier.wait()
        db.scan(
            b"foobar" * 256,
            match_event_handler=on_match,
            scratch=scratch,
        )
        assert matches, f"thread {slot} observed no matches"
        return matches[0]

    with concurrent.futures.ThreadPoolExecutor(max_workers=workers) as executor:
        observed = list(executor.map(run_scan, range(workers)))

    # Every worker should see the same literal match.
    assert all(match == (0, 0, 6) for match in observed)
