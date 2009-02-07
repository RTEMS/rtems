# $Id$

TARGET = i386-rtems4.9

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.19
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 2%{?dist}
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.3.2
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 20%{?dist}
NEWLIB_VERS = 1.16.0
NEWLIB_PKGVERS = $(NEWLIB_VERS)
NEWLIB_RPMREL = $(GCC_RPMREL)
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 6.8
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 5%{?dist}
include ../gdb.am
GDB_OPTS +=
