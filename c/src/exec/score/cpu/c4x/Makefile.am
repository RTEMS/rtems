##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

SUBDIRS = rtems

C_FILES = cpu.c irq.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

include_HEADERS = asm.h c4xio.h

S_FILES = cpu_asm.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

REL = $(ARCH)/rtems-cpu.rel

include $(top_srcdir)/../../../../../../automake/multilib.am
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

rtems_cpu_rel_OBJECTS = $(C_O_FILES) $(S_O_FILES)

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_RELEASE)/lib$(MULTISUBDIR)/rtems$(LIB_VARIANT).o: $(ARCH)/rtems.o
	$(INSTALL_DATA) $< $@

$(REL): $(rtems_cpu_rel_OBJECTS)
	$(make-rel)

PREINSTALL_FILES = $(PROJECT_INCLUDE) $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib$(MULTISUBDIR)/rtems$(LIB_VARIANT).o

all-local: $(ARCH) $(PREINSTALL_FILES) $(rtems_cpu_rel_OBJECTS) $(REL) \
    $(TMPINSTALL_FILES)

.PRECIOUS: $(REL)

EXTRA_DIST = cpu.c irq.c cpu_asm.S rtems.c

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
