##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_rtemsdir = $(includedir)/rtems

include_rtems_HEADERS = config.h directives.h extension.h fatal.h init.h io.h mptables.h \
    sptables.h

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

EXTRA_DIST = README

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
