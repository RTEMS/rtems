##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/macros

noinst_SCRIPTS = coff2bin elf2exe

SUBDIRS = ts_1325_ada network_ada debug_ada debug_c dos_sup

EXTRA_DIST = $(noinst_SCRIPTS)

include $(top_srcdir)/../../../../../../../automake/subdirs.am
include $(top_srcdir)/../../../../../../../automake/host.am
