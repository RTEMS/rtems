##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../../shared

C_FILES = clock_driver_stub.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

all-local: $(ARCH) $(OBJS)

.PRECIOUS:

EXTRA_DIST = 

include $(top_srcdir)/../../../../../../automake/local.am
