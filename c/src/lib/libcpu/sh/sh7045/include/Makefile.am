##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = io_types.h sci.h sh7_pfc.h sh7_sci.h
SCORE_H_FILES = ispsh7045.h iosh7045.h

# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts

$(PROJECT_INCLUDE)/sh:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/sh/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/rtems/score/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE)/sh \
    $(H_FILES:%.h=$(PROJECT_INCLUDE)/sh/%.h) \
    $(SCORE_H_FILES:%.h=$(PROJECT_INCLUDE)/rtems/score/%.h)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = io_types.h sci.h sh7_pfc.h sh7_sci.h

include $(top_srcdir)/../../../../../../automake/local.am
