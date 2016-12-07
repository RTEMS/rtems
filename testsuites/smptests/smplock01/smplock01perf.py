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
data = open('smplock01.scn').read()
data = re.sub(r'\*\*\*.*\*\*\*', '', data)
doc = libxml2.parseDoc(data)
ctx = doc.xpathNewContext()

plt.title('SMP Lock Performance')
plt.xlabel('Active Workers')
plt.ylabel('Operation Count')

y = map(xmlNode.getContent, ctx.xpathEval('/SMPLock01/GlobalTicketLockWithLocalCounter/SumOfLocalCounter'))
x = range(1, len(y) + 1)
plt.xticks(x)
plt.plot(x, y, label = 'Ticket Lock', marker = 'o')

y = map(xmlNode.getContent, ctx.xpathEval('/SMPLock01/GlobalMCSLockWithLocalCounter/SumOfLocalCounter'))
plt.plot(x, y, label = 'MCS Lock', marker = 'o')

y = map(xmlNode.getContent, ctx.xpathEval('/SMPLock01/GlobalTASLockWithLocalCounter/SumOfLocalCounter'))
plt.plot(x, y, label = 'TAS Lock', marker = 'o')

y = map(xmlNode.getContent, ctx.xpathEval('/SMPLock01/GlobalTTASLockWithLocalCounter/SumOfLocalCounter'))
plt.plot(x, y, label = 'TTAS Lock', marker = 'o')

plt.legend(loc = 'best')
plt.show()
