##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

C_FILES = debug.c entrytable.c extension.c fatal.c exinit.c io.c itronapi.c \
    posixapi.c rtemsapi.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

SRCS = $(C_FILES)
OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D__RTEMS_INSIDE__
AM_CFLAGS += $(CFLAGS_OS_V)

all: ${ARCH} ${OBJS}

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../automake/local.am
