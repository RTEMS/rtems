##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = hello ticker base_sp unlimited minimum fileio

if MPTESTS
## base_mp is a sample multiprocessing test
SUBDIRS += base_mp
endif

SUBDIRS += paranoia

if CXXTESTS
SUBDIRS += iostream cdtest
endif

if NETTESTS
## loopback tests a network loopback interface
SUBDIRS += loopback
SUBDIRS += pppd
endif

DIST_SUBDIRS = hello ticker base_sp unlimited minimum fileio base_mp cdtest \
    paranoia loopback pppd

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
