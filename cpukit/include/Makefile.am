##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.5

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE):
	@$(mkinstalldirs) $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS = rtems/bspIo.h rtems/libio_.h

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(include_rtems_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../automake/local.am
