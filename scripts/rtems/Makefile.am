##
## $Id$
##

include $(top_srcdir)/common/common.am

MKBSPSPEC = $(SHELL) $(top_builddir)/mkbspspec

MKBSPSPEC_DEPS = $(top_builddir)/mkbspspec rtems.spec.in \
    $(top_builddir)/setup.cache

i386-rtems-pc386-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems pc386

i960-rtems-cvme961-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . i960-rtems cvme961

m68k-rtems-gen68360-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems gen68360

powerpc-rtems-mcp750-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems mcp750

sh-rtems-gensh1-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems gensh1

sh-rtemself-gensh1-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtemself gensh1

sparc-rtems-erc32-$(BSPVERS).spec: $(MKBSPSPEC_DEPS)
	$(MKBSPSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems erc32

TEMPLATES = rtems.spec.in

RPM_SPECS_DATA = i386-rtems-pc386-$(BSPVERS).spec \
    i960-rtems-cvme961-$(BSPVERS).spec m68k-rtems-gen68360-$(BSPVERS).spec \
    powerpc-rtems-mcp750-$(BSPVERS).spec sh-rtems-gensh1-$(BSPVERS).spec \
    sh-rtemself-gensh1-$(BSPVERS).spec sparc-rtems-erc32-$(BSPVERS).spec

noinst_DATA = $(TEMPLATES)

EXTRA_DIST = $(TEMPLATES)

CLEANFILES = $(RPM_SPECS_DATA)

include $(top_srcdir)/../automake/local.am
