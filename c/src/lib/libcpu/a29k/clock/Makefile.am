##
## $Id$
##


PGM = $(ARCH)/clock.rel

C_FILES = ckinit.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

noinst_HEADERS = clock.h

S_FILES = clock.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

all-local: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = Makefile ckinit.c clock.S clock.h

include $(top_srcdir)/../../../../../automake/local.am
