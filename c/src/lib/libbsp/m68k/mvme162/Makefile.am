##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../../../aclocal

# consolex is a predecessor of the termios and libchip concepts.
# It is no longer being compiled into the source.
# The MVME162 has 2 z8530's to yield four serial ports.
# The application can access these by referencing the consolex driver
# with "CONSOLEX_DRIVER_TABLE_ENTRY" in the driver table definition,
# in place of "CONSOLE_DRIVER_TABLE_ENTRY".  See consolex/cTest.c for
# an example.

# wrapup is the one that actually builds and installs the library
#  from the individual .rel files built in other directories
SUBDIRS = . include start startup clock console timer tod network wrapup tools

include $(top_srcdir)/../../bsp.am

EXTRA_DIST = bsp_specs times

include $(top_srcdir)/../../../../automake/subdirs.am
include $(top_srcdir)/../../../../automake/local.am
