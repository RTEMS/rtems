##
## $Id$
##

C_FILES = p_clock.c
OBJS = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

# the .o files built here will be put into libbsp.a by ../wrapup/Makefile

noinst_DATA = $(OBJS)

include $(top_srcdir)/../../../../automake/local.am
