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
import math
import statistics
from libxml2 import xmlNode
import matplotlib.pyplot as plt
data = open('smplock01.scn').read()
data = re.sub(r'\*\*\*.*\*\*\*', '', data)
doc = libxml2.parseDoc(data)
ctx = doc.xpathNewContext()

plt.title('SMP Lock Fairness')
plt.xlabel('Active Workers')
plt.ylabel('Normed Coefficient of Variation')

i = 1
ticket = []
mcs = []
tas = []
ttas = []

def m(n):
	return int(xmlNode.getContent(n))

def normedCoefficientOfVariation(name, i):
	y = map(m, ctx.xpathEval('/SMPLock01/' + name + '[@activeWorker=' + str(i) + ']/LocalCounter'))
	if len(y) == 0:
		raise
	return (statistics.stdev(y) / statistics.mean(y)) / math.sqrt(len(y))

try:
	while True:
		i = i + 1
		ticket.append(normedCoefficientOfVariation('GlobalTicketLockWithLocalCounter', i))
		mcs.append(normedCoefficientOfVariation('GlobalMCSLockWithLocalCounter', i))
		tas.append(normedCoefficientOfVariation('GlobalTASLockWithLocalCounter', i))
		ttas.append(normedCoefficientOfVariation('GlobalTTASLockWithLocalCounter', i))
except:
	pass

x = range(2, len(ticket) + 2)
plt.xticks(x)
plt.yscale('symlog', linthreshy = 1e-6)
plt.plot(x, ticket, label = 'Ticket Lock', marker = 'o')
plt.plot(x, mcs, label = 'MCS Lock', marker = 'o')
plt.plot(x, tas, label = 'TAS Lock', marker = 'o')
plt.plot(x, ttas, label = 'TTAS Lock', marker = 'o')
plt.legend(loc = 'best')
plt.show()
