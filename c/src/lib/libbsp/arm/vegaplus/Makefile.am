##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . irq clock console startup timer start wrapup

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/registers.h

nodist_include_HEADERS += $(top_srcdir)/../../shared/include/coverhd.h

all-local: $(PREINSTALL_FILES)

$(PROJECT_INCLUDE)/registers.h: include/registers.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/registers.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/registers.h

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
