#!/usr/bin/env python

#
# Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
#
# The license and distribution terms for this file may be
# found in the file LICENSE in this distribution or at
# http://www.rtems.org/license/LICENSE.
#

import re
import libxml2
from libxml2 import xmlNode
import matplotlib.pyplot as plt
data = open('sptimecounter02.scn').read()
data = re.sub(r'\*\*\*.*\*\*\*', '', data)
doc = libxml2.parseDoc(data)
ctx = doc.xpathNewContext()

plt.title('Timestamp Performance')
plt.xlabel('Active Workers')
plt.ylabel('Operation Count')

def m(n):
	return int(n.getContent())

def getCounterSums(variant):
	w = 1
	y = []
	while True:
		c = map(m, ctx.xpathEval('/SPTimecounter01/' + variant + '[@activeWorker="' + str(w) + '"]/Counter'))
		if not c:
			break
		y.append(sum(c))
		w = w + 1
	return y

y = getCounterSums('BinuptimeTest')
x = range(1, len(y) + 1)
plt.xticks(x)
plt.plot(x, y, marker = 'o')
plt.show()
