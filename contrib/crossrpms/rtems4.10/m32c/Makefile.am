# $Id$

TARGET = m32c-rtems4.10

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.19
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 2%{?dist}
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.3.2
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 9%{?dist}
NEWLIB_VERS = 1.16.0
include ../gcc.am
GCC_OPTS += --disable-infos

GDB_VERS = 6.8
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 5%{?dist}
include ../gdb.am
GDB_OPTS +=

CPUKIT_VERS = 4.7.99.1
CPUKIT_PKGVERS = $(CPUKIT_VERS)-20070510
CPUKIT_RPMREL = 1%{?dist}
include ../cpukit.am
CPUKIT_OPTS +=
