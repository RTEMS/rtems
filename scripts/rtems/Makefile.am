AUTOMAKE_OPTIONS = foreign

MKBSPSPEC = $(SHELL) $(top_builddir)/mkbspspec

MKBSPSPEC_DEPS = $(top_builddir)/mkbspspec rtems.spec.in $(top_builddir)/setup.cache

i386-rtems-pc386-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems pc386

sh-rtems-gensh1-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems gensh1

powerpc-rtems-mcp750-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems mcp750

sparc-rtems-erc32-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems erc32

m68k-rtems-gen68360-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems gen68360

mips64orion-rtems-p4600-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . mips64orion-rtems p4600

i960-rtems-cvme961-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . i960-rtems cvme961

hppa1.1-rtems-simhppa-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . hppa1.1-rtems simhppa

TEMPLATES = \
rtems.spec.in

RPM_SPECS_DATA = \
i386-rtems-pc386-$(BSPVERS).spec \
sh-rtems-gensh1-$(BSPVERS).spec \
powerpc-rtems-mcp750-$(BSPVERS).spec \
sparc-rtems-erc32-$(BSPVERS).spec \
m68k-rtems-gen68360-$(BSPVERS).spec \
mips64orion-rtems-p4600-$(BSPVERS).spec \
i960-rtems-cvme961-$(BSPVERS).spec \
hppa1.1-rtems-simhppa-$(BSPVERS).spec

noinst_DATA = \
$(TEMPLATES)

EXTRA_DIST = \
$(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
