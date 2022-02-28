# SPDX-License-Identifier: BSD-2-Clause

#
# Check the allocations for libdl:
#
#  1. Turn on the allocation trace.
#
#  2. Load and unload object files.
#
#  3. Capture the trace output and feed to this tool

# Copyright (c) 2019 Chris Johns <chrisj@rtems.org>.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

from __future__ import print_function

import argparse


class libdl_trace(object):
    def __init__(self, name):
        self.name = name
        self.trace = {'alloc': []}

    def load(self):
        with open(self.name, 'r') as f:
            lc = 0
            for line in f:
                line = line[:-1]
                lc += 1
                if line.startswith('rtl: '):
                    if line.startswith('rtl: alloc: '):
                        self.trace['alloc'] += [(lc, line)]

    def check_allocs(self):
        allocs = {}
        locks = 0
        wr_enable = False
        for lc, line in self.trace['alloc']:
            ls = line.split(' ')
            if len(ls) > 3:
                if ls[2] == 'new:':
                    addr = ls[4].split('=')[1]
                    size = ls[5].split('=')[1]
                    count = 0
                    if addr in allocs:
                        alc, alloced, asize, count = allocs[addr]
                        if alloced:
                            print(
                                '%5d: already alloced: %5d: addr=%-9s size=%-9s count=%d'
                                % (lc, alc, addr, asize, count))
                    allocs[addr] = (lc, True, size, count + 1)
                elif ls[2] == 'del:':
                    addr = ls[4].split('=')[1]
                    if addr != '0':
                        if addr not in allocs:
                            print('%5d: delete never alloced: addr=%s' %
                                  (lc, addr))
                        else:
                            alc, alloced, size, count = allocs[addr]
                            if not alloced:
                                print(
                                    '%5d: delete not alloced: %5d: addr=%-9s size=%-9s count=%d'
                                    % (lc, alc, addr, size, count))
                        allocs[addr] = (lc, False, size, count)
        alloced_remaiing = 0
        addresses = sorted(list(allocs.keys()))
        for addr in addresses:
            lc, alloced, size, count = allocs[addr]
            if alloced:
                print('%5d: never deleted: addr=%-9s size=%-9s count=%d' %
                      (lc, addr, size, count))
                alloced_remaiing += int(size)
        if alloced_remaiing != 0:
            print("Amount alloced: %d" % (alloced_remaiing))


def run(args):
    argsp = argparse.ArgumentParser(prog='rtl-alloc-check',
                                    description='Audit libdl allocations')
    argsp.add_argument('traces', help='libdl trace files', nargs='+')

    opts = argsp.parse_args(args[1:])

    for t in opts.traces:
        trace = libdl_trace(t)
        trace.load()
        trace.check_allocs()


if __name__ == "__main__":
    import sys
    run(sys.argv)
