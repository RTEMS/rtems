##
## $Id$
##

include $(top_srcdir)/common/common.am

MKBINUTILSSPEC = $(SHELL) ./mkspec

SUBPACKAGES = header.add
SUBPACKAGES += $(top_builddir)/common/common.add binutils.add
SUBPACKAGES += $(top_builddir)/common/clean.add
SUBPACKAGES += base-binutils.add target-binutils.add

binutils.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = binutils.spec.in
RPM_SPECS_DATA =

MKBINUTILSSPEC_DEPS = mkspec binutils.spec.in \
    $(top_builddir)/setup.cache

arm-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache arm-rtems@osversion@ > $@
RPM_SPECS_DATA += arm-rtems@osversion@-$(BINUTILSVERS).spec

c4x-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache c4x-rtems@osversion@ > $@
RPM_SPECS_DATA += c4x-rtems@osversion@-$(BINUTILSVERS).spec

h8300-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache h8300-rtems@osversion@ > $@
RPM_SPECS_DATA += h8300-rtems@osversion@-$(BINUTILSVERS).spec

hppa1.1-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache hppa1.1-rtems@osversion@ > $@
RPM_SPECS_DATA += hppa1.1-rtems@osversion@-$(BINUTILSVERS).spec

i386-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache i386-rtems@osversion@ > $@
RPM_SPECS_DATA += i386-rtems@osversion@-$(BINUTILSVERS).spec

i960-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache i960-rtems@osversion@ > $@
RPM_SPECS_DATA += i960-rtems@osversion@-$(BINUTILSVERS).spec

m68k-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache m68k-rtems@osversion@ > $@
RPM_SPECS_DATA += m68k-rtems@osversion@-$(BINUTILSVERS).spec

mips64orion-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache mips64orion-rtems@osversion@ > $@
RPM_SPECS_DATA += mips64orion-rtems@osversion@-$(BINUTILSVERS).spec

mips-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache mips-rtems@osversion@ > $@
RPM_SPECS_DATA += mips-rtems@osversion@-$(BINUTILSVERS).spec

or32-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache or32-rtems@osversion@ > $@
RPM_SPECS_DATA += or32-rtems@osversion@-$(BINUTILSVERS).spec

powerpc-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache powerpc-rtems@osversion@ > $@
RPM_SPECS_DATA += powerpc-rtems@osversion@-$(BINUTILSVERS).spec

sh-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache sh-rtems@osversion@ > $@
RPM_SPECS_DATA += sh-rtems@osversion@-$(BINUTILSVERS).spec

sh-rtemself@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache sh-rtemself@osversion@ > $@
RPM_SPECS_DATA += sh-rtemself@osversion@-$(BINUTILSVERS).spec

sparc-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache sparc-rtems@osversion@ > $@
RPM_SPECS_DATA += sparc-rtems@osversion@-$(BINUTILSVERS).spec

CLEANFILES += $(RPM_SPECS_DATA)

EXTRA_DIST = $(SUBPACKAGES)

include $(top_srcdir)/../automake/local.am
