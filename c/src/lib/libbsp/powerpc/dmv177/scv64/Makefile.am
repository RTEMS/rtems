##
## $Id$
##

C_FILES = scv64.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

noinst_DATA = $(OBJS)

EXTRA_DIST = $(C_FILES)

include $(top_srcdir)/../../../../automake/local.am
