##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h coverhd.h $(srcdir)/../../mvme162/include/mvme16x_hw.h \
    page_table.h fatal.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: coverhd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/fatal.h: fatal.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/mvme16x_hw.h: \
    $(srcdir)/../../mvme162/include/mvme16x_hw.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/page_table.h: page_table.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/fatal.h: fatal.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp.h \
    $(PROJECT_INCLUDE)/coverhd.h $(PROJECT_INCLUDE)/mvme16x_hw.h \
    $(PROJECT_INCLUDE)/page_table.h $(PROJECT_INCLUDE)/fatal.h

all: $(TMPINSTALL_FILES)

EXTRA_DIST = bsp.h coverhd.h fatal.h page_table.h

include $(top_srcdir)/../../../../../../automake/local.am
