## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

PGM = ${ARCH}/milli.rel

## Assembly source names
S_FILES = milli.S

milli_rel_OBJECTS = $(S_FILES:%.S=${ARCH}/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(milli_rel_OBJECTS)
	$(make-rel)

all-local: ${ARCH} $(PGM)

EXTRA_DIST = $(S_FILES)

include $(top_srcdir)/../../../../../automake/local.am
