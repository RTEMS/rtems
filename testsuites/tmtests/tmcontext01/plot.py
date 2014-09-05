#
# Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
#
# The license and distribution terms for this file may be
# found in the file LICENSE in this distribution or at
# http://www.rtems.org/license/LICENSE.
#

import libxml2
from libxml2 import xmlNode
import matplotlib.pyplot as plt
doc = libxml2.parseFile("tmcontext01.scn")
ctx = doc.xpathNewContext()

colors = ['k', 'r', 'b', 'g', 'y', 'm']

def plot(y, color, label, first):
	n=len(y)
	x=range(0, n)
	if first:
		plt.plot(x, y, color=color, label=label)
	else:
		plt.plot(x, y, color=color)

plt.title("context switch timing test")
plt.xlabel('function nest level')
plt.ylabel('context switch time [ns]')

c = 0
for e in ["normal", "dirty"]:
	first = True
	for i in ["Min", "Q1", "Q2", "Q3", "Max"]:
		y=map(xmlNode.getContent, ctx.xpathEval("/Test/ContextSwitchTest[@environment='" + e + "' and not(@load)]/Sample/" + i))
		plot(y, colors[c], e, first)
		first = False
	c = c + 1
load = 1
while load > 0:
	first = True
	for i in ["Min", "Q1", "Q2", "Q3", "Max"]:
		y=map(xmlNode.getContent, ctx.xpathEval("/Test/ContextSwitchTest[@environment='dirty' and @load='" + str(load) + "']/Sample/" + i))
		if len(y) > 0:
			plot(y, colors[c], "load " + str(load), first)
			first = False
		else:
			load = 0
	if load > 0:
		load = load + 1
	c = c + 1
plt.legend()
plt.show()
