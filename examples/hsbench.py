#!/usr/bin/env python
from __future__ import print_function

import argparse
import io
import sqlite3
import sys
import timeit

import hyperscan


flagchars = {
    'i': hyperscan.HS_FLAG_CASELESS,
    's': hyperscan.HS_FLAG_DOTALL,
    'm': hyperscan.HS_FLAG_MULTILINE,
    'H': hyperscan.HS_FLAG_SINGLEMATCH,
    'V': hyperscan.HS_FLAG_ALLOWEMPTY,
    'W': hyperscan.HS_FLAG_UCP,
    '8': hyperscan.HS_FLAG_UTF8,
    'P': hyperscan.HS_FLAG_PREFILTER,
    'L': hyperscan.HS_FLAG_SOM_LEFTMOST,
}


def process_expression(expr):
    expr = expr.strip()
    cpos = expr.find(':')
    lppos = expr.find('/')
    if cpos > lppos:
        id_ = None
    else:
        id_ = int(expr[:cpos])
        if id_ < 0:
            raise TypeError('expression ids must be unsigned')
    rppos = expr.rfind('/')
    expression = expr[lppos + 1 : rppos].encode('utf8')
    flags = 0
    for fc in expr[rppos + 1 :]:
        flags |= flagchars[fc]
    return id_, expression, flags


def build_database(expr_path, mode=hyperscan.HS_MODE_STREAM):
    ids = []
    expressions = []
    flags = []
    with io.open(expr_path, 'r') as f:
        for line in f:
            id_, expression, flags_ = process_expression(line)
            ids.append(id_)
            expressions.append(expression)
            flags.append(flags_)
    database = hyperscan.Database(mode=mode)
    database.compile(expressions=expressions, ids=ids, flags=flags)
    return len(expressions), database


def iter_corpus(path):
    conn = sqlite3.connect(path)
    cur = conn.cursor()
    cur.execute('select data from chunk')
    try:
        for row in cur:
            yield row[0]
    finally:
        conn.close()


def bench(database, corpus_path, pcre_path, mode=hyperscan.HS_MODE_STREAM):
    if mode == hyperscan.HS_MODE_STREAM:
        with database.stream(match_event_handler=None) as stream:
            for blob in iter_corpus(corpus_path):
                stream.scan(blob)
    else:
        for blob in iter_corpus(corpus_path):
            database.scan(blob)


def main(args):
    mode = (
        hyperscan.HS_MODE_BLOCK
        if args.block_mode
        else hyperscan.HS_MODE_STREAM
    )
    expr_count, database = build_database(args.expressions, mode=mode)
    print('Signatures:\t\t{}'.format(args.expressions))
    print('Hyperscan info:\t\t{}'.format(database.info().decode('utf8')))
    print('Expression count:\t{}'.format(expr_count))
    print('Database size:\t\t{} bytes'.format(database.size()))
    print()

    print(
        "{} loops, best of 3: {}s".format(
            args.trials,
            timeit.timeit(
                lambda: bench(database, args.corpus, args.expressions, mode),
                number=args.trials,
            ),
        )
    )
    print()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Benchmark hyperscan-python')
    parser.add_argument('-n', '--trials', default=5, help='Number of trials.')
    parser.add_argument(
        '-e', '--expressions', metavar='FILE', help='Path to expressions.'
    )
    parser.add_argument(
        '-c', '--corpus', metavar='FILE', help='Path to corpus.'
    )
    parser.add_argument(
        '-N',
        '--block-mode',
        dest='block_mode',
        action='store_true',
        help='Benchmark in block mode.',
    )
    if len(sys.argv) == 1:
        parser.print_help()
        sys.exit(1)
    args = parser.parse_args()
    main(args)
