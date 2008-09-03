# $Id$

TARGET = sparc-rtems4.8

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.18
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 4%{?dist}
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.2.4
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 31%{?dist}
NEWLIB_VERS = 1.15.0
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 6.8
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 2%{?dist}
include ../gdb.am
GDB_OPTS +=
