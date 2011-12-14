# $Id$

TARGET = i686-pc-cygwin

include $(top_srcdir)/mkspec.am

CYGWIN_VERS = 1.7.9-1
CYGWIN_RPMREL = 0.20110427.0%{?dist}
include ../cygwin.am

CLOOG_PPL_VERS = 0.15.7-1
CLOOG_PPL_RPMREL = 0.20100204.0%{?dist}
include ../cloog_ppl.am

W32API_VERS = 3.17-2
W32API_RPMREL = 0.20110427.0%{?dist}
include ../w32api.am

NCURSES_VERS = 5.7-18
NCURSES_RPMREL = 0.20100204.0%{?dist}
include ../ncurses.am

EXPAT_VERS = 2.0.1-1
EXPAT_RPMREL = 0.20090903.0%{?dist}
include ../expat.am

READLINE_VERS = 6.1.2-2
READLINE_RPMREL = 0.20111214.0%{?dist}
include ../readline.am

TERMCAP_VERS = 20050421-1
TERMCAP_RPMREL = 0.20090827.0%{?dist}
include ../termcap.am

TERMINFO_VERS = 5.7_20091114-14
TERMINFO_RPMREL = 0.20100726.0%{?dist}
include ../terminfo.am

ZLIB_VERS = 1.2.5-1
ZLIB_RPMREL = 0.20110427.0%{?dist}
include ../zlib.am

GMP_VERS = 4.3.1-3
GMP_RPMREL = 0.20100204.0%{?dist}
include ../gmp.am

MPC_VERS = 0.8-1
MPC_RPMREL = 0.20100322.0%{?dist}
include ../mpc.am

MPFR_VERS = 2.4.1-4
MPFR_RPMREL = 0.20100204.0%{?dist}
include ../mpfr.am

# Cygwin's binutils' versioning is bizarre
# Check bfd/configure for the real version number
BINUTILS_VERS = 2.20.51-2
BINUTILS_PKGVERS = 2.20.51-2
BINUTILS_RPMREL = 0.20110427.0
include ../binutils.am
BINUTILS_OPTS += --disable-infos

GCC_VERS = 3.4.4
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 0.20110310.0
include ../gcc.am
GCC_OPTS += --languages=c,cxx
GCC_OPTS += --disable-infos
