##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../bsp.am

nodist_include_HEADERS += $(top_srcdir)/../shared/include/coverhd.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = build-tools do-bare-bsp-build

PREINSTALL_DIRS =

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../shared/include/coverhd.h $(PROJECT_INCLUDE)/$(dirstamp)
	$(INSTALL_DATA) $< $(PROJECT_INCLUDE)/coverhd.h
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

CLEANFILES = $(PREINSTALL_FILES)
DISTCLEANFILES = $(PREINSTALL_DIRS)

include $(top_srcdir)/../../../automake/local.am
