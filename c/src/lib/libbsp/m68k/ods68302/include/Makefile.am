##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_HEADERS = bare.h bsp.h coverhd.h crc.h debugport.h m68302scc.h bspopts.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
