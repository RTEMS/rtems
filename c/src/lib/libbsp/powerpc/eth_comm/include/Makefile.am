##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h coverhd.h canbus.h info.h

BSP_H_FILES = commproc.h 8xx_immap.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp: 
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: coverhd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/canbus.h: canbus.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/info.h: info.h
	$(INSTALL_DATA) $< $@

 $(PROJECT_INCLUDE)/bsp/commproc.h : commproc.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/8xx_immap.h : 8xx_immap.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp \
	$(PROJECT_INCLUDE)/bsp.h $(PROJECT_INCLUDE)/info.h \
	$(PROJECT_INCLUDE)/coverhd.h $(PROJECT_INCLUDE)/canbus.h \
	$(PROJECT_INCLUDE)/bsp/commproc.h $(PROJECT_INCLUDE)/bsp/8xx_immap.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = bsp.h canbus.h coverhd.h info.h

include $(top_srcdir)/../../../../../../automake/local.am
