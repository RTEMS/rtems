##
## $Id$
##


PGM = $(ARCH)/clock.rel

C_FILES = clock.c

clock_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(clock_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(clock_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = clock.c

include $(top_srcdir)/../../../../../automake/local.am
