##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . start startup clock console timer network wrapup

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/coverhd.h
include_HEADERS += include/m302_int.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = times

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_INCLUDE)/m302_int.h: include/m302_int.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/m302_int.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/m302_int.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
