##
##  $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

SUBDIRS = @cfg_subdirs@ @hst_subdirs@ @cfg_subdirs2@ wrapup tests

rtems_bspdir = $(prefix)/@RTEMS_BSP@

# NOTE: The wildcard on the install should pick up everything except
#       the tests directory.  This significantly minimizes the install size.
install-data-local:
	$(mkinstalldirs) $(rtems_bspdir)
	cd $(PROJECT_ROOT); \
          tar cf - @RTEMS_BSP@/[bilsuM]* | (cd $(DESTDIR)$(prefix); tar xpBf - );

uninstall-local:
	rm -rf $(rtems_bspdir)/[bsl]*;

include $(top_srcdir)/../../automake/subdirs.am
include $(top_srcdir)/../../automake/host.am
