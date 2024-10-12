#!/bin/bash
mprof rm 0 2>/dev/null
mprof run python test_memory.py
mprof plot >/dev/null 2>&1
