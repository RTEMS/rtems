##
## $Id$
##

MKGCCNEWLIBSPEC = $(SHELL) $(top_builddir)/mkgcc3newlibspec

SUBPACKAGES = gccnewlib.add \
    base-gcc.add target-gcc.add \
    base-g77.add target-g77.add \
    base-gcj.add target-gcj.add \
    target-objc.add

gccnewlib.spec.in: $(SUBPACKAGES)
	cat $^ \
	| sed -e 's%@GCCLANGS@%%g' > $@
CLEANFILES = gccnewlib.spec.in

gccnewlib_c_only.spec.in: gccnewlib.add base-gcc.add target-gcc.add
	cat $^ \
	| sed -e "s%@GCCLANGS@%\'--enable-languages=c,c++\'%g" > $@
CLEANFILES += gccnewlib_c_only.spec.in

C_ONLY_ARG = -a $(top_builddir)/gcc3newlib/gccnewlib_c_only.spec.in

MKGCCNEWLIBSPEC_DEPS = $(top_builddir)/mkgcc3newlibspec gccnewlib.spec.in \
    $(top_builddir)/setup.cache
MKGCCNEWLIB_C_ONLY_SPEC_DEPS = $(top_builddir)/mkgcc3newlibspec \
    gccnewlib_c_only.spec.in $(top_builddir)/setup.cache

arm-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . arm-rtems

c4x-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIB_C_ONLY_SPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . c4x-rtems

h8300-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIB_C_ONLY_SPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . h8300-rtems

hppa1.1-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . hppa1.1-rtems

i386-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . i386-rtems

i960-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIB_C_ONLY_SPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . i960-rtems

m68k-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . m68k-rtems

mips64orion-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . mips64orion-rtems

mips-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . mips-rtems

powerpc-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems

sh-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . sh-rtems

sh-rtemself-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . sh-rtemself

sparc-rtems-$(GCC3NEWLIBVERS).spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKGCCNEWLIBSPEC) $(C_ONLY_ARG) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

RPM_SPECS_DATA = \
    arm-rtems-$(GCC3NEWLIBVERS).spec \
    c4x-rtems-$(GCC3NEWLIBVERS).spec \
    h8300-rtems-$(GCC3NEWLIBVERS).spec \
    hppa1.1-rtems-$(GCC3NEWLIBVERS).spec \
    i386-rtems-$(GCC3NEWLIBVERS).spec i960-rtems-$(GCC3NEWLIBVERS).spec \
    m68k-rtems-$(GCC3NEWLIBVERS).spec  \
    mips-rtems-$(GCC3NEWLIBVERS).spec mips64orion-rtems-$(GCC3NEWLIBVERS).spec \
    powerpc-rtems-$(GCC3NEWLIBVERS).spec sh-rtems-$(GCC3NEWLIBVERS).spec \
    sh-rtemself-$(GCC3NEWLIBVERS).spec sparc-rtems-$(GCC3NEWLIBVERS).spec
CLEANFILES += $(RPM_SPECS_DATA)

EXTRA_DIST = $(SUBPACKAGES)

include $(top_srcdir)/../automake/local.am
