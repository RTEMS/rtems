##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../../shared

C_FILES = bsplibc.c bsppost.c bspstart.c exit.c bootcard.c main.c sbrk.c \
    gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

S_FILES =
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

startup_rel_OBJECTS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

# USE_INIT_FINI tells main.c what C++ help we need.

$(PGM): $(startup_rel_OBJECTS)
	$(make-rel)

bsplib_DATA = linkcmds

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

all: $(ARCH) $(startup_rel_OBJECTS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = linkcmds

include $(top_srcdir)/../../../../../../automake/local.am
