#
# $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

SUBDIRS = include kern lib libc net netinet nfs rtems rtems_servers wrapup

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
