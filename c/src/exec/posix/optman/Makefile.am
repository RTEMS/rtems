##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES =
H_FILES =

C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

SRCS = $(C_FILES)
OBJS = $(C_O_FILES)

PGMS = $(C_FILES:%.c=$(ARCH)/%.rel)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg

AM_CFLAGS += $(CFLAGS_OS_V)

include $(top_srcdir)/../../../../automake/local.am
