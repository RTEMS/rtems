##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/vectorisrs.rel

C_FILES = maxvectors.c vectorisrs.c

vectorisrs_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o) $(S_FILES:%.S=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(vectorisrs_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(vectorisrs_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = vectorisrs.c

include $(top_srcdir)/../../../../../automake/local.am
