# $Id$

TARGET = mipstx39-rtems4.9

include $(top_srcdir)/mkspec.am

# We only want a gdb with a tx39 simulator in it.  We share the other tools
GDB_VERS = 6.8
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 1%{?dist}
include ../gdb.am
GDB_OPTS +=
