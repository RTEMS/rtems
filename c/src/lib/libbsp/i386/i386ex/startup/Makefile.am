##
## $Id$
##

VPATH = @srcdir@:@srcdir@/../../../shared:@srcdir@/../../shared/comm:@srcdir@/../../shared/irq:@srcdir@/../../shared/io

PGM = $(ARCH)/startup.rel

C_FILES = bspclean.c bsplibc.c bsppost.c bspstart.c bootcard.c main.c sbrk.c \
    idt.c irq.c irq_init.c i386-stub.c i386-stub-glue.c uart.c \
    gnatinstallhandler.c gdb_glue.c tty_drv.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = irq_asm.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

startup_rel_OBJECTS = $(C_O_FILES) $(S_O_FILES)

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../../../automake/lib.am

#
# (OPTIONAL) Add local stuff here using +=
#

$(PGM): $(startup_rel_OBJECTS)
	$(make-rel)

bsplib_DATA = linkcmds

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

UNUSED_C_FILES = setvec.c

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

all-local: $(startup_rel_OBJECTS) $(PGM) $(TMPINSTALL_FILES)

.PRECIOUS: $(PGM)

EXTRA_DIST = README bspstart.c linkcmds setvec.c

include $(top_srcdir)/../../../../automake/local.am
