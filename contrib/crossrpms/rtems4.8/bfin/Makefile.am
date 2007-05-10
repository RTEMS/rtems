# $Id$

TARGET = bfin-rtems4.8

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.17
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 2
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.2.0
GCC_PKGVERS = 4.2-20070221
GCC_RPMREL = 12
NEWLIB_VERS = 1.15.0
include ../gcc.am
# ATM, we don't want infos from bleeding edge GCC
GCC_OPTS += --disable-infos

GDB_VERS = 6.5
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 1
include ../gdb.am
GDB_OPTS +=

CPUKIT_VERS = 4.7.99.1
CPUKIT_PKGVERS = $(CPUKIT_VERS)-20070510
CPUKIT_RPMREL = 0
include ../cpukit.am
CPUKIT_OPTS +=
