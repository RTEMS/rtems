# $Id$

TARGET = bfin-rtems4.9

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.18
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 5%{?dist}
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.3.2
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 15%{?dist}
NEWLIB_VERS = 1.16.0
include ../gcc.am
GCC_OPTS +=

GDB_VERS = 6.5
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 4%{?dist}
include ../gdb.am
GDB_OPTS +=
