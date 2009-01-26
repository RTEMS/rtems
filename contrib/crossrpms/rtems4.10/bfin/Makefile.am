# $Id$

TARGET = bfin-rtems4.10

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.19
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 2%{?dist}
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.3.3
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 21%{?dist}
NEWLIB_VERS = 1.17.0
NEWLIB_PKGVERS = $(NEWLIB_VERS)
NEWLIB_RPMREL = $(GCC_RPMREL)
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 6.5
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 4%{?dist}
include ../gdb.am
GDB_OPTS +=

### bfin's GCC is too broken to be usable
## CPUKIT_VERS = 4.7.99.1
## CPUKIT_PKGVERS = $(CPUKIT_VERS)-20070510
## CPUKIT_RPMREL = 1%{?dist}
## include ../cpukit.am
## CPUKIT_OPTS +=
