##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bspclean.c bsplibc.c bsppost.c bspstart.c bootcard.c \
    m68kpretaskinghook.c main.c page_table.c sbrk.c setvec.c \
    gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

# USE_INIT_FINI tells main.c what C++ help we need.
AM_CPPFLAGS += -DUSE_INIT_FINI

$(PGM): $(OBJS)
	$(make-rel)

bsplib_DATA = linkcmds

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

all-local: $(ARCH) $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = bspclean.c bspstart.c linkcmds page_table.c

include $(top_srcdir)/../../../../../../automake/local.am
