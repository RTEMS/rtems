##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = . binutils gccnewlib gcc3newlib gdb rtems autotools cpukit
SUBDIRS += rtemsdoc

noinst_SCRIPTS = setup buildall buildalltar mkbspspec \
    mkgccnewlibspec mkrpms mkspec

noinst_DATA = setup.def

EXTRA_DIST = $(noinst_DATA) $(noinst_SCRIPTS)

CONFIG_STATUS_DEPENDENCIES = setup.cache
setup.cache: setup.def
	cp setup.def $@

noinst_DATA += rtems/rtems-source.spec
rtems/rtems-source.spec: common/common.add \
    rtems/rtems-source.add \
    common/clean.add
	cat $^ | sed 's,[@]rtems_version[@],$(RTEMS_VERSION),g' > $@

CLEANFILES = rtems/rtems-source.spec

DISTCLEANFILES = setup.cache
include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
