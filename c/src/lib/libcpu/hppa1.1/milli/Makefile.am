##
## $Id$
##


PGM = $(ARCH)/milli.rel

S_FILES = milli.S

milli_rel_OBJECTS = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(milli_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(milli_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = milli.S

include $(top_srcdir)/../../../../../automake/local.am
