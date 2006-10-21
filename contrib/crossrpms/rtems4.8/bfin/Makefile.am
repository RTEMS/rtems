# $Id$

TARGET = bfin-rtems4.8

include $(top_srcdir)/mkspec.am

BINUTILS_VERS = 2.17
BINUTILS_RPMREL = 1
include ../binutils.am

GCC_VERS = 4.1.1
GCC_RPMREL = 1
NEWLIB_VERS = 1.14.0
include ../gcc.am
## GCC_OPTS += --languages=cxx
## I really would like to get these options passed through but do not 
## know how yet.  So I am building with this command.
## optargs="--disable-libssp --disable-libstdcxx-pch" rpmbuild -ba XXX.spec

GDB_VERS = 6.5
GDB_RPMREL = 1
include ../gdb.am
