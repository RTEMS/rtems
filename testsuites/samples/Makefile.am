ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = hello capture ticker base_sp unlimited minimum fileio

if MPTESTS
## base_mp is a sample multiprocessing test
SUBDIRS += base_mp
endif

SUBDIRS += paranoia

SUBDIRS += nsecs

if CXXTESTS
SUBDIRS += iostream cdtest
endif

if NETTESTS
## loopback tests a network loopback interface
SUBDIRS += loopback
SUBDIRS += pppd
endif

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
