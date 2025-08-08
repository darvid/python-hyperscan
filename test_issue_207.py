#!/usr/bin/env python3

import hyperscan

print(f'hyperscan version: {hyperscan.__version__}')

# Exact code from GitHub issue #207
bla = [r'<span\s+.*>السلام عليكم\s<\/span>'.encode('utf8'),
       r'<span\s+.*>ועליכום הסלאם\s<\/span>'.encode('utf8')]

print(f'Testing patterns: {bla}')

try:
    rules_db = hyperscan.Database()
    rules_db.compile(expressions=bla,
                     flags=hyperscan.HS_FLAG_UTF8 | hyperscan.HS_FLAG_UCP)
    print('SUCCESS: Patterns compiled with HS_FLAG_UTF8 | HS_FLAG_UCP!')
except Exception as e:
    print(f'FAILED: {e}')
    if 'Expression is not valid UTF-8' in str(e):
        print('*** THIS IS THE EXACT BUG FROM ISSUE #207! ***')
    else:
        print('*** Different error ***')