##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4

MKBINUTILSSPEC = $(SHELL) $(top_builddir)/mkbinutilspec

MKBINUTILSSPEC_DEPS = $(top_builddir)/mkbinutilspec binutils.spec.in \
    $(top_builddir)/setup.cache

hppa1.1-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . hppa1.1-rtems

i386-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems

i960-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . i960-rtems

m68k-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems

m68k-rtemself-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtemself

mips64orion-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . mips64orion-rtems

powerpc-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems

sh-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems

sh-rtemself-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtemself

sparc-rtems-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems

TEMPLATES = binutils.spec.in

RPM_SPECS_DATA = hppa1.1-rtems-$(BINUTILSVERS).spec \
    i386-rtems-$(BINUTILSVERS).spec i960-rtems-$(BINUTILSVERS).spec \
    m68k-rtems-$(BINUTILSVERS).spec m68k-rtemself-$(BINUTILSVERS).spec \
    mips64orion-rtems-$(BINUTILSVERS).spec \
    powerpc-rtems-$(BINUTILSVERS).spec sh-rtems-$(BINUTILSVERS).spec \
    sh-rtemself-$(BINUTILSVERS).spec sparc-rtems-$(BINUTILSVERS).spec

noinst_DATA = $(TEMPLATES)

EXTRA_DIST = $(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)
include $(top_srcdir)/../automake/local.am
