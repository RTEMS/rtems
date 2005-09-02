##
## $Id$
##

include $(top_srcdir)/common/common.am

automake-rtems.spec.in: header.add $(top_builddir)/common/common.add automake.add
	cat $^ > $@
CLEANFILES = automake-rtems.spec.in
RPM_SPECS_DATA = @rpmprefix@automake-rtems.spec

autoconf-rtems.spec.in: header.add $(top_builddir)/common/common.add autoconf.add
	cat $^ > $@
CLEANFILES += autoconf-rtems.spec.in
RPM_SPECS_DATA += @rpmprefix@autoconf-rtems.spec

@rpmprefix@autoconf-rtems.spec: autoconf-rtems.spec.in
	$(top_builddir)/config.status --file=@rpmprefix@autoconf-rtems.spec:autoconf-rtems.spec.in

@rpmprefix@automake-rtems.spec: automake-rtems.spec.in
	$(top_builddir)/config.status --file=@rpmprefix@automake-rtems.spec:automake-rtems.spec.in

CLEANFILES += $(RPM_SPECS_DATA)

include $(top_srcdir)/../automake/local.am
