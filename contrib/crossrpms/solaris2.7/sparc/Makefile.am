# $Id$

TARGET = sparc-sun-solaris2.7

include $(top_srcdir)/mkspec.am

SOLARIS_VERS = 2.7
SOLARIS_RPMREL = 0.20091130.1
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add
include ../libs.am

BINUTILS_VERS = 2.22
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 0.20111122.0
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.4.6
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 0.20111123.0
include ../gcc.am
GCC_OPTS += --languages=c,cxx,fortran,objc
