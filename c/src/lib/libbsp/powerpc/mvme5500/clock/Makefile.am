##
## Makefile.am,v 1.6 2002/12/17 15:10:31 ralf Exp
##


VPATH = @srcdir@:@srcdir@/../../shared/clock

C_FILES = p_clock.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(ARCH) $(OBJS)

include $(top_srcdir)/../../../../../../automake/local.am
