##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

RPM_SPECS_DATA = \
    sparc-solaris-binutils.spec \
    sparc-solaris-libs.spec \
    sparc-solaris-gcc.spec

CONFIGURE_DEPENDENCIES = setup.cache
setup.cache: setup.def
	cp setup.def $@

DISTCLEANFILES = setup.cache

noinst_SCRIPTS = setup
noinst_DATA = setup.def

EXTRA_DIST = $(noinst_SCRIPTS) $(noinst_DATA) .cvsignore

