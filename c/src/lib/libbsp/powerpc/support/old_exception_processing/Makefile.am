##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../../aclocal

# C source names
C_FILES = cpu.c
C_O_FILES = $(C_FILES:%.c=${ARCH}/%.$(OBJEXT))

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = rtems/score/ppc_offs.h

# Assembly source names
S_FILES = cpu_asm.S
S_O_FILES = $(S_FILES:%.S=${ARCH}/%.$(OBJEXT))

include $(top_srcdir)/../../../../../../../automake/compile.am
include $(top_srcdir)/../../../../../../../automake/lib.am

AM_CPPFLAGS += -I$(srcdir)

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%=$(PROJECT_INCLUDE)/%)

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%: %
	$(INSTALL_DATA) $< $@

REL = $(ARCH)/rtems-cpu.rel

$(REL): $(C_O_FILES) $(S_O_FILES)
	$(make-rel)

all-local: $(ARCH) $(TMPINSTALL_FILES) $(REL)

UNUSED_FILES = irq_stub.S

EXTRA_DIST = TODO $(C_FILES) $(S_FILES) $(UNUSED_FILES)

include $(top_srcdir)/../../../../../../../automake/local.am
