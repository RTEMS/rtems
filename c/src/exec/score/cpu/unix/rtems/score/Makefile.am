##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtems_scoredir = $(includedir)/rtems/score

GENSIZE = $(PROJECT_TOPdir)/tools/cpu/unix/gensize

GENERIC_H_FILES = unixsize.h
include_rtems_score_HEADERS = cpu.h unixtypes.h unix.h

#
# (OPTIONAL) Add local stuff here using +=
#

unixsize.h: $(GENSIZE) cpu.h
	$(RM) $@
	$(GENSIZE) > $@

CLEANFILES = unixsize.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems/score \
    $(GENERIC_H_FILES:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h) \
    $(include_rtems_score_HEADERS:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h)

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
