##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

SUBDIRS = rtems

C_FILES = cpu.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.o)

S_FILES = cpu_asm.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../automake/lib.am

REL = $(ARCH)/rtems-cpu.rel

rtems_cpu_rel_OBJECTS = $(C_O_FILES) $(S_O_FILES)

$(REL): $(rtems_cpu_rel_OBJECTS)
	$(make-rel)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/rtems$(LIB_VARIANT).o

all-local: $(ARCH) $(rtems_cpu_rel_OBJECTS) $(REL) \
    $(TMPINSTALL_FILES)

.PRECIOUS: $(REL)

EXTRA_DIST = cpu.c cpu_asm.S rtems.S

include $(top_srcdir)/../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../automake/local.am
