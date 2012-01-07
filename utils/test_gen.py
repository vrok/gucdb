#!/usr/bin/python

import random
import sys

MAX_LEN = 9
TEST_LEN = 7000
REMOVE_THRESHOLD = 6

all_db = {}
db = {}

input_lines = []
output_lines = []

(WRITE, REMOVE, READ) = range(3)

for i in xrange(TEST_LEN):
    test_len = random.randint(1, MAX_LEN)
    
    chars = []
    for j in xrange(test_len):
        chars.append(chr(random.randint(33, 126)))
    key = ''.join(chars)

    what_op = random.randint(0, 2)

    if what_op == WRITE:
        if key in db:
            continue
        db[key] = random.randint(1, 10000)
        all_db[key] = db[key]
        input_lines.append('write %s %d' % (key, db[key]))
        output_lines.append('0')
    elif what_op == REMOVE:
        if random.randint(0, REMOVE_THRESHOLD) != 0:
            continue

        try:
            rand_index = random.randint(0, len(db) - 1)
        except ValueError:
            continue # ignore, db might still be empty
        key = db.keys()[rand_index]

        del db[key]
        input_lines.append('remove %s' % key)
        output_lines.append('0')
    else: # READ
        try:
            rand_index = random.randint(0, len(all_db) - 1)
        except ValueError:
            continue # ignore, db might still be empty
        key = all_db.keys()[rand_index]
        input_lines.append('read %s' % key)
        output_lines.append(str(db.get(key, 0)))

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


