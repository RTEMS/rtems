##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/../console:@srcdir@/../../../shared:@srcdir@/../../shared/startup

C_FILES = bootcard.c main.c bspstart.c bsppost.c bsplibc.c sbrk.c bspclean.c \
    gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

# FIXME: This should not be here
# $(PROJECT_RELEASE)/lib/libbsp.a:
#	cd ../wrapup; $(MAKE) $(AM_MAKEFLAGS)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

all-local: $(ARCH) $(OBJS) $(TMPINSTALL_FILES)

EXTRA_DIST = linkcmds

include $(top_srcdir)/../../../../../../automake/local.am
