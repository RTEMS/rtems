## 
## $Id$
## 

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h coverhd.h page_table.h tod.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: coverhd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/page_table.h: page_table.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/tod.h: tod.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp.h \
    $(PROJECT_INCLUDE)/coverhd.h $(PROJECT_INCLUDE)/page_table.h \
    $(PROJECT_INCLUDE)/tod.h

all: $(TMPINSTALL_FILES)

EXTRA_DIST = bsp.h coverhd.h page_table.h tod.h

include $(top_srcdir)/../../../../../../automake/local.am
