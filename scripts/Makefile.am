AUTOMAKE_OPTIONS = foreign

SUBDIRS = . binutils gccnewlib

noinst_SCRIPTS = \
setup \
buildall \
mkbinutils_subpackage_version \
mkbinutilspec \
mkbspspec \
mkgccnewlibspec \
mkgdbspec \
mkrpms \
mkspec \
mktoolspec

TEMPLATES = \
gccnewlib.spec.in \
gdb.spec.in \
rtems.spec.in \
toolchain.spec.in

noinst_DATA = \
setup.def $(TEMPLATES)

EXTRA_DIST = \
$(noinst_DATA) \
$(noinst_SCRIPTS)

CONFIGURE_DEPENDENCIES = setup.cache
setup.cache: setup.def
	cp setup.def $@

DISTCLEANFILES = setup.cache
