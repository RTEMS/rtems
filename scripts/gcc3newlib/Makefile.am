##
## $Id$
##

include $(top_srcdir)/common/common.am

MKSPEC = $(SHELL) ./mkspec

SUBPACKAGES = header.add
SUBPACKAGES += $(top_builddir)/common/common.add
SUBPACKAGES += gccnewlib.add
SUBPACKAGES += $(top_builddir)/common/clean.add
SUBPACKAGES += base-gcc.add target-gcc.add base-g77.add target-g77.add \
    base-gcj.add target-gcj.add target-objc.add target-c++.add base-gnat.add \
    target-gnat.add

noinst_DATA = gccnewlib.spec.in

gccnewlib.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = gccnewlib.spec.in

MKGCCNEWLIBSPEC_DEPS = mkspec gccnewlib.spec.in $(top_builddir)/setup.cache

RPM_SPECS_DATA =

arm-rtems@osversion@-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache arm-rtems@osversion@ > $@
RPM_SPECS_DATA += arm-rtems@osversion@-$(GCC3NEWLIBVERS).spec

c4x-rtems@osversion@-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache c4x-rtems@osversion@ > $@
RPM_SPECS_DATA += c4x-rtems@osversion@-$(GCC3NEWLIBVERS).spec

tic4x-rtems@osversion@-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache tic4x-rtems@osversion@ > $@
RPM_SPECS_DATA += tic4x-rtems@osversion@-$(GCC3NEWLIBVERS).spec

h8300-rtems@osversion@-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache h8300-rtems@osversion@ > $@
RPM_SPECS_DATA += h8300-rtems@osversion@-$(GCC3NEWLIBVERS).spec

i386-rtems@osversion@-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i386-rtems@osversion@ > $@
RPM_SPECS_DATA += i386-rtems@osversion@-$(GCC3NEWLIBVERS).spec

i960-rtems@osversion@-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i960-rtems@osversion@ > $@
RPM_SPECS_DATA += i960-rtems@osversion@-$(GCC3NEWLIBVERS).spec

m68k-rtems@osversion@-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache m68k-rtems@osversion@ > $@
RPM_SPECS_DATA += m68k-rtems@osversion@-$(GCC3NEWLIBVERS).spec

mips-rtems@osversion@-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mips-rtems@osversion@ > $@
RPM_SPECS_DATA += mips-rtems@osversion@-$(GCC3NEWLIBVERS).spec

powerpc-rtems@osversion@-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache powerpc-rtems@osversion@ > $@
RPM_SPECS_DATA += powerpc-rtems@osversion@-$(GCC3NEWLIBVERS).spec

sh-rtems@osversion@-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtems@osversion@ > $@
RPM_SPECS_DATA += sh-rtems@osversion@-$(GCC3NEWLIBVERS).spec

sh-rtemself@osversion@-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtemself@osversion@ > $@
RPM_SPECS_DATA += sh-rtemself@osversion@-$(GCC3NEWLIBVERS).spec

sparc-rtems@osversion@-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sparc-rtems@osversion@ > $@
RPM_SPECS_DATA += sparc-rtems@osversion@-$(GCC3NEWLIBVERS).spec

CLEANFILES += $(RPM_SPECS_DATA)

EXTRA_DIST = $(SUBPACKAGES)

include $(top_srcdir)/../automake/local.am
