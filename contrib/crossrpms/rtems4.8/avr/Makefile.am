# $Id$

TARGET = avr-rtems4.8

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.17
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 2
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.0.3
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 9
NEWLIB_VERS = 1.15.0
include ../gcc.am
GCC_OPTS += --disable-infos

GDB_VERS = 6.5
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 1
include ../gdb.am
GDB_OPTS +=
