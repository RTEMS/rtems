##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = config.h directives.h extension.h fatal.h init.h io.h mptables.h \
    sptables.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

EXTRA_DIST = README

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
