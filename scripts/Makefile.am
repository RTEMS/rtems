##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

SUBDIRS = . binutils gccnewlib gdb rtems

noinst_SCRIPTS = setup buildall mkbinutilspec mkbspspec mkgccnewlibspec \
    mkgdbspec mkrpms mkspec

TEMPLATES =

noinst_DATA = setup.def $(TEMPLATES)

EXTRA_DIST = $(noinst_DATA) $(noinst_SCRIPTS)

CONFIGURE_DEPENDENCIES = setup.cache
setup.cache: setup.def
	cp setup.def $@

DISTCLEANFILES = setup.cache
include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
