##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_HEADERS = bsp.h coverhd.h leon.h bspopts.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
