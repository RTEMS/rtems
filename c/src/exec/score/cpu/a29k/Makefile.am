##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

SUBDIRS = rtems

C_FILES = cpu.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

H_FILES = amd.ah asm.h cpu_asm.h pswmacro.ah register.ah

S_FILES = cpu_asm.S sig.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

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

all-local: $(ARCH) $(PREINSTALL_FILES) $(rtems_cpu_rel_OBJECTS) $(REL) \
    $(TMPINSTALL_FILES)

.PRECIOUS: $(REL)

EXTRA_DIST = amd.ah asm.h cpu.c cpu_asm.S pswmacro.ah register.ah rtems.c \
    sig.S

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
