# $Id$

TARGET = m32c-rtems4.10

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.19.1
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 1
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.3.3
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 11
NEWLIB_VERS = 1.17.0
NEWLIB_PKGVERS = $(NEWLIB_VERS)
NEWLIB_RPMREL = `expr 21 + $(GCC_RPMREL)`
include ../gcc.am
GCC_OPTS += --disable-infos

GDB_VERS = 6.8.50.20090224
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 4
include ../gdb.am
GDB_OPTS += --disable-infos

CPUKIT_VERS = 4.9.99.0
CPUKIT_PKGVERS = $(CPUKIT_VERS)-20090206-2
CPUKIT_RPMREL = 1
include ../cpukit.am
CPUKIT_OPTS +=
