##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
include_rtemsdir = $(includedir)/rtems

if INLINE
include_rtems_HEADERS = extension.inl

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.inl: %.inl
	$(INSTALL_DATA) $< $@
endif

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
