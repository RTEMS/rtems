## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = \
    @srcdir@:@srcdir@/../../../shared:@srcdir@/../../shared/comm:@srcdir@/../../shared/irq:@srcdir@/../../shared/io

PGM = ${ARCH}/startup.rel

## C source names
C_FILES = bspclean.c bsplibc.c bsppost.c bspstart.c bootcard.c main.c sbrk.c \
    irq.c irq_init.c i386-stub.c i386-stub-glue.c uart.c gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

## Assembly source names
S_FILES = irq_asm.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

startup_rel_OBJECTS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg

#
# (OPTIONAL) Add local stuff here using +=
#

## FIXME: This doesn't seem to be useful
## AM_CPPFLAGS += -I$(srcdir)

AM_CPPFLAGS += -DUSE_INIT_FINI

$(PGM): ${startup_rel_OBJECTS}
	$(make-rel)

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

all-local: ${ARCH} $(PREINSTALL_FILES) $(PGM) $(TMPINSTALL_FILES)

UNUSED_C_FILES = setvec.c
EXTRA_DIST = README bspstart.c linkcmds $(UNUSED_C_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
