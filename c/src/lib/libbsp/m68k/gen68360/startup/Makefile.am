##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../shared:@srcdir@/../../../shared:@srcdir@/../../shared/gdbstub::@srcdir@/../../../shared/gdbstub

PGM = $(ARCH)/startup.rel

C_FILES = alloc360.c bspclean.c bsplibc.c bsppost.c m68kpretaskinghook.c \
    bspstart.c init68360.c bootcard.c main.c sbrk.c setvec.c \
    gnatinstallhandler.c m68k-stub.c rtems-stub-glue.c

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

OBJS = $(C_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

# USE_INIT_FINI tells main.c what C++ help we need.
AM_CPPFLAGS += -DUSE_INIT_FINI -I @srcdir@/../../shared/gdbstub

$(PGM): $(OBJS)
	$(make-rel)

bsplib_DATA = linkcmds linkcmds.prom linkcmds.bootp

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/linkcmds.prom: linkcmds.prom
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/linkcmds.bootp: linkcmds.bootp
	$(INSTALL_DATA) $< $@

# the .rel file built here will be put into libbsp.a by ../wrapup/Makefile
TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds \
    $(PROJECT_RELEASE)/lib/linkcmds.prom \
    $(PROJECT_RELEASE)/lib/linkcmds.bootp

all-local: $(ARCH) $(OBJS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = alloc360.c bspclean.c bspstart.c init68360.c linkcmds \
    linkcmds.bootp linkcmds.prom

include $(top_srcdir)/../../../../../../automake/local.am
