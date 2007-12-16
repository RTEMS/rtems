# $Id$

TARGET = mipstx39-rtems4.8

include $(top_srcdir)/mkspec.am

# We only want a gdb with a tx39 simulator in it.  We share the other tools
GDB_VERS = 6.6
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 11%{?dist}
include ../gdb.am
GDB_OPTS +=
