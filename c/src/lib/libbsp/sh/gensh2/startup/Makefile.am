##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = hw_init.c bsplibc.c bsppost.c bspstart.c bspclean.c sbrk.c \
    bootcard.c main.c gnatinstallhandler.c
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
TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

$(PGM): $(OBJS)
	$(make-rel)

all-local: $(PREINSTALL_FILES) $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = bspclean.c bspstart.c hw_init.c linkcmds linkcmds.ram \
    linkcmds.rom

include $(top_srcdir)/../../../../automake/local.am
