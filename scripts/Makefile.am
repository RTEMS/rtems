##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../aclocal

SUBDIRS = . binutils gcc3newlib gdb rtems autotools cpukit
SUBDIRS += rtemsdoc

noinst_SCRIPTS = setup buildall buildalltar mkbspspec mkrpms \
    mkspec

noinst_DATA = setup.def

EXTRA_DIST = $(noinst_DATA) $(noinst_SCRIPTS)

CONFIG_STATUS_DEPENDENCIES = setup.cache
setup.cache: setup.def
	cp setup.def $@

rtems_source_spec_DEPS = common/common.add rtems/rtems-source.add \
    common/clean.add
noinst_DATA += rtems/rtems-source.spec
rtems/rtems-source.spec: $(rtems_source_spec_DEPS)
	cat $(rtems_source_spec_DEPS) | sed \
        -e 's,[@]rtems_version[@],$(RTEMS_VERSION),g' \
        -e 's,[@]RTEMS_RPM_RELEASE[@],$(RTEMS_RPM_RELEASE),g' > $@

CLEANFILES = rtems/rtems-source.spec

DISTCLEANFILES = setup.cache

include $(top_srcdir)/../automake/subdirs.am
include $(top_srcdir)/../automake/local.am
