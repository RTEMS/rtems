# $Id$

TARGET = mipstx39-rtems4.10

include $(top_srcdir)/mkspec.am

# We only want a gdb with a tx39 simulator in it.  We share the other tools
GDB_VERS = 7.2
GDB_PKGVERS = $(GDB_VERS)
GDB_RPMREL = 2
include ../gdb.am
GDB_OPTS +=
