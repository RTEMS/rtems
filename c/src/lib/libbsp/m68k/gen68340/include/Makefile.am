##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

include_HEADERS = bsp.h coverhd.h m340uart.h m340timer.h m68340.h m68340.inc \
    bspopts.h m68349.inc

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/%.h: %.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/%.inc: %.inc
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) \
    $(include_HEADERS:%=$(PROJECT_INCLUDE)/%)

all-local: $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
