#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

if HAS_NETWORKING
## We only build the networking and the rpc library 
## if HAS_NETWORKING was defined
LIBNETWORKING = libnetworking 
LIBRPC = librpc

if HAS_RDBG
LIBRDBG = librdbg
endif

endif

if HAS_LIBCHIP
LIBCHIP = libchip
endif

SUBDIRS = start include libmisc libc $(LIBNETWORKING) libcpu libbsp \
    $(LIBRPC) $(LIBRDBG) $(LIBCHIP) $(LIBHWAPI) wrapup

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
