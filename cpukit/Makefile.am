##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../aclocal

include $(top_srcdir)/../../../automake/multilib.am

SUBDIRS = . include score rtems sapi @subdirs@ wrapup

noinst_DATA = preinstall-stamp

preinstall-stamp:
	$(MAKE) $(AM_MAKEFLAGS) preinstall
	touch preinstall-stamp
DISTCLEANFILES = preinstall-stamp

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
