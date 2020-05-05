#
# Copyright (c) 2019 Chris Johns <chrisj@rtems.org>.
# All rights reserved.
#
# The license and distribution terms for this file may be
# found in the file LICENSE in this distribution or at
# http://www.rtems.org/license/LICENSE.
#
# Check the allocations for libdl:
#
#  1. Turn on the allocation trace.
#
#  2. Load and unload object files.
#
#  3. Capture the trace output and feed to this tool
#

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
