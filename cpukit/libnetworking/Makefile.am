##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I ../../../aclocal

SUBDIRS = include arpa kern machine sys vm lib libc net netinet nfs rtems \
    rtems_servers pppd modem rtems_webserver wrapup

EXTRA_DIST = CHANGELOG bpfilter.h loop.h netdb.h opt_ipfw.h opt_mrouting.h \
    opt_tcpdebug.h poll.h resolv.h syslog.h memory.h

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
