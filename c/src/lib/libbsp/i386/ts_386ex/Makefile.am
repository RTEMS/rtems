##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . tools start startup clock console timer network wrapup

include $(top_srcdir)/../../bsp.am

if HAS_NETWORKING
include_HEADERS += include/wd80x3.h
endif

include_HEADERS += include/coverhd.h

all-local: $(PREINSTALL_FILES)

if HAS_NETWORKING
$(PROJECT_INCLUDE)/wd80x3.h: include/wd80x3.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/wd80x3.h
endif

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
