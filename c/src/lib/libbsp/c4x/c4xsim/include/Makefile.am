##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_HEADERS = bsp.h simio.h bspopts.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h
	cp $< $@
CLEANFILES = coverhd.h

PREINSTALL_FILES = $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
