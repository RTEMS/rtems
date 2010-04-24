# $Id$

TARGET = i586-pc-freebsd7.3

include $(top_srcdir)/mkspec.am

FREEBSD_VERS = 7.3
FREEBSD_RPMREL = 0.20100416.1
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add
include ../libs.am

BINUTILS_VERS = 2.20.1
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 0.20100316.1
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.5.0
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 0.20100424.0
include ../gcc.am
GCC_OPTS += --languages=cxx,fortran,objc
