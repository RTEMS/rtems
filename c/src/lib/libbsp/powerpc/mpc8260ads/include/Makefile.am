##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h coverhd.h

BSP_H_FILES =

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp: 
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: coverhd.h
	$(INSTALL_DATA) $< $@



PREINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp \
	$(PROJECT_INCLUDE)/bsp.h \
	$(PROJECT_INCLUDE)/coverhd.h

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
