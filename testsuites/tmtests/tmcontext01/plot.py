# SPDX-License-Identifier: BSD-2-Clause

#
# Copyright (c) 2014 embedded brains GmbH & Co. KG
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
