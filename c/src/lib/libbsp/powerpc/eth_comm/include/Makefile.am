##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h coverhd.h canbus.h info.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: coverhd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/canbus.h: canbus.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/info.h: info.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp.h \
    $(PROJECT_INCLUDE)/coverhd.h $(PROJECT_INCLUDE)/canbus.h \
    $(PROJECT_INCLUDE)/info.h

all: $(TMPINSTALL_FILES)

EXTRA_DIST = bsp.h canbus.h coverhd.h info.h

include $(top_srcdir)/../../../../../../automake/local.am
