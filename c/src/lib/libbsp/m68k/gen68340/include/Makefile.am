##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h coverhd.h m340uart.h m340timer.h m68340.h m68340.inc \
    m68349.inc

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: coverhd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/m340uart.h: m340uart.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/m340timer.h: m340timer.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/m68340.h: m68340.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/m68340.inc: m68340.inc
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/m68349.inc: m68349.inc
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp.h \
    $(PROJECT_INCLUDE)/coverhd.h $(PROJECT_INCLUDE)/m340uart.h \
    $(PROJECT_INCLUDE)/m340timer.h $(PROJECT_INCLUDE)/m68340.h \
    $(PROJECT_INCLUDE)/m68340.inc $(PROJECT_INCLUDE)/m68349.inc

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = bsp.h coverhd.h m340timer.h m340uart.h m68340.h m68340.inc \
    m68349.inc

include $(top_srcdir)/../../../../../../automake/local.am
