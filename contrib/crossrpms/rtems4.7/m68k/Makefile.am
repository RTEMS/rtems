# $Id$

TARGET = m68k-rtems4.7

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.17
BINUTILS_RPMREL = 4
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.1.1
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 7
NEWLIB_VERS = 1.15.0
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 6.5
GDB_RPMREL = 2
include ../gdb.am
GDB_OPTS +=
