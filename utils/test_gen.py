
import random

MAX_LEN = 3
TEST_LEN = 20

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

print
print 'INPUT:'
print '-------'
print '\n'.join(input_lines)

print
print 'OUTPUT:'
print '-------'
print '\n'.join(output_lines)


