##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h ../../../shared/include/coverhd.h dmv170.h \
    ../../../shared/tod.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: ../../../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/dmv170.h: dmv170.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/tod.h: ../../../shared/tod.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp.h \
    $(PROJECT_INCLUDE)/coverhd.h $(PROJECT_INCLUDE)/dmv170.h \
    $(PROJECT_INCLUDE)/tod.h

all: $(TMPINSTALL_FILES)

EXTRA_DIST = bsp.h dmv170.h

include $(top_srcdir)/../../../../../../automake/local.am
