##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bspclean.c bsplibc.c bsppost.c m68kpretaskinghook.c bspstart.c \
    init5206e.c bootcard.c main.c sbrk.c setvec.c gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

project_lib_DATA = linkcmds gdbinit linkcmds.flash

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile

all-local: $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = bspclean.c bspstart.c init5206e.c linkcmds linkcmds.flash \
    gdbinit

${PROJECT_RELEASE}/lib/linkcmds: linkcmds ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds

${PROJECT_RELEASE}/lib/gdbinit: gdbinit ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/gdbinit
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/gdbinit

${PROJECT_RELEASE}/lib/linkcmds.flash: linkcmds.flash ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds.flash
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds.flash

include $(top_srcdir)/../../../../automake/local.am
