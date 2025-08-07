#!/usr/bin/env python3

import hyperscan
print(f'hyperscan version: {hyperscan.__version__}')

# Test unicode pattern compilation
patterns = ['السلام عليكم', 'ועליכום הסלאם']
print(f'Testing unicode patterns: {patterns}')

try:
    db = hyperscan.Database()
    db.compile(expressions=patterns)
    print('SUCCESS: Unicode patterns compiled without errors!')
except Exception as e:
    print(f'FAILED: {str(e)}')
    if 'Expression is not valid UTF-8' in str(e):
        print('*** THIS IS THE BUG - the fix is NOT working! ***')
    else:
        print('*** Different error, not the unicode bug ***')