##
## $Id$
##


MKSPEC = $(SHELL) ./mkspec

MKSPEC_DEPS = mkspec rtems-cpukit.spec.in \
    $(top_builddir)/setup.cache

c4x-rtems-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . c4x-rtems > $@
RPM_SPECS_DATA = c4x-rtems-cpukit.spec

hppa1.1-rtems-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . hppa1.1-rtems > $@
RPM_SPECS_DATA += hppa1.1-rtems-cpukit.spec

h8300-rtems-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . h8300-rtems > $@
RPM_SPECS_DATA += h8300-rtems-cpukit.spec

i386-rtems-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems > $@
RPM_SPECS_DATA += i386-rtems-cpukit.spec

i960-rtems-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . i960-rtems > $@
RPM_SPECS_DATA += i960-rtems-cpukit.spec

m68k-rtems-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems > $@
RPM_SPECS_DATA += m68k-rtems-cpukit.spec

mips64orion-rtems-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . mips64orion-rtems > $@
RPM_SPECS_DATA += mips64orion-rtems-cpukit.spec

mips-rtems-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . mips-rtems > $@
RPM_SPECS_DATA += mips-rtems-cpukit.spec

powerpc-rtems-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems > $@
RPM_SPECS_DATA += powerpc-rtems-cpukit.spec

sh-rtems-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems > $@
RPM_SPECS_DATA += sh-rtems-cpukit.spec

sh-rtemself-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtemself > $@
RPM_SPECS_DATA += sh-rtemself-cpukit.spec

sparc-rtems-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems > $@
RPM_SPECS_DATA += sparc-rtems-cpukit.spec

TEMPLATES = rtems-cpukit.spec.in

noinst_DATA = $(TEMPLATES)

EXTRA_DIST = $(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
include $(top_srcdir)/../automake/local.am
