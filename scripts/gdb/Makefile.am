##
## $Id$
##


MKGDBSPEC = $(SHELL) $(top_builddir)/mkgdbspec

SUBPACKAGES = gdb.add base-gdb.add target-gdb.add

gdb.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = gdb.spec.in

MKGDBSPEC_DEPS = $(top_builddir)/mkgdbspec gdb.spec.in \
    $(top_builddir)/setup.cache

arm-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . arm-rtems

c4x-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . c4x-rtems

h8300-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . h8300-rtems

hppa1.1-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . hppa1.1-rtems

i386-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems

i386-RTEMS-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . i386-RTEMS

i960-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . i960-rtems

m68k-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems

mips64orion-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . mips64orion-rtems

mips-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . mips-rtems

mipstx39-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . mipstx39-rtems

powerpc-RTEMS-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-RTEMS

powerpc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems

sh-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems

sh-rtemself-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtemself

sparc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

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
