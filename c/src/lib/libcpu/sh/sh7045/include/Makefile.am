##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = null.h sci.h sh7_pfc.h sh7_sci.h

# NOTE: Unlike other CPUS, we install into a subdirectory to avoid
#       file name conflicts

$(PROJECT_INCLUDE)/sh:
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/sh/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE)/sh \
    $(H_FILES:%=$(PROJECT_INCLUDE)/sh/%)

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = io_types.h null.h sci.h sh7_pfc.h sh7_sci.h

include $(top_srcdir)/../../../../../automake/local.am
