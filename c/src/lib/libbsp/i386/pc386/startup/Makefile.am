## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

VPATH = \
    @srcdir@:@srcdir@/../../../shared:@srcdir@/../../shared/irq:@srcdir@/../../shared/comm:@srcdir@/../../shared/pci

PGM = ${ARCH}/startup.rel

## C source names
C_FILES = bsplibc.c bsppost.c bspstart.c exit.c irq.c irq_init.c bootcard.c \
    main.c sbrk.c i386-stub.c i386-stub-glue.c uart.c pcibios.c \
    gnatinstallhandler.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

## Assembly source names
S_FILES = ldsegs.S irq_asm.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

startup_rel_OBJECTS = $(C_O_FILES) $(S_O_FILES)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(RTEMS_ROOT)/make/leaf.cfg

#
# (OPTIONAL) Add local stuff here using +=
#

# USE_INIT_FINI tells main.c what C++ help we need.

AM_CPPFLAGS += -DUSE_INIT_FINI

$(PGM): ${startup_rel_OBJECTS}
	$(make-rel)

$(PROJECT_RELEASE)/lib/linkcmds: linkcmds
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/linkcmds

all-local: ${ARCH} $(PREINSTALL_FILES) $(PGM) $(TMPINSTALL_FILES)

EXTRA_DIST = bspstart.c exit.c sbrk.c ldsegs.S linkcmds

include $(top_srcdir)/../../../../../../automake/local.am
