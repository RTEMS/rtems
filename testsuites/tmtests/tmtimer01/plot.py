#
# Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
#
# The license and distribution terms for this file may be
# found in the file LICENSE in this distribution or at
# http://www.rtems.org/license/LICENSE.
#

import libxml2
from libxml2 import xmlNode
import matplotlib.pyplot as plt
doc = libxml2.parseFile('tmtimer01.scn')
ctx = doc.xpathNewContext()

plt.title('timer test')
plt.xscale('log')
plt.xlabel('active timers')
plt.ylabel('timer fire and cancel [ns]')

x = map(xmlNode.getContent, ctx.xpathEval('/TMTimer01/Sample/ActiveTimers'))
for i in ['First', 'Middle', 'Last']:
	y = map(xmlNode.getContent, ctx.xpathEval('/TMTimer01/Sample/' + i))
	plt.plot(x, y, label = i)
plt.legend(loc = 'best')
plt.show()
