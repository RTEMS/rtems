## $Id$

AUTOMAKE_OPTIONS = foreign

MKGNATNEWLIBSPEC = $(SHELL) $(top_builddir)/mkgnatnewlibspec

MKGNATNEWLIBSPEC_DEPS = \
  $(top_builddir)/mkgnatnewlibspec gnatnewlib.spec.in $(top_builddir)/setup.cache

sparc-rtems-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

TEMPLATES = \
gnatnewlib.spec.in

RPM_SPECS_DATA = \
sparc-rtems-$(GNATNEWLIBVERS).spec 

noinst_DATA = \
$(TEMPLATES)

EXTRA_DIST = \
$(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
