## $Id$

AUTOMAKE_OPTIONS = foreign

MKGCCNEWLIBSPEC = $(SHELL) $(top_builddir)/mkgccnewlibspec

MKGCCNEWLIBSPEC_DEPS = \
  $(top_builddir)/mkgccnewlibspec gccnewlib.spec.in $(top_builddir)/setup.cache

i386-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems

sh-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems

powerpc-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems

sparc-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

m68k-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems

mips64orion-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . mips64orion-rtems

i960-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . i960-rtems

hppa1.1-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . hppa1.1-rtems

TEMPLATES = \
gccnewlib.spec.in

RPM_SPECS_DATA = \
i386-rtems-$(GCCNEWLIBVERS).spec \
sh-rtems-$(GCCNEWLIBVERS).spec \
powerpc-rtems-$(GCCNEWLIBVERS).spec \
sparc-rtems-$(GCCNEWLIBVERS).spec \
m68k-rtems-$(GCCNEWLIBVERS).spec \
mips64orion-rtems-$(GCCNEWLIBVERS).spec \
i960-rtems-$(GCCNEWLIBVERS).spec \
hppa1.1-rtems-$(GCCNEWLIBVERS).spec

noinst_DATA = \
$(TEMPLATES)

EXTRA_DIST = \
$(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
