##
## $Id$
##


PGM = ${ARCH}/vectors.rel

## Assembly sources
S_FILES = vectors.S

vectors_rel_OBJECTS = $(S_FILES:%.S=${ARCH}/%.$(OBJEXT))

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(vectors_rel_OBJECTS)
	$(make-rel)

all-local: ${ARCH} $(PGM)

EXTRA_DIST = vectors.S

include $(top_srcdir)/../../../../../automake/local.am
