##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../../shared

C_FILES = bsplibc.c bsppost.c bspstart.c exit.c bootcard.c main.c sbrk.c \
    gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES =
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

startup_rel_OBJECTS = $(C_O_FILES) $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(startup_rel_OBJECTS)
	$(make-rel)

project_lib_DATA = linkcmds

all-local: $(startup_rel_OBJECTS) $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = linkcmds

${PROJECT_RELEASE}/lib/$(dirstamp):
	@$(mkdir_p) ${PROJECT_RELEASE}/lib
	@: > ${PROJECT_RELEASE}/lib/$(dirstamp)

${PROJECT_RELEASE}/lib/linkcmds: linkcmds ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds

include $(top_srcdir)/../../../../automake/local.am
