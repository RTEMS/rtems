##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories

SUBDIRS = . start startup clock console fatal timer network wrapup

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/coverhd.h
include_HEADERS += include/page_table.h
include_HEADERS += include/fatal.h

nodist_include_HEADERS += $(top_srcdir)/../shared/mvme/mvme16x_hw.h

all: $(PREINSTALL_FILES)

EXTRA_DIST = times

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_INCLUDE)/page_table.h: include/page_table.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/page_table.h

$(PROJECT_INCLUDE)/fatal.h: include/fatal.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/fatal.h

$(PROJECT_INCLUDE)/mvme16x_hw.h: $(top_srcdir)/../shared/mvme/mvme16x_hw.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mvme16x_hw.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
