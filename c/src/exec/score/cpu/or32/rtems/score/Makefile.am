##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtems_scoredir = $(includedir)/rtems/score
include_rtems_score_HEADERS = cpu.h or32.h cpu_asm.h or32types.h

#
# (OPTIONAL) Add local stuff here using +=
#

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h)

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
