##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bare.h bsp.h coverhd.h crc.h debugport.h m68302scc.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bare.h: bare.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: coverhd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/crc.h: crc.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/debugport.h: debugport.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/m68302scc.h: m68302scc.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bare.h \
    $(PROJECT_INCLUDE)/bsp.h $(PROJECT_INCLUDE)/coverhd.h \
    $(PROJECT_INCLUDE)/crc.h $(PROJECT_INCLUDE)/debugport.h \
    $(PROJECT_INCLUDE)/m68302scc.h

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = bare.h bsp.h coverhd.h crc.h debugport.h m68302scc.h

include $(top_srcdir)/../../../../../../automake/local.am
