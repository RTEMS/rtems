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

project_lib_DATA = linkcmds linkcmds-le linkcmds-le.coff

$(PGM): $(OBJS)
	$(make-rel)

all-local: $(TMPINSTALL_FILES) $(PGM)

EXTRA_DIST = bspclean.c bspstart.c hw_init.c linkcmds linkcmds-le \
    linkcmds-le.coff

${PROJECT_RELEASE}/lib/$(dirstamp):
	@$(mkdir_p) ${PROJECT_RELEASE}/lib
	@: > ${PROJECT_RELEASE}/lib/$(dirstamp)

${PROJECT_RELEASE}/lib/linkcmds: linkcmds ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds

${PROJECT_RELEASE}/lib/linkcmds-le: linkcmds-le ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds-le
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds-le

${PROJECT_RELEASE}/lib/linkcmds-le.coff: linkcmds-le.coff ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds-le.coff
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds-le.coff

include $(top_srcdir)/../../../../automake/local.am
