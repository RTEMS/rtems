##
## $Id$
##


# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts
include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score

include_sh_HEADERS = sh/sh4uart.h
include_rtems_score_HEADERS = \
rtems/score/sh7750_regs.h \
rtems/score/ipl.h \
rtems/score/iosh7750.h \
rtems/score/ispsh7750.h \
rtems/score/sh4_regs.h

$(PROJECT_INCLUDE)/sh:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/rtems/score:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = \
    $(PROJECT_INCLUDE)/sh \
    $(include_sh_HEADERS:%=$(PROJECT_INCLUDE)/%) \
    $(PROJECT_INCLUDE)/rtems/score \
    $(include_rtems_score_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
