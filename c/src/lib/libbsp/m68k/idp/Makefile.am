##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . startup clock console timer wrapup

include_HEADERS += include/leds.h
nodist_include_HEADERS += $(top_srcdir)/../../shared/include/coverhd.h

m68k_shared_S_FILES = $(top_srcdir)/../../m68k/shared/start.S
$(ARCH)/%$(LIB_VARIANT).$(OBJEXT): $(top_srcdir)/../../m68k/shared/start.S $(ARCH)/$(dirstamp)
	${CCASCOMPILE} -DASM -o $@ -c $<

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

EXTRA_DIST = include/README
EXTRA_DIST += times

TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/leds.h: include/leds.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/leds.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/leds.h

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
