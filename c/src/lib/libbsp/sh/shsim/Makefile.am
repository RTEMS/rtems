##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . start startup clock scitab gdbsci console wrapup

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/gdbsci.h
nodist_include_HEADERS += $(top_srcdir)/../../shared/include/coverhd.h

all-local: $(PREINSTALL_FILES)

$(PROJECT_INCLUDE)/gdbsci.h: include/gdbsci.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/gdbsci.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/gdbsci.h

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
