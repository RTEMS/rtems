##
## $Id$
##

include $(top_srcdir)/common/common.am

MKSPEC = $(SHELL) ./mkspec

SUBPACKAGES = header.add
SUBPACKAGES += $(top_builddir)/common/common.add
SUBPACKAGES += rtems-cpukit.add
SUBPACKAGES += $(top_builddir)/common/clean.add
SUBPACKAGES += target-cpukit.add

MKSPEC_DEPS = mkspec rtems-cpukit.spec.in $(top_builddir)/setup.cache

rtems-cpukit.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = rtems-cpukit.spec.in

c4x-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . c4x-rtems@osversion@ > $@
RPM_SPECS_DATA = c4x-rtems@osversion@-cpukit.spec

h8300-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . h8300-rtems@osversion@ > $@
RPM_SPECS_DATA += h8300-rtems@osversion@-cpukit.spec

i386-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems@osversion@ > $@
RPM_SPECS_DATA += i386-rtems@osversion@-cpukit.spec

i960-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . i960-rtems@osversion@ > $@
RPM_SPECS_DATA += i960-rtems@osversion@-cpukit.spec

m68k-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems@osversion@ > $@
RPM_SPECS_DATA += m68k-rtems@osversion@-cpukit.spec

mips-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . mips-rtems@osversion@ > $@
RPM_SPECS_DATA += mips-rtems@osversion@-cpukit.spec

powerpc-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems@osversion@ > $@
RPM_SPECS_DATA += powerpc-rtems@osversion@-cpukit.spec

sh-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems@osversion@ > $@
RPM_SPECS_DATA += sh-rtems@osversion@-cpukit.spec

sh-rtemself@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtemself@osversion@ > $@
RPM_SPECS_DATA += sh-rtemself@osversion@-cpukit.spec

sparc-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems@osversion@ > $@
RPM_SPECS_DATA += sparc-rtems@osversion@-cpukit.spec

EXTRA_DIST = $(SUBPACKAGES)

CLEANFILES += $(RPM_SPECS_DATA)

include $(top_srcdir)/../automake/local.am
