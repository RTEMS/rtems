##
## $Id$
##


C_FILES = memcpy.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../automake/compile.am
include $(top_srcdir)/../../../../../automake/lib.am

# gcc doesn't recognize difference between the cpu32 and cpu32+ so we have to
if mcpu32p
AM_CPPFLAGS += -D__mcpu32p__
endif

all-local: $(ARCH) $(OBJS)

EXTRA_DIST = memcpy.c

include $(top_srcdir)/../../../../../automake/local.am
