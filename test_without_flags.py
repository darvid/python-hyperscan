#!/usr/bin/env python3

import hyperscan

print(f'hyperscan version: {hyperscan.__version__}')

# Same patterns from GitHub issue #207 but WITHOUT the problematic flags
bla = [r'<span\s+.*>السلام عليكم\s<\/span>'.encode('utf8'),
       r'<span\s+.*>ועליכום הסلאם\s<\/span>'.encode('utf8')]

print(f'Testing patterns: {bla}')

print('\n=== Testing WITH problematic flags (should fail) ===')
try:
    rules_db = hyperscan.Database()
    rules_db.compile(expressions=bla,
                     flags=hyperscan.HS_FLAG_UTF8 | hyperscan.HS_FLAG_UCP)
    print('SUCCESS: Patterns compiled with HS_FLAG_UTF8 | HS_FLAG_UCP!')
except Exception as e:
    print(f'FAILED: {e}')

print('\n=== Testing WITHOUT flags (should work) ===')
try:
    rules_db = hyperscan.Database()
    rules_db.compile(expressions=bla)
    print('SUCCESS: Patterns compiled without flags!')
except Exception as e:
    print(f'FAILED: {e}')

print('\n=== Testing with unicode strings (should work) ===')
try:
    unicode_patterns = [r'<span\s+.*>السلام عليكم\s<\/span>',
                       r'<span\s+.*>ועליכום הסلאם\s<\/span>']
    rules_db = hyperscan.Database()
    rules_db.compile(expressions=unicode_patterns)
    print('SUCCESS: Unicode patterns compiled without flags!')
except Exception as e:
    print(f'FAILED: {e}')