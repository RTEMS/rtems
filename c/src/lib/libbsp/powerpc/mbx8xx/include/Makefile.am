##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h coverhd.h

BSP_H_FILES = mbx.h commproc.h 8xx_immap.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp: 
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: coverhd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/mbx.h : mbx.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/commproc.h : commproc.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bsp/8xx_immap.h : 8xx_immap.h
	$(INSTALL_DATA) $< $@

PREINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp \
	$(PROJECT_INCLUDE)/bsp.h \
	$(PROJECT_INCLUDE)/coverhd.h $(PROJECT_INCLUDE)/bsp/mbx.h \
	$(PROJECT_INCLUDE)/bsp/commproc.h $(PROJECT_INCLUDE)/bsp/8xx_immap.h

all-local: $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../../../automake/local.am
