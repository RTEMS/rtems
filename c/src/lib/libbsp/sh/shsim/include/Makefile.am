##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h gdbsci.h ../../../shared/include/coverhd.h

$(PROJECT_INCLUDE):
	$(mkinstalldirs) $@

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/bspopts.h: bspopts.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/sh/gdbsci.h: gdbsci.h
	$(INSTALL_DATA) $< $@ 

$(PROJECT_INCLUDE)/coverhd.h: ../../../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp.h \
    $(PROJECT_INCLUDE)/coverhd.h $(PROJECT_INCLUDE)/sh/gdbsci.h \
    $(PROJECT_INCLUDE)/bspopts.h

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = bsp.h

include $(top_srcdir)/../../../../../../automake/local.am
