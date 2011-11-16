# $Id$

TARGET = sh-rtems4.10

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.20.1
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 3
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.4.6
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 4
NEWLIB_VERS = 1.18.0
NEWLIB_PKGVERS = $(NEWLIB_VERS)
NEWLIB_RPMREL = `expr $(GCC_RPMREL) + 23`
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 7.2
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 2
include ../gdb.am
GDB_OPTS +=
