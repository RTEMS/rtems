ACLOCAL_AMFLAGS = -I ../aclocal

_SUBDIRS  =
_SUBDIRS += base_sp
_SUBDIRS += capture
_SUBDIRS += fileio
_SUBDIRS += hello
_SUBDIRS += minimum
_SUBDIRS += nsecs
_SUBDIRS += paranoia
_SUBDIRS += ticker
_SUBDIRS += unlimited

if MPTESTS
## base_mp is a sample multiprocessing test
_SUBDIRS += base_mp
endif

if CXXTESTS
_SUBDIRS += cdtest
_SUBDIRS += iostream
endif

if NETTESTS
## loopback tests a network loopback interface
_SUBDIRS += loopback
if HAS_SMP
else
_SUBDIRS += pppd
endif
endif

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
