##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtems_scoredir = $(includedir)/rtems/score

include_HEADERS = asm.h
include_rtems_score_HEADERS = ppc.h ppctypes.h

#
# (OPTIONAL) Add local stuff here using +=
#

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/rtems/score \
    $(include_HEADERS:%.h=$(PROJECT_INCLUDE)/%.h) \
    $(include_rtems_score_HEADERS:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h)

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
