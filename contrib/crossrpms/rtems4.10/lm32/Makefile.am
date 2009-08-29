# $Id$

TARGET = lm32-rtems4.10

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.19.51-20090721
BINUTILS_PKGVERS = 2.19.51
BINUTILS_RPMREL = 2
include ../binutils.am
BINUTILS_OPTS += --disable-infos

GCC_VERS = 4.4.1
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 7
NEWLIB_VERS = 1.17.0
NEWLIB_PKGVERS = $(NEWLIB_VERS)
NEWLIB_RPMREL = `expr 40 + $(GCC_RPMREL)`
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 6.8.50.20090629
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 2
include ../gdb.am
GDB_OPTS += --disable-infos

# NOTREADY
#CPUKIT_VERS = 4.9.99.0
#CPUKIT_PKGVERS = $(CPUKIT_VERS)-20090206-2
#CPUKIT_RPMREL = 1
#include ../cpukit.am
#CPUKIT_OPTS +=
