##
## $Id$
##

include $(top_srcdir)/common/common.am

MKSPEC = $(SHELL) ./mkspec

SUBPACKAGES = header.add
SUBPACKAGES += $(top_builddir)/common/common.add
SUBPACKAGES += gdb.add
SUBPACKAGES += $(top_builddir)/common/clean.add
SUBPACKAGES += base-gdb.add target-gdb.add

noinst_DATA = gdb.spec.in

gdb.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = gdb.spec.in

MKGDBSPEC_DEPS = mkspec gdb.spec.in \
    $(top_builddir)/setup.cache

RPM_SPECS_DATA = 

arm-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache arm-rtems > $@
RPM_SPECS_DATA += arm-rtems-$(GDBVERS).spec

# does not build with 5.2
# c4x-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
# 	$(MKSPEC) -cfg $(top_builddir)/setup.cache c4x-rtems > $@
# RPM_SPECS_DATA += c4x-rtems-$(GDBVERS).spec

h8300-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache h8300-rtems > $@
RPM_SPECS_DATA += h8300-rtems-$(GDBVERS).spec

# does not build with 5.2
# hppa1.1-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
# 	$(MKSPEC) -cfg $(top_builddir)/setup.cache hppa1.1-rtems > $@
# RPM_SPECS_DATA += hppa1.1-rtems-$(GDBVERS).spec

RPM_SPECS_DATA += i386-rtems-$(GDBVERS).spec
i386-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i386-rtems > $@

# does not build with 5.2
# i386-RTEMS-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
# 	$(MKSPEC) -cfg $(top_builddir)/setup.cache i386-RTEMS > $@
# RPM_SPECS_DATA += i386-RTEMS-$(GDBVERS).spec

i960-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i960-rtems > $@
RPM_SPECS_DATA += i960-rtems-$(GDBVERS).spec

m68k-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache m68k-rtems > $@
RPM_SPECS_DATA += m68k-rtems-$(GDBVERS).spec

mips64orion-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mips64orion-rtems > $@
RPM_SPECS_DATA += mips64orion-rtems-$(GDBVERS).spec

mips-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mips-rtems > $@
RPM_SPECS_DATA += mips-rtems-$(GDBVERS).spec

mipstx39-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mipstx39-rtems > $@
RPM_SPECS_DATA += mipstx39-rtems-$(GDBVERS).spec

# does not build with 5.2
# powerpc-RTEMS-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
# 	$(MKSPEC) -cfg $(top_builddir)/setup.cache powerpc-RTEMS > $@
# RPM_SPECS_DATA += powerpc-RTEMS-$(GDBVERS).spec

powerpc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache powerpc-rtems > $@
RPM_SPECS_DATA += powerpc-rtems-$(GDBVERS).spec

sh-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtems > $@
RPM_SPECS_DATA += sh-rtems-$(GDBVERS).spec

sh-rtemself-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtemself > $@
RPM_SPECS_DATA += sh-rtemself-$(GDBVERS).spec

sparc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sparc-rtems > $@
RPM_SPECS_DATA += sparc-rtems-$(GDBVERS).spec

CLEANFILES += $(RPM_SPECS_DATA)

EXTRA_DIST = $(SUBPACKAGES)

include $(top_srcdir)/../automake/local.am
