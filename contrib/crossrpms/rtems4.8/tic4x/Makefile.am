# $Id$

TARGET = tic4x-rtems4.8

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.17
BINUTILS_RPMREL = 1
include ../binutils.am

GCC_VERS = 3.4.6
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 2
NEWLIB_VERS = 1.14.0
include ../gcc.am
GCC_OPTS +=

### GDB-6.4 doesn't support the tic4x
## GDB_VERS = 6.4
## include ../gdb.am
