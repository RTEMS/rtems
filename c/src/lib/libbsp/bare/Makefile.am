##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../bsp.am

nodist_include_HEADERS += $(top_srcdir)/../shared/include/coverhd.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = build-tools do-bare-bsp-build

$(PROJECT_INCLUDE)/coverhd.h: $(top_srcdir)/../shared/include/coverhd.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

include $(top_srcdir)/../../../automake/local.am
