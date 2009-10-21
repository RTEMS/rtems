# $Id$

TARGET = i586-pc-freebsd7.0

include $(top_srcdir)/mkspec.am

FREEBSD_VERS = 7.0
FREEBSD_RPMREL = 0.20090827.1
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add
include ../libs.am

BINUTILS_VERS = 2.20
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 0.20091021.1
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.3.4
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 0.20090827.1
include ../gcc.am
GCC_OPTS += --languages=cxx,fortran,objc
