##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared/tod:@srcdir@/../../../shared

C_FILES = todcfg.c tod.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

all-local: $(ARCH) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
