##
## $Id$
##


MKGDBSPEC = $(SHELL) $(top_builddir)/mkgdbspec

MKGDBSPEC_DEPS = $(top_builddir)/mkgdbspec gdb.spec.in \
    $(top_builddir)/setup.cache

powerpc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems

sparc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

TEMPLATES = gdb.spec.in

RPM_SPECS_DATA = powerpc-rtems-$(GDBVERS).spec sparc-rtems-$(GDBVERS).spec

noinst_DATA = $(TEMPLATES)

EXTRA_DIST = $(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
include $(top_srcdir)/../automake/local.am
