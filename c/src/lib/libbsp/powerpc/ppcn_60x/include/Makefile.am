##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_HEADERS = bsp.h coverhd.h tod.h nvram.h pci.h bspopts.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
