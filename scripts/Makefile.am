##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = . binutils gccnewlib gcc3newlib gdb rtems autotools cpukit
SUBDIRS += rtemsdoc

noinst_SCRIPTS = setup buildall buildalltar mkbspspec mkgccnewlibspec mkrpms \
    mkspec

noinst_DATA = setup.def

EXTRA_DIST = $(noinst_DATA) $(noinst_SCRIPTS)

CONFIG_STATUS_DEPENDENCIES = setup.cache
setup.cache: setup.def
	cp setup.def $@

DISTCLEANFILES = setup.cache

noinst_DATA += rtems/rtems-source.spec
rtems/rtems-source.spec: $(top_builddir)/common/common.add rtems/rtems-source.add \
	$(top_builddir)/common/clean.add
	cat $^ | sed 's,[@]rtems_version[@],$(RTEMS_VERSION),g' > $@

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
