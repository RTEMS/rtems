AUTOMAKE_OPTIONS = foreign

MKGDBSPEC = $(SHELL) $(top_builddir)/mkgdbspec

MKGDBSPEC_DEPS = \
$(top_builddir)/mkgdbspec gdb.spec.in $(top_builddir)/setup.cache

sparc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

TEMPLATES = \
gdb.spec.in

RPM_SPECS_DATA = \
sparc-rtems-$(GDBVERS).spec

noinst_DATA = \
$(TEMPLATES)

EXTRA_DIST = \
$(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
