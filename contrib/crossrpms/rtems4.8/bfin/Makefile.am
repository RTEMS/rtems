# $Id$

TARGET = bfin-rtems4.8

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.17
BINUTILS_RPMREL = 1
include ../binutils.am

GCC_VERS = 4.2.0
GCC_PKGVERS = 4.2-20061031
GCC_RPMREL = 3
NEWLIB_VERS = 1.14.0
include ../gcc.am
# ATM, we don't want info's from bleeding edge GCC
GCC_OPTS += --disable-infos

GDB_VERS = 6.5
GDB_RPMREL = 1
include ../gdb.am
