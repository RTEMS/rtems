# $Id$

TARGET = sparc-sun-solaris2.7

include $(top_srcdir)/mkspec.am

SOLARIS_VERS = 2.7
SOLARIS_RPMREL = 0.20090827.1
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add
include ../libs.am

BINUTILS_VERS = 2.19
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 0.20090827.1
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.3.4
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 0.20090827.1
include ../gcc.am
GCC_OPTS += --languages=c,cxx,fortran,objc
