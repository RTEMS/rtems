AUTOMAKE_OPTIONS = foreign

MKGDBSPEC = $(SHELL) $(top_srcdir)/mkgdbspec

MKGDBSPEC_DEPS = ../mkgdbspec gdb.spec.in ../setup.cache

i386-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg ../setup.cache -o . i386-rtems

sh-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg ../setup.cache -o . sh-rtems

powerpc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg ../setup.cache -o . powerpc-rtems

sparc-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg ../setup.cache -o . sparc-rtems

m68k-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg ../setup.cache -o . m68k-rtems

mips64orion-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg ../setup.cache -o . mips64orion-rtems

i960-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg ../setup.cache -o . i960-rtems

hppa1.1-rtems-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKGDBSPEC) -cfg ../setup.cache -o . hppa1.1-rtems

TEMPLATES = \
gdb.spec.in

RPM_SPECS_DATA = \
i386-rtems-$(GDBVERS).spec \
sh-rtems-$(GDBVERS).spec \
powerpc-rtems-$(GDBVERS).spec \
sparc-rtems-$(GDBVERS).spec \
m68k-rtems-$(GDBVERS).spec \
mips64orion-rtems-$(GDBVERS).spec \
i960-rtems-$(GDBVERS).spec \
hppa1.1-rtems-$(GDBVERS).spec

noinst_DATA = \
$(TEMPLATES)

EXTRA_DIST = \
$(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
