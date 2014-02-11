#
# Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
#
# The license and distribution terms for this file may be
# found in the file LICENSE in this distribution or at
# http://www.rtems.com/license/LICENSE.
#

import libxml2
from libxml2 import xmlNode
import matplotlib.pyplot as plt
doc = libxml2.parseFile("tmcontext01.scn")
ctx = doc.xpathNewContext()

def plot(y):
	n=len(y)
	x=range(0, n)
	plt.plot(x, y)

plt.title("context switch timing test")
plt.xlabel('function nest level')
plt.ylabel('context switch time [ns]')

for e in ["normal", "dirty"]:
	for i in ["Min", "Q1", "Q2", "Q3", "Max"]:
		y=map(xmlNode.getContent, ctx.xpathEval("/Test/ContextSwitchTest[@environment='" + e + "' and not(@load)]/Sample/" + i))
		plot(y)
load=1
while load > 0:
	for i in ["Min", "Q1", "Q2", "Q3", "Max"]:
		y=map(xmlNode.getContent, ctx.xpathEval("/Test/ContextSwitchTest[@environment='dirty' and @load='" + str(load) + "']/Sample/" + i))
		if len(y) > 0:
			plot(y)
			load = load + 1
		else:
			load = 0
plt.show()
