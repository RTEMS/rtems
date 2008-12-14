# $Id$

TARGET = sparc-rtems4.9

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.19
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 1%{?dist}
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.3.2
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 16%{?dist}
NEWLIB_VERS = 1.16.0
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 6.8
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 4%{?dist}
include ../gdb.am
GDB_OPTS +=
