#!/usr/bin/env python
"""Benchmark to reproduce performance regression reported in #253.

Simulates the reporter's workload: 50 patterns scanning 500KB documents
in block mode. Reports throughput (MB/s) and average time per scan.

Usage:
    python tools/bench_regression.py
    python tools/bench_regression.py --patterns 100 --doc-size 1048576
"""

import argparse
import os
import random
import statistics
import string
import time

import hyperscan


def generate_patterns(count):
    """Generate realistic regex patterns for benchmarking."""
    templates = [
        rb"\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}\b",
        rb"\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\b",
        rb"\b(https?|ftp)://[^\s/$.?#].[^\s]*\b",
        rb"\b\d{3}[-.]?\d{3}[-.]?\d{4}\b",
        rb"\b[A-Z][a-z]+\s[A-Z][a-z]+\b",
        rb"[0-9a-fA-F]{32}",
        rb"\b(error|warning|critical|fatal)\b",
        rb"\b\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\b",
        rb"<[^>]+>",
        rb"\$\d+[\.,]?\d*",
    ]
    keyword_bases = [
        b"password", b"secret", b"token", b"api.key",
        b"authorization", b"credential", b"private",
        b"admin", b"root", b"config", b"database",
        b"server", b"client", b"session", b"cookie",
        b"header", b"payload", b"request", b"response",
        b"encrypt", b"decrypt", b"hash", b"salt",
        b"certificate", b"key.file", b"login", b"logout",
        b"access", b"permission", b"role", b"user",
        b"account", b"profile", b"setting", b"option",
        b"enable", b"disable", b"start", b"stop",
        b"create", b"delete", b"update", b"select",
    ]

    patterns = []
    for i in range(count):
        if i < len(templates):
            patterns.append(templates[i])
        else:
            base = keyword_bases[i % len(keyword_bases)]
            suffix = str(i).encode()
            patterns.append(rb"\b" + base + suffix + rb"\b")
    return patterns


def generate_document(size):
    """Generate a synthetic document of approximately the given size."""
    words = [
        "the", "quick", "brown", "fox", "jumps", "over", "lazy", "dog",
        "server", "error", "warning", "request", "response", "data",
        "user", "admin", "config", "session", "token", "password",
        "https://example.com/path", "192.168.1.100", "test@email.com",
        "2025-01-15T10:30:00", "$1,234.56", "abcdef1234567890abcdef",
        "authorization", "credential", "certificate", "encrypted",
    ]
    rng = random.Random(42)
    chunks = []
    total = 0
    while total < size:
        line_words = rng.choices(words, k=rng.randint(5, 20))
        line = " ".join(line_words) + "\n"
        chunks.append(line)
        total += len(line)
    return "".join(chunks)[:size].encode("utf-8")


def run_benchmark(db, document, num_scans, warmup=3):
    """Run the benchmark and return per-scan times."""
    match_count = 0

    def on_match(id, start, end, flags, ctx):
        nonlocal match_count
        match_count += 1

    # warmup
    for _ in range(warmup):
        db.scan(document, match_event_handler=on_match)

    match_count = 0
    times = []
    for _ in range(num_scans):
        t0 = time.perf_counter()
        db.scan(document, match_event_handler=on_match)
        t1 = time.perf_counter()
        times.append(t1 - t0)

    return times, match_count


def main():
    parser = argparse.ArgumentParser(
        description="Benchmark for hyperscan regression #253"
    )
    parser.add_argument(
        "--patterns", type=int, default=50,
        help="Number of regex patterns (default: 50)",
    )
    parser.add_argument(
        "--doc-size", type=int, default=500_000,
        help="Document size in bytes (default: 500000)",
    )
    parser.add_argument(
        "--scans", type=int, default=100,
        help="Number of scans to perform (default: 100)",
    )
    parser.add_argument(
        "--warmup", type=int, default=5,
        help="Number of warmup scans (default: 5)",
    )
    args = parser.parse_args()

    print("=" * 60)
    print("hyperscan regression benchmark (#253)")
    print("=" * 60)

    db_info = hyperscan.Database(mode=hyperscan.HS_MODE_BLOCK)
    patterns = generate_patterns(args.patterns)
    db_info.compile(
        expressions=patterns,
        ids=list(range(len(patterns))),
        flags=[hyperscan.HS_FLAG_CASELESS | hyperscan.HS_FLAG_SINGLEMATCH]
        * len(patterns),
    )

    print(f"engine info:     {db_info.info().decode()}")
    print(f"database size:   {db_info.size():,} bytes")
    print(f"pattern count:   {args.patterns}")
    print(f"document size:   {args.doc_size:,} bytes")
    print(f"scan iterations: {args.scans}")
    print(f"warmup scans:    {args.warmup}")
    print()

    document = generate_document(args.doc_size)

    print("running benchmark...")
    times, match_count = run_benchmark(
        db_info, document, args.scans, args.warmup
    )

    avg_time = statistics.mean(times)
    median_time = statistics.median(times)
    stdev_time = statistics.stdev(times) if len(times) > 1 else 0
    min_time = min(times)
    max_time = max(times)
    doc_mb = args.doc_size / (1024 * 1024)
    throughput_avg = doc_mb / avg_time if avg_time > 0 else float("inf")
    throughput_median = (
        doc_mb / median_time if median_time > 0 else float("inf")
    )

    print()
    print("-" * 60)
    print("results")
    print("-" * 60)
    print(f"total matches:      {match_count:,}")
    print(f"avg time/scan:      {avg_time * 1000:.3f} ms")
    print(f"median time/scan:   {median_time * 1000:.3f} ms")
    print(f"min time/scan:      {min_time * 1000:.3f} ms")
    print(f"max time/scan:      {max_time * 1000:.3f} ms")
    print(f"stdev:              {stdev_time * 1000:.3f} ms")
    print(f"throughput (avg):   {throughput_avg:.1f} MB/s")
    print(f"throughput (median):{throughput_median:.1f} MB/s")
    print()

    if avg_time * 1000 > 10:
        print("!! REGRESSION DETECTED !!")
        print(
            f"avg scan time {avg_time*1000:.1f}ms is way above the "
            f"expected ~3ms baseline from v0.7.21"
        )
        print(
            "likely cause: SIMDE_BACKEND=ON forcing SSE2-only code "
            "paths on x86-64"
        )
    else:
        print("performance looks healthy")


if __name__ == "__main__":
    main()
