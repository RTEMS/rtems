##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared/startup:@srcdir@/../../../shared

INCLUDES = -I @srcdir@/../GT64260

C_FILES = bootcard.c main.c bspstart.c bspclean.c bsppost.c bsplibc.c \
	sbrk.c gnatinstallhandler.c builddate.c pgtbl_setup.c \
	pgtbl_activate.c reboot.c

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

builddate.c::
	echo 'char *BSP_build_date="'`date +%Y%m%d%Z%T`'";' > builddate.c

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#
bsplib_DATA = linkcmds

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

all-local: $(ARCH) $(OBJS) $(TMPINSTALL_FILES) 

EXTRA_DIST = linkcmds bspstart.c bspclean.c bootpstuff.c reboot.c

include $(top_srcdir)/../../../../../../automake/local.am
