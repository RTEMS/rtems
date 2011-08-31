# $Id$

rtems_api = 4.11
TARGET = m68k-rtems4.11

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.21.1
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 1
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.6.1
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 7
NEWLIB_VERS = 1.19.0
NEWLIB_PKGVERS = $(NEWLIB_VERS)
NEWLIB_RPMREL = `expr $(GCC_RPMREL) + 10`
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 7.2
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 2
include ../gdb.am
GDB_OPTS +=

# CPUKIT_VERS = 4.10.99.0
# CPUKIT_PKGVERS = $(CPUKIT_VERS)-YYYYMMDD-R
# CPUKIT_RPMREL = 1
# include ../cpukit.am
# CPUKIT_OPTS +=
