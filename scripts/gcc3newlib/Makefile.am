##
## $Id$
##

MKSPEC = $(SHELL) ./mkspec

SUBPACKAGES = gccnewlib.add \
    base-gcc.add target-gcc.add \
    base-g77.add target-g77.add \
    base-gcj.add target-gcj.add \
    target-objc.add \
    target-c++.add \
    base-gnat.add target-gnat.add

noinst_DATA = gccnewlib.spec.in

gccnewlib.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = gccnewlib.spec.in

MKGCCNEWLIBSPEC_DEPS = mkspec gccnewlib.spec.in \
    $(top_builddir)/setup.cache

RPM_SPECS_DATA =

arm-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache arm-rtems > $@
RPM_SPECS_DATA += arm-rtems-$(GCC3NEWLIBVERS).spec

c4x-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache c4x-rtems > $@
RPM_SPECS_DATA += c4x-rtems-$(GCC3NEWLIBVERS).spec

h8300-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache h8300-rtems > $@
RPM_SPECS_DATA += h8300-rtems-$(GCC3NEWLIBVERS).spec

hppa1.1-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache hppa1.1-rtems > $@
RPM_SPECS_DATA += hppa1.1-rtems-$(GCC3NEWLIBVERS).spec

i386-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i386-rtems > $@
RPM_SPECS_DATA += i386-rtems-$(GCC3NEWLIBVERS).spec

i960-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i960-rtems > $@
RPM_SPECS_DATA += i960-rtems-$(GCC3NEWLIBVERS).spec

m68k-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache m68k-rtems > $@
RPM_SPECS_DATA += m68k-rtems-$(GCC3NEWLIBVERS).spec

mips64orion-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mips64orion-rtems > $@
RPM_SPECS_DATA += mips64orion-rtems-$(GCC3NEWLIBVERS).spec

mips-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mips-rtems > $@
RPM_SPECS_DATA += mips-rtems-$(GCC3NEWLIBVERS).spec

powerpc-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache powerpc-rtems > $@
RPM_SPECS_DATA += powerpc-rtems-$(GCC3NEWLIBVERS).spec

sh-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtems > $@
RPM_SPECS_DATA += sh-rtems-$(GCC3NEWLIBVERS).spec

sh-rtemself-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtemself > $@
RPM_SPECS_DATA += sh-rtemself-$(GCC3NEWLIBVERS).spec

sparc-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sparc-rtems > $@
RPM_SPECS_DATA += sparc-rtems-$(GCC3NEWLIBVERS).spec

CLEANFILES += $(RPM_SPECS_DATA)

EXTRA_DIST = $(SUBPACKAGES)

include $(top_srcdir)/../automake/local.am
