##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h ../../../shared/include/coverhd.h mrm332.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: ../../../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/mrm332.h: mrm332.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp.h \
    $(PROJECT_INCLUDE)/coverhd.h $(PROJECT_INCLUDE)/mrm332.h

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = bsp.h mrm332.h

include $(top_srcdir)/../../../../../../automake/local.am
