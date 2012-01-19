#!/usr/bin/python

import random
import sys
import os

MAX_LEN = 1024
TEST_LEN = 5

if len(sys.argv) != 3:
    print 'Usage test_slabs.py DB_BINARY TEMPORARY_DIR'
    sys.exit(-1)

binary = sys.argv[1]
temp_desired_result = sys.argv[2] + '/slabs.reference.tmp'
temp_in1_file = sys.argv[2] + '/slabs.in.write.tmp'
temp_out1_file = sys.argv[2] + '/slabs.out.write.tmp'
temp_in2_file = sys.argv[2] + '/slabs.in.read.tmp'
temp_out2_file = sys.argv[2] + '/slabs.out.read.tmp'

values = []
input_lines = []

def print_and_run(command):
    print ' * Running "%s"' % command
    os.system(command)

print 'Generating input writes'

for i in xrange(TEST_LEN):
    test_len = random.randint(1, MAX_LEN)
    
    chars = []
    for j in xrange(test_len):
        chars.append(chr(random.randint(33, 126)))
    val = ''.join(chars)

    values.append(val)
    input_lines.append('write %s' % val)

input_lines.append('exit')

with open(temp_in1_file, 'w') as generated_input_file:
    for line in input_lines:
        generated_input_file.write(line + '\n')

with open(temp_desired_result, 'w') as reference_file:
    for line in values:
        reference_file.write(line + '\n')

print 'Running writes'

print_and_run('%s < %s > %s 2>/dev/null' % (binary, temp_in1_file, temp_out1_file))

print 'Reading results, generating reads'

keys = []

with open(temp_out1_file, 'r') as write_output_file:
    while True:
        line = write_output_file.readline()
        if line == '':
            break

        line.rstrip()
        keys.append(line)

with open(temp_in2_file, 'w') as generated_input_file:
    for line in keys:
        generated_input_file.write('read %s' % line)
    generated_input_file.write('exit')

print 'Running reads'

print_and_run('%s < %s > %s 2>/dev/null' % (binary, temp_in2_file, temp_out2_file))

print 'Comparing'

print_and_run('cmp %s %s' % (temp_desired_result, temp_out2_file))

