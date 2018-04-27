#! /usr/bin/env python
#
# Copyright 2017 Chris Johns <chrisj@rtems.org>
# All rights reserved
#

#
# Python version the rtems-test-check script.
#

from __future__ import print_function
import os.path
import sys

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

#
# Search the include paths for a file.
#
def find_testdata(paths, name):
    for p in paths:
        fn = os.path.join(p, name)
        if os.path.exists(fn):
            return fn
    return None

#
# Arguments. Keep it simple.
#
if len(sys.argv) < 4:
    eprint('error: invalid command line')
    print('INVALID-TEST-DATA')
    sys.exit(2)

verbose = False
args = 0

if sys.argv[1] == '-v':
    verbose = True
    args = 1

mode = sys.argv[args + 1]
bsp = sys.argv[args + 2]
includepaths = sys.argv[args + 4].split(':')
testconfig = [find_testdata(includepaths, sys.argv[args + 3])]
tests = sys.argv[args + 5:]

if verbose:
    eprint('cmd: %s' % (' '.join(sys.argv)))

#
# Handle the modes.
#
if mode == 'exclude':
    pass
elif mode == 'flags':
    if len(tests) != 1:
        eprint('error: test count not 1 for mode: %s' % (mode))
        print('INVALID-TEST-DATA')
        sys.exit(1)
else:
    eprint('error: invalid mode: %s' % (mode))
    print('INVALID-TEST-DATA')
    sys.exit(1)

#
# Common RTEMS testsuite configuration. Load first.
#
rtems_testdata = find_testdata(includepaths, os.path.join('testdata', 'rtems.tcfg'))
if rtems_testdata is not None:
    testconfig.insert(0, rtems_testdata)

states = ['exclude',
          'expected-fail',
          'user-input',
          'indeterminate',
          'benchmark']
defines = { 'expected-fail' : '-DTEST_STATE_EXPECTED_FAIL=1',
            'user-input'    : '-DTEST_STATE_USER_INPUT=1',
            'indeterminate' : '-DTEST_STATE_INDETERMINATE=1',
            'benchmark'     : '-DTEST_STATE_BENCHMARK=1' }
output = []
testdata = {}

if verbose:
    eprint('mode: %s' % (mode))
    eprint('testconfig: %r' % (testconfig))
    eprint('testconfig: %s' % (', '.join([x for x in testconfig if x is not None])))
    eprint('includepaths: %s' % (includepaths))
    eprint('bsp: %s' % (bsp))
    eprint('tests: %s' % (', '.join(tests)))

def clean(line):
    line = line[0:-1]
    b = line.find('#')
    if b >= 0:
        line = line[1:b]
    return line.strip()

#
# Load the test data.
#
while len(testconfig):
    tc = testconfig[0]
    testconfig.remove(tc)
    if tc is None:
        continue
    if verbose:
        eprint('reading: %s' % (tc))
    if not os.path.exists(tc):
        if verbose:
            eprint('%s: not found' % (tc))
        continue
    with open(tc) as f:
        tdata = [clean(l) for l in f.readlines()]
    lc = 0
    for line in tdata:
        lc += 1
        ls = [s.strip() for s in line.split(':')]
        if len(line) == 0:
            continue
        if verbose:
            eprint('%4d: %s' % (lc, line))
        if len(ls) != 2:
            eprint('error: syntax error: %s:%d' % (tc, lc))
            print('INVALID-TEST-DATA')
            sys.exit(1)
        state = ls[0]
        test = ls[1]
        if state == 'include':
            td = find_testdata(includepaths, test)
            if td is None:
                eprint('error: include not found: %s:%d' % (tc, lc))
                print('INVALID-TEST-DATA')
            testconfig.insert(0, td)
            if verbose:
                eprint('include: %s' % (', '.join(testconfig)))
        elif state in states:
            if state not in testdata:
                testdata[state] = [test]
            else:
                testdata[state] += [test]
        else:
            eprint('error: invalid test state: %s in %s:%d' % (state, tc, lc))
            print('INVALID-TEST-DATA')
            sys.exit(1)

for test in tests:
    if mode == 'exclude':
        if 'exclude' not in testdata or test not in testdata['exclude']:
            output += [test]
    elif mode == 'flags':
        for state in states:
            if state != 'exclude' and state in testdata and test in testdata[state]:
                output += [defines[state]]

print(' '.join(sorted(set(output))))

sys.exit(0)
