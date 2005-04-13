##
## $Id$
##

include $(top_srcdir)/common/common.am

SUBPACKAGES = header.add
SUBPACKAGES += $(top_builddir)/common/common.add
SUBPACKAGES += rtems.add
SUBPACKAGES += $(top_builddir)/common/clean.add
SUBPACKAGES += target-rtems.add

noinst_DATA = rtems.spec.in

rtems.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = rtems.spec.in
RPM_SPECS_DATA =

MKBSPSPEC = $(SHELL) ./mkspec

MKBSPSPEC_DEPS = $(top_builddir)/mkspec rtems.spec.in \
    $(top_builddir)/setup.cache

i386-rtems-pc386-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems pc386
RPM_SPECS_DATA += i386-rtems-pc386-$(BSPVERS).spec

m68k-rtems-gen68360-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems gen68360
RPM_SPECS_DATA += i386-rtems-gen68360-$(BSPVERS).spec

powerpc-rtems-mcp750-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems mcp750
RPM_SPECS_DATA += powerpc-rtems-mcp750-$(BSPVERS).spec

sh-rtems-gensh1-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems gensh1
RPM_SPECS_DATA += sh-rtems-gensh1-$(BSPVERS).spec

sparc-rtems-erc32-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems erc32
RPM_SPECS_DATA += sparc-rtems-erc32-$(BSPVERS).spec

CLEANFILES += $(RPM_SPECS_DATA)

include $(top_srcdir)/../automake/local.am
