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

MKBSPSPEC = $(SHELL) ./mkspec

MKBSPSPEC_DEPS = $(top_builddir)/mkspec rtems.spec.in \
    $(top_builddir)/setup.cache

RPM_SPECS_DATA =

@rpmprefix@i386-rtems-pc386-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems@osversion@ pc386
RPM_SPECS_DATA += @rpmprefix@i386-rtems-pc386-$(BSPVERS).spec

@rpmprefix@m68k-rtems-gen68360-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems@osversion@ gen68360
RPM_SPECS_DATA += @rpmprefix@m68k-rtems-gen68360-$(BSPVERS).spec

@rpmprefix@powerpc-rtems-mcp750-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems@osversion@ mcp750
RPM_SPECS_DATA += @rpmprefix@powerpc-rtems-mcp750-$(BSPVERS).spec

@rpmprefix@sh-rtems-gensh1-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems@osversion@ gensh1
RPM_SPECS_DATA += @rpmprefix@sh-rtems-gensh1-$(BSPVERS).spec

@rpmprefix@sparc-rtems-erc32-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems@osversion@ erc32
RPM_SPECS_DATA += @rpmprefix@sparc-rtems-erc32-$(BSPVERS).spec

CLEANFILES += $(RPM_SPECS_DATA)

include $(top_srcdir)/../automake/local.am
