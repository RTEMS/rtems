##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . clock console startup start timer scv64 tod sonic @exceptions@ \
    wrapup

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/dmv170.h
nodist_include_HEADERS += $(top_srcdir)/../../shared/tod.h
nodist_include_HEADERS += $(top_srcdir)/../../shared/include/coverhd.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = QUIRKS README.net STATUS cable.doc times

$(PROJECT_INCLUDE)/dmv170.h: include/dmv170.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/dmv170.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/dmv170.h

$(PROJECT_INCLUDE)/tod.h: $(top_srcdir)/../../shared/tod.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tod.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tod.h

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
