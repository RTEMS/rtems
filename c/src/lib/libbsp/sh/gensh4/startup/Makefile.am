##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bsplibc.c bsppost.c bspstart.c bspclean.c sbrk.c bootcard.c main.c \
    gnatinstallhandler.c

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

project_lib_DATA = linkcmds linkcmds.rom linkcmds.rom2ram

$(PGM): $(OBJS)
	$(make-rel)

all-local: $(TMPINSTALL_FILES) $(PGM)

EXTRA_DIST = bspclean.c bspstart.c linkcmds linkcmds.rom linkcmds.rom2ram

${PROJECT_RELEASE}/lib/linkcmds: linkcmds ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds

${PROJECT_RELEASE}/lib/linkcmds.rom: linkcmds.rom ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds.rom
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds.rom

${PROJECT_RELEASE}/lib/linkcmds.rom2ram: linkcmds.rom2ram ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds.rom2ram
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds.rom2ram

include $(top_srcdir)/../../../../automake/local.am
