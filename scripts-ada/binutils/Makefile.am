##
## $Id$
##


MKBINUTILSSPEC = $(SHELL) $(top_builddir)/mkbinutilspec

MKBINUTILSSPEC_DEPS = $(top_builddir)/mkbinutilspec binutils.spec.in \
    $(top_builddir)/setup.cache

sparc-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

TEMPLATES = binutils.spec.in

RPM_SPECS_DATA = sparc-rtems-$(BINUTILSVERS).spec

noinst_DATA = $(TEMPLATES)

EXTRA_DIST = $(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
include $(top_srcdir)/../automake/local.am
