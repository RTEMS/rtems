# $Id$

TARGET = i386-pc-netbsdelf5.0.1

include $(top_srcdir)/mkspec.am

NETBSD_VERS = 5.0.1
NETBSD_RPMREL = 0.20090830.0
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add
include ../libs.am

BINUTILS_VERS = 2.20.1
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 0.20100305.1
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.3.4
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 0.20090831.1
include ../gcc.am
# C only
# GCC_OPTS += --languages=cxx
