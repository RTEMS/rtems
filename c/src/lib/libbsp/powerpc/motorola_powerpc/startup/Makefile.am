##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../console:@srcdir@/../../shared/startup:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bootcard.c main.c bspstart.c bsppost.c bsplibc.c sbrk.c bspclean.c \
    gnatinstallhandler.c pgtbl_setup.c pgtbl_activate.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

project_lib_DATA = linkcmds

all-local: $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = linkcmds

${PROJECT_RELEASE}/lib/$(dirstamp):
	@$(mkdir_p) ${PROJECT_RELEASE}/lib
	@: > ${PROJECT_RELEASE}/lib/$(dirstamp)

${PROJECT_RELEASE}/lib/linkcmds: linkcmds ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds

include $(top_srcdir)/../../../../automake/local.am
