##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

SUBDIRS = . binutils gccnewlib gdb rtems

noinst_SCRIPTS = setup buildall buildalltar mkbinutilspec mkbspspec \
    mkgccnewlibspec mkgdbspec mkrpms mkspec

TEMPLATES =

noinst_DATA = setup.def $(TEMPLATES)

EXTRA_DIST = $(noinst_DATA) $(noinst_SCRIPTS)

CONFIGURE_DEPENDENCIES = setup.cache
setup.cache: setup.def
	cp setup.def $@

DISTCLEANFILES = setup.cache
include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
