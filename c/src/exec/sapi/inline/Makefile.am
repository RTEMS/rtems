##
## $Id$
##


include_rtemsdir = $(includedir)/rtems

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%: %
	$(INSTALL_DATA) $< $@

if INLINE
include_rtems_HEADERS = rtems/extension.inl

PREINSTALL_FILES = $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/%)
endif

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/automake/local.am
