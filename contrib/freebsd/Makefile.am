##
## $Id$
##

RPM_BUILD_ROOT = /var/tmp/%{name}-%{version}

PREFIX = /opt/i386-freebsd5.2.1
FREEBSD_VERS = 5.2.1
FREEBSD_RPMREL = 0.rc.2
BINUTILS_VERS = 2.15
BINUTILS_RPMREL = 0
GCC_VERS = 3.4.0
GCC_RPMREL = 0

noinst_DATA =
EXTRA_DIST =
DISTCLEANFILES =

noinst_DATA += i386-freebsd-libs.spec
i386-freebsd-libs.spec: i386-freebsd-libs.spec.in Makefile.am
	sed \
	-e 's,[@]FREEBSD_VERS[@],$(FREEBSD_VERS),g' \
	-e 's,[@]FREEBSD_RPMREL[@],$(FREEBSD_RPMREL),g' \
	-e 's,[@]RPM_BUILD_ROOT[@],$(RPM_BUILD_ROOT),g' \
	-e 's,[@]PREFIX[@],$(PREFIX),g' \
	< $< > $@
EXTRA_DIST += i386-freebsd-libs.spec.in
DISTCLEANFILES += i386-freebsd-libs.spec

noinst_DATA += i386-freebsd-binutils.spec
i386-freebsd-binutils.spec: i386-freebsd-binutils.spec.in Makefile.am
	sed \
	-e 's,[@]BINUTILS_VERS[@],$(BINUTILS_VERS),g' \
	-e 's,[@]BINUTILS_RPMREL[@],$(BINUTILS_RPMREL),g' \
	-e 's,[@]RPM_BUILD_ROOT[@],$(RPM_BUILD_ROOT),g' \
	-e 's,[@]PREFIX[@],$(PREFIX),g' \
	< $< > $@
EXTRA_DIST += i386-freebsd-binutils.spec.in
DISTCLEANFILES += i386-freebsd-binutils.spec

noinst_DATA += i386-freebsd-gcc.spec
i386-freebsd-gcc.spec: i386-freebsd-gcc.spec.in Makefile.am
	sed \
	-e 's,[@]GCC_VERS[@],$(GCC_VERS),g' \
	-e 's,[@]GCC_RPMREL[@],$(GCC_RPMREL),g' \
	-e 's,[@]RPM_BUILD_ROOT[@],$(RPM_BUILD_ROOT),g' \
	-e 's,[@]PREFIX[@],$(PREFIX),g' \
	< $< > $@
EXTRA_DIST += i386-freebsd-gcc.spec.in
DISTCLEANFILES += i386-freebsd-gcc.spec
