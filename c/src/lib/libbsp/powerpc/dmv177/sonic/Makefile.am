##
## $Id$
##

C_FILES = dmvsonic.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

AM_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__

if HAS_NETWORKING
noinst_DATA = $(OBJS)
endif

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../../automake/local.am
