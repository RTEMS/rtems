##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = debug.h system.h

noinst_HEADERS = $(H_FILES)

PREINSTALL_FILES += $(PROJECT_INCLUDE)/rtems \
    $(noinst_HEADERS:%=$(PROJECT_INCLUDE)/rtems/%)

$(PROJECT_INCLUDE)/rtems:
	@$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/rtems/%.h: %.h
	$(INSTALL_DATA) $< $@

all-am: $(PREINSTALL_FILES)

SUBDIRS = score

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
