##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/hw_init.rel

C_FILES = hw_init.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

## FIXME: This is gcc-specific.
AM_CFLAGS += -fomit-frame-pointer

$(PGM): $(OBJS)
	$(make-rel)

all-local: $(ARCH) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = hw_init.c

include $(top_srcdir)/../../../../../../automake/local.am
