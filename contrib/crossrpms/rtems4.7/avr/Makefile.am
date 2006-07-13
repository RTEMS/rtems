# $Id$

TARGET = avr-rtems4.7

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.17
BINUTILS_RPMREL = @VERSION@
include ../binutils.am

GCC_VERS = 4.0.3
GCC_RPMREL = @VERSION@
NEWLIB_VERS = 1.14.0
include ../gcc.am
GCC_OPTS +=

GDB_VERS = 6.5
GDB_RPMREL = @VERSION@
include ../gdb.am
