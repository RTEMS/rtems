##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

if ppc403
SUBDIRS = console clock timer vectors ictrl
endif
if ppc405
SUBDIRS = console tty_drv clock timer vectors ictrl
endif

include $(top_srcdir)/../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../automake/local.am
