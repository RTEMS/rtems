##
## $Id$
##


PGM = $(ARCH)/timer.rel

C_FILES = timer.c

S_FILES = gettime.S

timer_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT)) $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(timer_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(timer_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = gettime.S timer.c

include $(top_srcdir)/../../../../../automake/local.am
