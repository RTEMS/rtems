##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

SUBDIRS = rtems

C_FILES = cpu.c memcpy.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

H_FILES = asm.h m68302.h m68360.h qsm.h sim.h

S_FILES = cpu_asm.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/lib.am

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib/rtems$(LIB_VARIANT).o: $(ARCH)/rtems.o
	$(INSTALL_DATA) $< $@

REL = $(ARCH)/rtems-cpu.rel

rtems_cpu_rel_OBJECTS = $(C_O_FILES) $(S_O_FILES)

$(REL): $(rtems_cpu_rel_OBJECTS)
	$(make-rel)

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(H_FILES:%=$(PROJECT_INCLUDE)/%)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/rtems$(LIB_VARIANT).o

all: $(ARCH) $(PREINSTALL_FILES) $(rtems_cpu_rel_OBJECTS) $(REL) \
    $(TMPINSTALL_FILES)

.PRECIOUS: $(REL)

EXTRA_DIST = asm.h cpu.c cpu_asm.S m68302.h m68360.h memcpy.c qsm.h rtems.S \
    sim.h

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
