##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

GENOFFSETS = $(PROJECT_TOPdir)/tools/cpu/hppa1.1/genoffsets

GENERIC_H_FILES = offsets.h
STATIC_H_FILES = cpu.h cpu_asm.h hppa.h hppatypes.h
noinst_HEADERS = $(STATIC_H_FILES)

#
# (OPTIONAL) Add local stuff here using +=
#

offsets.h: $(GENOFFSETS) cpu.h
	$(RM) $@
	$(GENOFFSETS) > $@

CLEANFILES = offsets.h

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems/score \
    $(STATIC_H_FILES:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h) \
    $(GENERIC_H_FILES:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h)

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
