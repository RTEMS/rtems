#! /usr/bin/env python
#
# Copyright 2018 Chris Johns (chrisj@rtems.org)
# All rights reserved.
#
# This file is part of the RTEMS Tools package in 'rtems-tools'.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
#

#
# Python version the rtems-test-check script.
#

from __future__ import print_function

import os
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
sys_args = sys.argv[1:]
if len(sys_args) < 4:
    eprint('error: invalid command line')
    print('INVALID-TEST-DATA')
    sys.exit(2)

verbose = False
args = 0

if sys_args[1] == '-v':
    verbose = True
    args = 1

mode = sys_args[args + 1]
bsp = sys_args[args + 2]
includepaths = sys_args[args + 4].split(os.pathsep)
testconfig = [find_testdata(includepaths, sys_args[args + 3])]
tests = sys_args[args + 5:]

if verbose:
    eprint('cmd: %s' % (' '.join(sys_args)))

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
