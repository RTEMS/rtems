##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/residual

C_FILES = residual.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

H_FILES = ../../shared/residual/pnp.h ../../shared/residual/residual.h

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(PREINSTALL_FILES) $(OBJS)

include $(top_srcdir)/../../../../automake/local.am
