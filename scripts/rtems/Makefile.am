AUTOMAKE_OPTIONS = foreign

# MKBSPSPEC = $(SHELL) $(top_srcdir)/mkbspspec
MKBSPSPEC = @echo $(top_srcdir)/mkbspspec

MKBSPSPEC_DEPS = ../mkbspspec rtems.spec.in ../setup.cache

i386-rtems-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg ../setup.cache -o . i386-rtems

sh-rtems-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg ../setup.cache -o . sh-rtems

powerpc-rtems-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg ../setup.cache -o . powerpc-rtems

sparc-rtems-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg ../setup.cache -o . sparc-rtems

m68k-rtems-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg ../setup.cache -o . m68k-rtems

mips64orion-rtems-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg ../setup.cache -o . mips64orion-rtems

i960-rtems-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg ../setup.cache -o . i960-rtems

hppa1.1-rtems-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg ../setup.cache -o . hppa1.1-rtems

TEMPLATES = \
rtems.spec.in

RPM_SPECS_DATA = \
i386-rtems-$(BSPVERS).spec \
sh-rtems-$(BSPVERS).spec \
powerpc-rtems-$(BSPVERS).spec \
sparc-rtems-$(BSPVERS).spec \
m68k-rtems-$(BSPVERS).spec \
mips64orion-rtems-$(BSPVERS).spec \
i960-rtems-$(BSPVERS).spec \
hppa1.1-rtems-$(BSPVERS).spec

noinst_DATA = \
$(TEMPLATES)

EXTRA_DIST = \
$(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
