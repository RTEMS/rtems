##
## $Id$
##


include_HEADERS = pcibios.h

C_FILES = pcibios.c

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@
$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = pcibios.c

include $(top_srcdir)/../../../../../automake/local.am
