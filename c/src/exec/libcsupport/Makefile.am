##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

if HAS_LIBCHIP
LIBCHIP = libchip
endif

SUBDIRS = start include libmisc libc libcpu libbsp \
    $(LIBCHIP) $(LIBHWAPI)

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
