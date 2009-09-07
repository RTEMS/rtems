# $Id$

TARGET = i686-pc-cygwin

include $(top_srcdir)/mkspec.am

CYGWIN_VERS = 1.5.25-15
CYGWIN_RPMREL = 0.20090827.0%{?dist}
include ../cygwin.am

W32API_VERS = 3.13-1
W32API_RPMREL = 0.20090827.0%{?dist}
include ../w32api.am

NCURSES_VERS = 5.7-5
NCURSES_RPMREL = 0.20090827.0%{?dist}
include ../ncurses.am

EXPAT_VERS = 2.0.1-1
EXPAT_RPMREL = 0.20090903.0%{?dist}
include ../expat.am

READLINE_VERS = 5.2.14-12
READLINE_RPMREL = 0.20090827.0%{?dist}
include ../readline.am

TERMCAP_VERS = 20050421-1
TERMCAP_RPMREL = 0.20090827.0%{?dist}
include ../termcap.am

TERMINFO_VERS = 5.5_20061104-1
TERMINFO_RPMREL = 0.20090827.0%{?dist}
include ../terminfo.am

ZLIB_VERS = 1.2.3-3
ZLIB_RPMREL = 0.20090827.0%{?dist}
include ../zlib.am

GMP_VERS = 4.2.4-1
GMP_RPMREL = 0.20090827.0%{?dist}
include ../gmp.am

MPFR_VERS = 2.4.1-1
MPFR_RPMREL = 0.20090827.0%{?dist}
include ../mpfr.am

# Cygwin's binutils' versioning is bizarre
# Check bfd/configure for the real version number
BINUTILS_VERS = 2.18.50
BINUTILS_PKGVERS = 20080624-2
BINUTILS_RPMREL = 0.20090829.1
include ../binutils.am
BINUTILS_OPTS += --disable-infos

GCC_VERS = 3.4.4
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 0.20090907.0
include ../gcc.am
GCC_OPTS += --languages=c,cxx
GCC_OPTS += --disable-infos
