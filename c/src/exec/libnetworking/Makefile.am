##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

if HAS_POSIX
POSIX_SUBDIRS = rtems_webserver
endif

SUBDIRS = include kern lib libc net netinet nfs rtems rtems_servers \
pppd $(POSIX_SUBDIRS) wrapup

EXTRA_DIST = \
CHANGELOG \
README \
bpfilter.h \
loop.h \
netdb.h \
opt_ipfw.h \
opt_mrouting.h \
opt_tcpdebug.h \
poll.h \
resolv.h \
syslog.h

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
