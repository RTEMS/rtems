# $Id$

TARGET = i386-rtems4.8

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.17.90
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 2%{?dist}
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.2.1
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 23%{?dist}
NEWLIB_VERS = 1.15.0
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 6.5
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 1%{?dist}
include ../gdb.am
GDB_OPTS +=

CPUKIT_VERS = 4.7.99.1
CPUKIT_PKGVERS = $(CPUKIT_VERS)-20070510
CPUKIT_RPMREL = 0
include ../cpukit.am
CPUKIT_OPTS +=
