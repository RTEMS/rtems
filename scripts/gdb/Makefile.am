##
## $Id$
##

MKSPEC = $(SHELL) ./mkspec

SUBPACKAGES = gdb.add base-gdb.add target-gdb.add

noinst_DATA = gdb.spec.in

gdb.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = gdb.spec.in

MKGDBSPEC_DEPS = $(top_builddir)/mkspec gdb.spec.in \
    $(top_builddir)/setup.cache

arm-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache arm-rtems > $@

c4x-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache c4x-rtems > $@

h8300-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache h8300-rtems > $@

hppa1.1-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache hppa1.1-rtems > $@

i386-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i386-rtems > $@

i386-RTEMS-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i386-RTEMS > $@

i960-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i960-rtems > $@

m68k-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache m68k-rtems > $@

mips64orion-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mips64orion-rtems > $@

mips-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mips-rtems > $@

mipstx39-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mipstx39-rtems > $@

powerpc-RTEMS-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache powerpc-RTEMS > $@

powerpc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache powerpc-rtems > $@

sh-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtems > $@

sh-rtemself-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtemself > $@

sparc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sparc-rtems > $@

RPM_SPECS_DATA = \
    arm-rtems-$(GDBVERS).spec c4x-rtems-$(GDBVERS).spec \
    h8300-rtems-$(GDBVERS).spec hppa1.1-rtems-$(GDBVERS).spec  \
    i386-rtems-$(GDBVERS).spec i960-rtems-$(GDBVERS).spec \
    m68k-rtems-$(GDBVERS).spec mips-rtems-$(GDBVERS).spec \
    mipstx39-rtems-$(GDBVERS).spec mips64orion-rtems-$(GDBVERS).spec \
    powerpc-rtems-$(GDBVERS).spec sh-rtems-$(GDBVERS).spec \
    sh-rtemself-$(GDBVERS).spec sparc-rtems-$(GDBVERS).spec \
    i386-RTEMS-$(GDBVERS).spec powerpc-RTEMS-$(GDBVERS).spec

CLEANFILES += $(RPM_SPECS_DATA)

EXTRA_DIST = $(SUBPACKAGES)

include $(top_srcdir)/../automake/local.am
