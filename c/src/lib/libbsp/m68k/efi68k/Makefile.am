##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . startup clock console spurious timer wrapup

nodist_include_HEADERS += $(top_srcdir)/../../shared/include/coverhd.h
include_HEADERS += include/efi68k.h
include_HEADERS += include/16550.h
include_HEADERS += include/DP8570A.h

EXTRA_DIST = times

C_FILES = start/start.C
$(ARCH)/%$(LIB_VARIANT).$(OBJEXT): start/start.c $(ARCH)/$(dirstamp)
	$(COMPILE) -o $@ -c $<

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

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

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
