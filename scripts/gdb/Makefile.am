##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

MKGDBSPEC = $(SHELL) $(top_builddir)/mkgdbspec

MKGDBSPEC_DEPS = $(top_builddir)/mkgdbspec gdb.spec.in \
    $(top_builddir)/setup.cache

hppa1.1-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . hppa1.1-rtems

i386-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems

i960-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . i960-rtems

m68k-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems

m68k-rtemself-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtemself

mips64orion-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . mips64orion-rtems

powerpc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems

sh-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems

sh-rtemself-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtemself

sparc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

TEMPLATES = gdb.spec.in

RPM_SPECS_DATA = hppa1.1-rtems-$(GDBVERS).spec i386-rtems-$(GDBVERS).spec \
    i960-rtems-$(GDBVERS).spec m68k-rtems-$(GDBVERS).spec \
    m68k-rtemself-$(GDBVERS).spec mips64orion-rtems-$(GDBVERS).spec \
    powerpc-rtems-$(GDBVERS).spec sh-rtems-$(GDBVERS).spec \
    sh-rtemself-$(GDBVERS).spec sparc-rtems-$(GDBVERS).spec

noinst_DATA = $(TEMPLATES)

EXTRA_DIST = $(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
include $(top_srcdir)/../automake/local.am
