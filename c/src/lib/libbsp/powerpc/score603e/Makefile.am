##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . clock console startup start timer tod PCI_bus vectors \
    @exceptions@ wrapup

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/coverhd.h include/gen2.h
include_HEADERS += include/tod.h

all-local: $(PREINSTALL_FILES)

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_INCLUDE)/gen2.h: include/gen2.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/gen2.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/gen2.h

$(PROJECT_INCLUDE)/tod.h: include/tod.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/tod.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tod.h

CLEANFILES = $(PREINSTALL_FILES)

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
