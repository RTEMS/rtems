## $Id$

AUTOMAKE_OPTIONS = foreign

MKBINUTILSSPEC = $(SHELL) $(top_builddir)/mkbinutilspec

MKBINUTILSSPEC_DEPS = \
$(top_builddir)/mkbinutilspec binutils.spec.in $(top_builddir)/setup.cache

i386-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems

sh-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems

powerpc-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems

sparc-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

m68k-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems

mips64orion-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . mips64orion-rtems

i960-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . i960-rtems

hppa1.1-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . hppa1.1-rtems

TEMPLATES = \
binutils.spec.in

RPM_SPECS_DATA = \
i386-rtems-$(BINUTILSVERS).spec \
sh-rtems-$(BINUTILSVERS).spec \
powerpc-rtems-$(BINUTILSVERS).spec \
sparc-rtems-$(BINUTILSVERS).spec \
m68k-rtems-$(BINUTILSVERS).spec \
mips64orion-rtems-$(BINUTILSVERS).spec \
i960-rtems-$(BINUTILSVERS).spec \
hppa1.1-rtems-$(BINUTILSVERS).spec

noinst_DATA = \
$(TEMPLATES)

EXTRA_DIST = \
$(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
