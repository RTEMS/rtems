# $Id$

TARGET = powerpc-rtems4.7

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.17
BINUTILS_RPMREL = 3
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.1.1
GCC_RPMREL = 5
NEWLIB_VERS = 1.14.0
include ../gcc.am
GCC_OPTS += --languages=cxx

GDB_VERS = 6.5
GDB_RPMREL = 2
include ../gdb.am
