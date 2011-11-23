# $Id$

TARGET = i386-pc-netbsdelf5.1

include $(top_srcdir)/mkspec.am

NETBSD_VERS = 5.1
NETBSD_RPMREL = 0.20110320.0
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add
include ../libs.am

BINUTILS_VERS = 2.22
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 0.20111122.0
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.5.2
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 0.20111116.0
include ../gcc.am
GCC_OPTS += --languages=c,cxx,fortran,objc
