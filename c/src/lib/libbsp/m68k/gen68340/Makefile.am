##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . start startup clock console timer wrapup

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/coverhd.h
include_HEADERS += include/m340uart.h
include_HEADERS += include/m340timer.h
include_HEADERS += include/m68340.h
include_HEADERS += include/m68340.inc
include_HEADERS += include/m68349.inc

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = times

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

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
