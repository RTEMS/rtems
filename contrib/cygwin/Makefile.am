##
## $Id$
##

RPM_SPECS_DATA = i386-cygwin-binutils.spec i386-cygwin-libs.spec \
    i386-cygwin-mingw.spec i386-cygwin-w32api.spec i386-cygwin-gcc.spec

CONFIGURE_DEPENDENCIES = setup.cache
setup.cache: setup.def
	cp setup.def $@

DISTCLEANFILES = setup.cache

noinst_SCRIPTS = setup
noinst_DATA = setup.def

EXTRA_DIST = $(noinst_SCRIPTS) $(noinst_DATA) .cvsignore
