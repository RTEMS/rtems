##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared/motorola

C_FILES = motorola.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

# FIXME: This doesn't give any sense here
H_FILES = ../../shared/motorola/motorola.h

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(ARCH) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
