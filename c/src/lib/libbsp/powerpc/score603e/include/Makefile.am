##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h coverhd.h gen1.h gen2.h tod.h

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: coverhd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/gen1.h: gen1.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/gen2.h: gen2.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/tod.h: tod.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp.h \
    $(PROJECT_INCLUDE)/coverhd.h \
    $(PROJECT_INCLUDE)/gen1.h $(PROJECT_INCLUDE)/gen2.h \
    $(PROJECT_INCLUDE)/tod.h

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = bsp.h chain.h coverhd.h gen1.h gen2.h tod.h

include $(top_srcdir)/../../../../../../automake/local.am
