#!/usr/bin/python

import random
import sys

MAX_LEN = 9
TEST_LEN = 40

db = {}

input_lines = []
output_lines = []

for i in xrange(TEST_LEN):
    test_len = random.randint(1, MAX_LEN)
    
    chars = []
    for j in xrange(test_len):
        chars.append(chr(random.randint(33, 126)))
    key = ''.join(chars)

    if random.randint(0, 1):
        if key in db:
            continue
        db[key] = random.randint(1, 10000)
        input_lines.append('write %s %d' % (key, db[key]))
        output_lines.append('0')
    else:
        try:
            rand_index = random.randint(0, len(db) - 1)
        except ValueError:
            continue # ignore, db might still be empty
        key, value = db.items()[rand_index]
        input_lines.append('read %s' % key)
        output_lines.append(str(db[key]))

input_lines.append('exit')

if len(sys.argv) > 1:
    if len(sys.argv) != 3:
        print 'You must provide 0 or 2 extra arguments'
        sys.exit(-1)
    
    with open(sys.argv[1], 'w') as generated_input_file:
        for line in input_lines:
            generated_input_file.write(line + '\n')

    with open(sys.argv[2], 'w') as generated_output_file:
        for line in output_lines:
            generated_output_file.write(line + '\n')
else:
    print
    print 'INPUT:'
    print '-------'
    print '\n'.join(input_lines)

    print
    print 'OUTPUT:'
    print '-------'
    print '\n'.join(output_lines)


