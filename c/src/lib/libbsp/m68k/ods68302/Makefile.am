##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . start startup clock console timer wrapup

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/bare.h
include_HEADERS += include/crc.h
include_HEADERS += include/debugport.h
include_HEADERS += include/m68302scc.h
include_HEADERS += include/coverhd.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = times

$(PROJECT_INCLUDE)/bare.h: include/bare.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/bare.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/bare.h

$(PROJECT_INCLUDE)/crc.h: include/crc.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/crc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/crc.h

$(PROJECT_INCLUDE)/debugport.h: include/debugport.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/debugport.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/debugport.h

$(PROJECT_INCLUDE)/m68302scc.h: include/m68302scc.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/m68302scc.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/m68302scc.h

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
