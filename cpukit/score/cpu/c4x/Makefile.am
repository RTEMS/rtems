##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am
include $(top_srcdir)/../../../automake/compile.am
include $(top_srcdir)/../../../automake/lib.am

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

include_HEADERS = asm.h c4xio.h
PREINSTALL_FILES = $(PROJECT_INCLUDE) $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = \
    rtems/score/cpu.h \
    rtems/score/c4x.h \
    rtems/score/types.h \
    rtems/score/cpu_asm.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%.h=$(PROJECT_INCLUDE)/%.h)

C_FILES = cpu.c irq.c
C_O_FILES = $(C_FILES:%.c=$(ARCH)/%.$(OBJEXT))

S_FILES = cpu_asm.S
S_O_FILES = $(S_FILES:%.S=$(ARCH)/%.$(OBJEXT))

REL = $(ARCH)/rtems-cpu.rel

rtems_cpu_rel_OBJECTS = $(C_O_FILES) $(S_O_FILES)

$(REL): $(rtems_cpu_rel_OBJECTS)
	$(make-rel)

all-local: $(ARCH) $(PREINSTALL_FILES) $(rtems_cpu_rel_OBJECTS) $(REL) \
    $(TMPINSTALL_FILES)

.PRECIOUS: $(REL)

EXTRA_DIST = cpu.c irq.c cpu_asm.S

include $(top_srcdir)/../../../automake/local.am
