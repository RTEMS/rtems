##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if HAS_NETWORKING
NETWORK_H_FILES = wd80x3.h
endif

include_HEADERS = bsp.h coverhd.h wd80x3.h $(NETWORK_H_FILES) bspopts.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = wd80x3.h

include $(top_srcdir)/../../../../../../automake/local.am
