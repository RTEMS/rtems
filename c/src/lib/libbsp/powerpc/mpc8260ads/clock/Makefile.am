##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/clock

C_FILES = p_clock.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(OBJS)

include $(top_srcdir)/../../../../automake/local.am
