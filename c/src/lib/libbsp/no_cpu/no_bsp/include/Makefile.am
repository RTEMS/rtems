##
## $Id$
##


include_HEADERS = bsp.h coverhd.h bspopts.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

coverhd.h: $(top_srcdir)../../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@
CLEANFILES = coverhd.h

TMPINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADER:%=S(PROJECT_INCLUDE)/%)

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
