## $Id$

AUTOMAKE_OPTIONS = foreign

MKGNATNEWLIBSPEC = $(SHELL) $(top_builddir)/mkgnatnewlibspec

MKGNATNEWLIBSPEC_DEPS = \
  $(top_builddir)/mkgnatnewlibspec gnatnewlib.spec.in $(top_builddir)/setup.cache

hppa1.1-rtems-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . hppa1.1-rtems

i386-rtems-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems

i960-rtems-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . i960-rtems

m68k-rtems-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems

m68k-rtemself-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtemself

mips64orion-rtems-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . mips64orion-rtems

powerpc-rtems-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems

sh-rtems-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems

sh-rtemself-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtemself

sparc-rtems-$(GNATNEWLIBVERS).spec: $(MKGNATNEWLIBSPEC_DEPS)
	$(MKGNATNEWLIBSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

TEMPLATES = \
gnatnewlib.spec.in

RPM_SPECS_DATA = \
hppa1.1-rtems-$(GNATNEWLIBVERS).spec \
i386-rtems-$(GNATNEWLIBVERS).spec \
i960-rtems-$(GNATNEWLIBVERS).spec \
m68k-rtems-$(GNATNEWLIBVERS).spec \
m68k-rtemself-$(GNATNEWLIBVERS).spec \
mips64orion-rtems-$(GNATNEWLIBVERS).spec \
powerpc-rtems-$(GNATNEWLIBVERS).spec \
sh-rtems-$(GNATNEWLIBVERS).spec \
sh-rtemself-$(GNATNEWLIBVERS).spec \
sparc-rtems-$(GNATNEWLIBVERS).spec 

noinst_DATA = \
$(TEMPLATES)

EXTRA_DIST = \
$(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
