##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../../shared:@srcdir@/../../shared/irq:@srcdir@/../../shared/comm:@srcdir@/../../shared/pci

PGM = $(ARCH)/startup.rel

C_FILES = bsplibc.c bsppost.c bspstart.c exit.c idt.c irq.c irq_init.c \
    bootcard.c main.c sbrk.c i386-stub.c i386-stub-glue.c uart.c pcibios.c \
    gnatinstallhandler.c gdb_glue.c tty_drv.c

C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = ldsegs.S irq_asm.S
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

EXTRA_DIST = bspstart.c exit.c ldsegs.S linkcmds sbrk.c

${PROJECT_RELEASE}/lib/linkcmds: linkcmds ${PROJECT_RELEASE}/lib/$(dirstamp)
	$(INSTALL_DATA) $< ${PROJECT_RELEASE}/lib/linkcmds
TMPINSTALL_FILES += ${PROJECT_RELEASE}/lib/linkcmds

include $(top_srcdir)/../../../../automake/local.am
