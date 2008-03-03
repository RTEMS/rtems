# $Id$

TARGET = tic4x-rtems4.9

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.18
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 4%{?dist}
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 3.4.6
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 7%{?dist}
NEWLIB_VERS = 1.16.0
include ../gcc.am
GCC_OPTS +=

### rtems doesn't build for the tic4x
## CPUKIT_VERS = 4.7.99.1
## CPUKIT_PKGVERS = $(CPUKIT_VERS)-20070510
## CPUKIT_RPMREL = 0
## include ../cpukit.am
## CPUKIT_OPTS +=
