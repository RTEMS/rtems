##
##  $Id$
##


EXTENSION_FILES = extension.c extensioncreate.c extensiondelete.c \
    extensionident.c
C_FILES = debug.c $(EXTENSION_FILES)  fatal.c exinit.c io.c \
    itronapi.c posixapi.c rtemsapi.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

OBJS = $(C_O_FILES)

include $(top_srcdir)/automake/multilib.am
include $(top_srcdir)/automake/compile.am
include $(top_srcdir)/automake/lib.am

#
# Add local stuff here using +=
#

AM_CPPFLAGS += -D__RTEMS_INSIDE__

all-local: ${ARCH} ${OBJS}

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/automake/local.am
