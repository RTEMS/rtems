#!/usr/bin/env python

# SPDX-License-Identifier: BSD-2-Clause

#
# Copyright (c) 2016 embedded brains GmbH & Co. KG
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
