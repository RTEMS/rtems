##
##  $Id$
##

ACLOCAL_AMFLAGS = -I ../../aclocal

SUBDIRS = @subdirs@

# NOTE: The wildcard on the install should pick up everything except
#       the tests directory.  This significantly minimizes the install size.
install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(exec_prefix)/@RTEMS_BSP@
	cd $(PROJECT_ROOT)/@RTEMS_BSP@; \
          tar cf - [bilsuM]* | \
	  (cd $(DESTDIR)$(exec_prefix)/@RTEMS_BSP@; tar xpBf - );

uninstall-local:
	rm -rf $(DESTDIR)$(exec_prefix)/@RTEMS_BSP@/[bsl]*;

include $(top_srcdir)/../../automake/subdirs.am
include $(top_srcdir)/../../automake/host.am
