##
## $Id$
##
## FIXME: Handling of offsets.h needs to be reworked.
##
AUTOMAKE_OPTIONS = foreign 1.4

include_rtems_scoredir = $(includedir)/rtems/score

GENOFFSETS = $(PROJECT_TOPdir)/tools/cpu/hppa1.1/genoffsets

GENERIC_H_FILES = offsets.h
include_rtems_score_HEADERS = cpu.h cpu_asm.h hppa.h hppatypes.h

#
# (OPTIONAL) Add local stuff here using +=
#

offsets.h: $(GENOFFSETS) cpu.h
	$(RM) $@
	$(GENOFFSETS) > $@

CLEANFILES += offsets.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h) \
    $(GENERIC_H_FILES:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h)

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
