## $Id$

AUTOMAKE_OPTIONS = foreign

MKGCCNEWLIBSPEC = $(SHELL) $(top_builddir)/mkgccnewlibspec

MKGCCNEWLIBSPEC_DEPS = \
  $(top_builddir)/mkgccnewlibspec gccnewlib.spec.in $(top_builddir)/setup.cache

hppa1.1-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . hppa1.1-rtems

i386-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems

i960-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . i960-rtems

m68k-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems

m68k-rtemself-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtemself

mips64orion-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . mips64orion-rtems

powerpc-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems

sh-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems

sh-rtemself-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtemself

sparc-rtems-$(GCCNEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

TEMPLATES = \
gccnewlib.spec.in

RPM_SPECS_DATA = \
hppa1.1-rtems-$(GCCNEWLIBVERS).spec \
i386-rtems-$(GCCNEWLIBVERS).spec \
i960-rtems-$(GCCNEWLIBVERS).spec \
m68k-rtems-$(GCCNEWLIBVERS).spec \
m68k-rtemself-$(GCCNEWLIBVERS).spec \
mips64orion-rtems-$(GCCNEWLIBVERS).spec \
powerpc-rtems-$(GCCNEWLIBVERS).spec \
sh-rtems-$(GCCNEWLIBVERS).spec \
sh-rtemself-$(GCCNEWLIBVERS).spec \
sparc-rtems-$(GCCNEWLIBVERS).spec 

noinst_DATA = \
$(TEMPLATES)

EXTRA_DIST = \
$(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
