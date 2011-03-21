# $Id$

TARGET = i586-pc-freebsd6.4

include $(top_srcdir)/mkspec.am

FREEBSD_VERS = 6.4
FREEBSD_RPMREL = 0.20090827.1
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add
include ../libs.am

BINUTILS_VERS = 2.21
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 0.20101211.0
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.5.2
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 0.20110321.1
include ../gcc.am
GCC_OPTS += --languages=cxx,fortran,objc
