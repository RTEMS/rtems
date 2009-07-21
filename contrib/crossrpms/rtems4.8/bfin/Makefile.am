# $Id$

TARGET = bfin-rtems4.8

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.18
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 5
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.2.4
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 33
NEWLIB_VERS = 1.15.0
include ../gcc.am

GDB_VERS = 6.5
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 6
include ../gdb.am
GDB_OPTS +=
