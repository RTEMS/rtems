#
#  $Id$
#

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

SUBDIRS= make exec lib librtems++ tests

rtems_bspdir=$(prefix)/@RTEMS_BSP@

# NOTE: The wildcard on the install should pick up everything except
#       the tests directory.  This significantly minimizes the install size.
install-data-local:
	rm -rf $(rtems_bspdir)/[bsl]*;
	$(mkinstalldirs) $(rtems_bspdir)
	cd $(PROJECT_ROOT); \
          tar cf - @RTEMS_BSP@/[bilsuM]* | (cd $(prefix); tar xpBf - );

include $(top_srcdir)/../../automake/subdirs.am
include $(top_srcdir)/../../automake/local.am
