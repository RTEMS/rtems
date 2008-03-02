# $Id$

TARGET = i386-rtems4.9

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.18
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 4%{?dist}
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.3.0
GCC_PKGVERS = 4.3.0-RC-20080301
GCC_RPMREL = 10.20080301.1%{?dist}
NEWLIB_VERS = 1.16.0
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 6.7.1
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 5%{?dist}
include ../gdb.am
GDB_OPTS +=

CPUKIT_VERS = 4.7.99.1
CPUKIT_PKGVERS = $(CPUKIT_VERS)-20070510
CPUKIT_RPMREL = 0
include ../cpukit.am
CPUKIT_OPTS +=
