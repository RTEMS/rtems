##
## $Id$
##


PGM = $(ARCH)/network.rel

C_FILES = amd79c970.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

if HAS_NETWORKING
all-local: $(ARCH) $(OBJS) $(PGM)
endif

.PRECIOUS: $(PGM)

EXTRA_DIST = amd79c970.c amd79c970.h

include $(top_srcdir)/../../../../../../automake/local.am
