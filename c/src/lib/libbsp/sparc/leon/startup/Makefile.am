##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bspclean.c bsplibc.c bsppost.c bspstart.c main.c bootcard.c sbrk.c \
    setvec.c spurious.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = boardinit.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

project_lib_DATA = linkcmds

all-local: $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = linkcmds setvec.c spurious.c boardinit.S

${PROJECT_RELEASE}/lib/linkcmds: linkcmds ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds

include $(top_srcdir)/../../../../automake/local.am
