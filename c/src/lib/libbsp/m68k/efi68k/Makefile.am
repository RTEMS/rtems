##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . start startup clock console spurious timer wrapup

include $(top_srcdir)/../../bsp.am

nodist_include_HEADERS += $(top_srcdir)/../../shared/include/coverhd.h
include_HEADERS += include/efi68k.h
include_HEADERS += include/16550.h
include_HEADERS += include/DP8570A.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = times

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_INCLUDE)/efi68k.h: include/efi68k.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/efi68k.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/efi68k.h

$(PROJECT_INCLUDE)/16550.h: include/16550.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/16550.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/16550.h

$(PROJECT_INCLUDE)/DP8570A.h: include/DP8570A.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/DP8570A.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/DP8570A.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
