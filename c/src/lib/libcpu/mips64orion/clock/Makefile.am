##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

PGM = $(ARCH)/clock.rel

C_FILES = ckinit.c

S_FILES = clock.S

H_FILES = clock.h

clock_rel_OBJECTS = $(C_FILES:%.c=$(ARCH)/%.o) $(S_FILES:%.S=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(clock_rel_OBJECTS)
	$(make-rel)

all: $(ARCH) $(clock_rel_OBJECTS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = ckinit.c clock.S clock.h

include $(top_srcdir)/../../../../../automake/local.am
