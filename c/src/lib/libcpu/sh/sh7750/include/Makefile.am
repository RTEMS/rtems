##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts
include_shdir = $(includedir)/sh
include_rtems_scoredir = $(includedir)/rtems/score

include_sh_HEADERS = sh4uart.h
include_rtems_score_HEADERS = ispsh7750.h iosh7750.h

$(PROJECT_INCLUDE)/sh:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/sh/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES = $(PROJECT_INCLUDE)/sh \
    $(include_sh_HEADERS:%=$(PROJECT_INCLUDE)/sh/%) \
    $(include_rtems_score_HEADERS:%=$(PROJECT_INCLUDE)/rtems/score/%)

all-local: $(TMPINSTALL_FILES)

# FIXME: What are these files here?
# They should either be installed or removed
EXTRA_DIST = \
ipl.h \
sh4_regs.h \
sh4uart.h \
sh7750_regs.h

include $(top_srcdir)/../../../../../../automake/local.am
