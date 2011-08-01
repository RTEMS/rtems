# $Id$

TARGET = amd64-pc-freebsd8.2

include $(top_srcdir)/mkspec.am

FREEBSD_VERS = 8.2
FREEBSD_RPMREL = 0.20110227.0
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add
include ../libs.am

BINUTILS_VERS = 2.21
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 0.20110227.0
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.5.2
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 0.20110731.0
include ../gcc.am
GCC_OPTS += --languages=cxx,fortran,objc
