##
## $Id$
##


ACLOCAL_AMFLAGS = -I ../../../../../../../aclocal

INCLUDES = -I$(srcdir)

# C source names
C_FILES = cpu.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.o)

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/ppc_offs.h

# Assembly source names
S_FILES = cpu_asm.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.o)

include $(RTEMS_ROOT)/make/custom/@RTEMS_BSP@.cfg
include $(top_srcdir)/../../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../../automake/lib.am

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%=$(PROJECT_INCLUDE)/%)

TMPINSTALL_FILES += $(PROJECT_RELEASE)/lib/rtems$(LIB_VARIANT).o

$(PROJECT_RELEASE)/lib/rtems$(LIB_VARIANT).o: $(ARCH)/rtems.o
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%: %
	$(INSTALL_DATA) $< $@

REL = $(ARCH)/rtems-cpu.rel

$(REL): $(C_O_FILES) $(S_O_FILES)
	$(make-rel)

all-local: $(ARCH) $(TMPINSTALL_FILES) $(REL) $(ARCH)/rtems.o

UNUSED_FILES = irq_stub.S

EXTRA_DIST = TODO rtems.S $(C_FILES) $(S_FILES) $(UNUSED_FILES)

include $(top_srcdir)/../../../../../../../automake/local.am
