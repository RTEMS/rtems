##
## $Id$
##


VPATH = @srcdir@:@srcdir@/../../../shared:@srcdir@/../../shared/irq:@srcdir@/../../shared/comm:@srcdir@/../../shared/pci

PGM = $(ARCH)/startup.rel

C_FILES = bsplibc.c bsppost.c bspstart.c exit.c idt.c irq.c irq_init.c \
    bootcard.c main.c sbrk.c i386-stub.c i386-stub-glue.c uart.c pcibios.c \
    gnatinstallhandler.c gdb_glue.c tty_drv.c

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

S_FILES = ldsegs.S irq_asm.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

startup_rel_OBJECTS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

# USE_INIT_FINI tells main.c what C++ help we need.

AM_CPPFLAGS += -DUSE_INIT_FINI

$(PGM): $(startup_rel_OBJECTS)
	$(make-rel)

bsplib_DATA = linkcmds

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

all-local: $(ARCH) $(startup_rel_OBJECTS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = bspstart.c exit.c ldsegs.S linkcmds sbrk.c

include $(top_srcdir)/../../../../../../automake/local.am
