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

arm-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache arm-rtems > $@
RPM_SPECS_DATA += arm-rtems-$(BINUTILSVERS).spec

c4x-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache c4x-rtems > $@
RPM_SPECS_DATA += c4x-rtems-$(BINUTILSVERS).spec

h8300-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache h8300-rtems > $@
RPM_SPECS_DATA += h8300-rtems-$(BINUTILSVERS).spec

hppa1.1-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache hppa1.1-rtems > $@
RPM_SPECS_DATA += hppa1.1-rtems-$(BINUTILSVERS).spec

i386-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache i386-rtems > $@
RPM_SPECS_DATA += i386-rtems-$(BINUTILSVERS).spec

i960-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache i960-rtems > $@
RPM_SPECS_DATA += i960-rtems-$(BINUTILSVERS).spec

m68k-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache m68k-rtems > $@
RPM_SPECS_DATA += m68k-rtems-$(BINUTILSVERS).spec

mips64orion-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache mips64orion-rtems > $@
RPM_SPECS_DATA += mips64orion-rtems-$(BINUTILSVERS).spec

mips-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache mips-rtems > $@
RPM_SPECS_DATA += mips-rtems-$(BINUTILSVERS).spec

or32-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache or32-rtems > $@
RPM_SPECS_DATA += or32-rtems-$(BINUTILSVERS).spec

powerpc-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache powerpc-rtems > $@
RPM_SPECS_DATA += powerpc-rtems-$(BINUTILSVERS).spec

sh-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache sh-rtems > $@
RPM_SPECS_DATA += sh-rtems-$(BINUTILSVERS).spec

sh-rtemself-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache sh-rtemself > $@
RPM_SPECS_DATA += sh-rtemself-$(BINUTILSVERS).spec

sparc-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache sparc-rtems > $@
RPM_SPECS_DATA += sparc-rtems-$(BINUTILSVERS).spec

CLEANFILES += $(RPM_SPECS_DATA)

EXTRA_DIST = $(SUBPACKAGES)

include $(top_srcdir)/../automake/local.am
