##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . start startup console clock timer wrapup tools

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/simio.h
nodist_include_HEADERS += $(top_srcdir)/../../shared/include/coverhd.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = times

PREINSTALL_DIRS =

$(PROJECT_INCLUDE)/simio.h: include/simio.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/simio.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/simio.h

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
