##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . startup clock console timer wrapup

include_HEADERS += include/coverhd.h
include_HEADERS += include/m340uart.h
include_HEADERS += include/m340timer.h
include_HEADERS += include/m68340.h
include_HEADERS += include/m68340.inc
include_HEADERS += include/m68349.inc

S_FILES = start/start.S
$(ARCH)/%$(LIB_VARIANT).$(OBJEXT): start/start.S $(ARCH)/$(dirstamp)
	$(CCASCOMPILE) -DASM -o $@ -c $<

project_lib_DATA = $(ARCH)/start$(LIB_VARIANT).$(OBJEXT)

all-local: $(PREINSTALL_FILES) $(TMPINSTALL_FILES)

TMPINSTALL_FILES =

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_INCLUDE)/m340uart.h: include/m340uart.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/m340uart.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/m340uart.h

$(PROJECT_INCLUDE)/m340timer.h: include/m340timer.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/m340timer.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/m340timer.h

$(PROJECT_INCLUDE)/m68340.h: include/m68340.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/m68340.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/m68340.h

$(PROJECT_INCLUDE)/m68340.inc: include/m68340.inc $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/m68340.inc
PREINSTALL_FILES += $(PROJECT_INCLUDE)/m68340.inc

$(PROJECT_INCLUDE)/m68349.inc: include/m68349.inc $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/m68349.inc
PREINSTALL_FILES += $(PROJECT_INCLUDE)/m68349.inc

$(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT): $(ARCH)/start$(LIB_VARIANT).$(OBJEXT) $(PROJECT_LIB)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)
TMPINSTALL_FILES += $(PROJECT_LIB)/start$(LIB_VARIANT).$(OBJEXT)

CLEANFILES = $(PREINSTALL_FILES)
CLEANFILES += $(TMPINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
