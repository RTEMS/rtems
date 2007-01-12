# $Id$

TARGET = i386-rtems4.8

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.17
BINUTILS_RPMREL = 1
include ../binutils.am

GCC_VERS = 4.1.1
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 4
NEWLIB_VERS = 1.15.0
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 6.5
GDB_RPMREL = 1
include ../gdb.am
