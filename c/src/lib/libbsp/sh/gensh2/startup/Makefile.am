##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = hw_init.c bsplibc.c bsppost.c bspstart.c bspclean.c sbrk.c \
    bootcard.c main.c gnatinstallhandler.c
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

$(PGM): $(OBJS)
	$(make-rel)

PREINSTALL_FILES = $(PROJECT_RELEASE)/lib/linkcmds

all-local: $(ARCH) $(PREINSTALL_FILES) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = bspclean.c bspstart.c hw_init.c linkcmds linkcmds.ram \
    linkcmds.rom

include $(top_srcdir)/../../../../../../automake/local.am
