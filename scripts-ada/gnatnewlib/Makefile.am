## $Id$


MKGNATNEWLIBSPEC = $(SHELL) $(top_builddir)/mkgnatnewlibspec

MKGNATNEWLIBSPEC_DEPS = $(top_builddir)/mkgnatnewlibspec gnatnewlib.spec.in \
    $(top_builddir)/setup.cache

i386-rtemscoff-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtemscoff

powerpc-rtems-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems

sparc-rtems-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

TEMPLATES = gnatnewlib.spec.in

RPM_SPECS_DATA = \
    i386-rtemscoff-$(GNATNEWLIBVERS).spec \
    powerpc-rtems-$(GNATNEWLIBVERS).spec \
    sparc-rtems-$(GNATNEWLIBVERS).spec

noinst_DATA = $(TEMPLATES)

EXTRA_DIST = $(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
include $(top_srcdir)/../automake/local.am
