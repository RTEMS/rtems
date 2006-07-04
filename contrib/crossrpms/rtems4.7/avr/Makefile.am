# $Id$

TARGET = avr-rtems4.7

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.17
BINUTILS_RPMREL = @VERSION@
include $(top_srcdir)/rtems4.7/binutils.am

GCC_VERS = 4.0.3
GCC_RPMREL = @VERSION@
NEWLIB_VERS = 1.14.0
include $(top_srcdir)/rtems4.7/gcc.am

GDB_VERS = 6.4
GDB_RPMREL = @VERSION@
include $(top_srcdir)/rtems4.7/gdb.am
