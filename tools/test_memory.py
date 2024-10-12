#!/usr/bin/env python
import os
import tracemalloc
from typing import Any, Callable, Generator

import psutil
from rich.progress import track

import hyperscan

db = hyperscan.Database(mode=hyperscan.HS_MODE_BLOCK)


def profile_mem(
    target: Callable[..., Any],
    iterations=100_000,
    sample_interval=10_000,
) -> Generator[int, None, None]:
    for i in range(iterations):
        target()
        if i % sample_interval == 0:
            process = psutil.Process()
            yield process.memory_info().rss


def main() -> None:
    tracemalloc.start()
    os.getpid()

    db = hyperscan.Database(mode=hyperscan.HS_MODE_BLOCK)

    def compile():
        db.compile(
            expressions=[b'test'],
            ids=[1],
            flags=[hyperscan.HS_FLAG_ALLOWEMPTY],
        )

    def dump():
        compile()
        hyperscan.dumpb(db)

    for callback in (dump,):
        for i, step in enumerate(
            track(
                profile_mem(callback),
                description=f"📊 Profiling {callback.__name__}...",
                total=10,
            )
        ):
            ...


if __name__ == "__main__":
    main()
