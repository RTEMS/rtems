#!/usr/bin/env python

#
# Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
#
# The license and distribution terms for this file may be
# found in the file LICENSE in this distribution or at
# http://www.rtems.org/license/LICENSE.
#

import re
import libxml2
from libxml2 import xmlNode
import matplotlib.pyplot as plt
data = open('smpopenmp01.scn').read()
data = re.sub(r'\*\*\*.*', '', data)
doc = libxml2.parseDoc(data)
ctx = doc.xpathNewContext()

plt.title('OpenMP Microbench')
plt.xlabel('Number of Threads')
plt.ylabel('Relative Duration')

def m(n):
	return float(n.getContent())

def p(bench):
	d = map(m, ctx.xpathEval('/SMPOpenMP01/Microbench/' + bench))
	y = [x / d[0] for x in d]
	x = range(1, len(y) + 1)
	plt.xticks(x)
	plt.plot(x, y, label = bench, marker = 'o')

p('BarrierBench')
p('ParallelBench')
p('StaticBench')
p('DynamicBench')
p('GuidedBench')
p('RuntimeBench')
p('SingleBench')
plt.legend(loc = 'best')
plt.show()
