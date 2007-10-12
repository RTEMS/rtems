# $Id$

TARGET = m68k-rtems4.9

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.18
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 1%{?dist}
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.2.2
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 2%{?dist}
NEWLIB_VERS = 1.15.0
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 6.7
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 2%{?dist}
include ../gdb.am
GDB_OPTS +=

CPUKIT_VERS = 4.7.99.1
CPUKIT_PKGVERS = $(CPUKIT_VERS)-20070510
CPUKIT_RPMREL = 0
include ../cpukit.am
CPUKIT_OPTS +=
