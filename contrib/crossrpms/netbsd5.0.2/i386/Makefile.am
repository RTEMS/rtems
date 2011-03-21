# $Id$

TARGET = i386-pc-netbsdelf5.0.2

include $(top_srcdir)/mkspec.am

NETBSD_VERS = 5.0.2
NETBSD_RPMREL = 0.20100419.1
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
GCC_OPTS += --languages=c,cxx,fortran,objc
