##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../console:@srcdir@/../../shared/startup:@srcdir@/../../../shared

C_FILES = bootcard.c main.c bspstart.c bsppost.c bsplibc.c sbrk.c bspclean.c \
    gnatinstallhandler.c pgtbl_setup.c pgtbl_activate.c 
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

bsplib_DATA = linkcmds

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

# FIXME: This should not be here
# $(PROJECT_RELEASE)/lib/libbsp.a:
#	cd ../wrapup; $(MAKE) $(AM_MAKEFLAGS)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

all-local: $(OBJS) $(TMPINSTALL_FILES)

EXTRA_DIST = linkcmds

include $(top_srcdir)/../../../../automake/local.am
