##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

# consolex is a predecessor of the termios and libchip concepts.
# It is no longer being compiled into the source.
# The MVME162 has 2 z8530's to yield four serial ports.
# The application can access these by referencing the consolex driver
# with "CONSOLEX_DRIVER_TABLE_ENTRY" in the driver table definition,
# in place of "CONSOLE_DRIVER_TABLE_ENTRY". See consolex/cTest.c for
# an example.

# wrapup is the one that actually builds and installs the library
# from the individual .rel files built in other directories
SUBDIRS = . start startup clock console timer tod network wrapup tools

include $(top_srcdir)/../../bsp.am

include_HEADERS += include/coverhd.h
include_HEADERS += include/page_table.h
include_HEADERS += include/tod.h

nodist_include_HEADERS += $(top_srcdir)/../shared/mvme/include/mvme16x_hw.h

all-local: $(PREINSTALL_FILES)

EXTRA_DIST = times

$(PROJECT_INCLUDE)/coverhd.h: include/coverhd.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/coverhd.h

$(PROJECT_INCLUDE)/page_table.h: include/page_table.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/page_table.h

$(PROJECT_INCLUDE)/tod.h: include/tod.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/tod.h

$(PROJECT_INCLUDE)/mvme16x_hw.h: \
    $(top_srcdir)/../shared/mvme/include/mvme16x_hw.h
	$(INSTALL_DATA) $< $@
PREINSTALL_FILES += $(PROJECT_INCLUDE)/mvme16x_hw.h

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
