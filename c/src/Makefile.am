##
##  $Id$
##

ACLOCAL_AMFLAGS = -I ../../aclocal

SUBDIRS = @exec@ @subdirs@

# NOTE: The wildcard on the install should pick up everything except
#       the tests directory.  This significantly minimizes the install size.

include $(top_srcdir)/../../automake/subdirs.am
include $(top_srcdir)/../../automake/host.am
