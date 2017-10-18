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
data = open('tmfine01.scn').read()
data = re.sub(r'\*\*\*.*\*\*\*', '', data)
doc = libxml2.parseDoc(data)
ctx = doc.xpathNewContext()

plt.yscale('log')
plt.title('Uncontested Mutex Performance')
plt.xlabel('Active Workers')
plt.ylabel('Operation Count')

def m(n):
	return int(n.getContent())

def getCounterSums(variant):
	w = 1
	y = []
	while True:
		c = map(m, ctx.xpathEval('/TestTimeFine01/' + variant + '[@activeWorker="' + str(w) + '"]/Counter'))
		if not c:
			break
		y.append(sum(c))
		w = w + 1
	return y

y = getCounterSums('ManySysLockMutex')
x = range(1, len(y) + 1)
plt.plot(x, y, label = 'Sys Lock Mutex', marker = 'o')

y = getCounterSums('ManyMutex')
plt.plot(x, y, label = 'Classic Inheritance Mutex', marker = 'o')

y = getCounterSums('ManyClassicCeilingMutex')
plt.plot(x, y, label = 'Classic Ceiling Mutex', marker = 'o')

y = getCounterSums('ManyClassicMrsPMutex')
plt.plot(x, y, label = 'Classic MrsP Mutex', marker = 'o')

y = getCounterSums('ManyPthreadSpinlock')
plt.plot(x, y, label = 'Pthread Spinlock', marker = 'o')

y = getCounterSums('ManyPthreadMutexInherit')
plt.plot(x, y, label = 'Pthread Mutex Inherit', marker = 'o')

y = getCounterSums('ManyPthreadMutexProtect')
plt.plot(x, y, label = 'Pthread Mutex Protect', marker = 'o')

plt.legend(loc = 'best')
plt.show()
