##
## $Id$
##


include_HEADERS = amba.h bsp.h coverhd.h leon.h bspopts.h spacewire.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES = $(PROJECT_INCLUDE) $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
