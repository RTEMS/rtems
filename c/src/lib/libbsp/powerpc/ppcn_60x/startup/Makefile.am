##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../../shared

PGM = $(ARCH)/startup.rel

C_FILES = bspstart.c bspclean.c sbrk.c setvec.c spurious.c genpvec.c swap.c \
    main.c bootcard.c bsplibc.c bsppost.c gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = bsptrap.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES) $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#FIXME: What is this?
# Files expected to be from the shared directory:
#  sbrk bootcard main bsplibc bsppost
STARTUP_C_PIECES = bspstart bspclean sbrk setvec spurious genpvec swap main \
    bootcard bsplibc bsppost

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(OBJS)
	$(make-rel)

bsplib_DATA = linkcmds

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

all-local: $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = bspclean.c bspstart.c bsptrap.S genpvec.c linkcmds \
    rtems-ctor.cc setvec.c spurious.c swap.c

include $(top_srcdir)/../../../../automake/local.am
