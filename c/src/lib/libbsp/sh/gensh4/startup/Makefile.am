##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bsplibc.c bsppost.c bspstart.c bspclean.c sbrk.c \
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

$(PROJECT_RELEASE)/lib/linkcmds.rom: linkcmds.rom
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/linkcmds.rom2ram: linkcmds.rom2ram
	$(INSTALL_DATA) $< $@

$(PGM): $(OBJS)
	$(make-rel)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds $(PROJECT_RELEASE)/lib/linkcmds.rom \
                    $(PROJECT_RELEASE)/lib/linkcmds.rom2ram

all-local: $(ARCH) $(TMPINSTALL_FILES) $(OBJS) $(PGM)

.PRECIOUS: $(PGM)

EXTRA_DIST = bspclean.c bspstart.c linkcmds linkcmds.rom linkcmds.rom2ram

include $(top_srcdir)/../../../../../../automake/local.am
