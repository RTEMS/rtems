##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

H_FILES = bsp.h ../../../shared/include/coverhd.h efi68k.h 16550.h DP8570A.h

$(PROJECT_INCLUDE)/bsp.h: bsp.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/coverhd.h: ../../../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/efi68k.h: efi68k.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/16550.h: 16550.h
	$(INSTALL_DATA) $< $@

$(PROJECT_INCLUDE)/DP8570A.h: DP8570A.h
	$(INSTALL_DATA) $< $@

TMPINSTALL_FILES += $(PROJECT_INCLUDE) $(PROJECT_INCLUDE)/bsp.h \
    $(PROJECT_INCLUDE)/coverhd.h $(PROJECT_INCLUDE)/efi68k.h \
    $(PROJECT_INCLUDE)/16550.h $(PROJECT_INCLUDE)/DP8570A.h

all-local: $(TMPINSTALL_FILES)

EXTRA_DIST = 16550.h DP8570A.h bsp.h efi68k.h

include $(top_srcdir)/../../../../../../automake/local.am
