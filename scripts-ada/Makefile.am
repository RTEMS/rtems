##
## $Id$
##


SUBDIRS = . gnatnewlib gdb

noinst_SCRIPTS = setup buildall mkbinutilspec mkbspspec mkgnatnewlibspec \
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
