##
## $Id$
##

include $(top_srcdir)/common/common.am

MKSPEC = $(SHELL) ./mkspec

SUBPACKAGES = header.add
SUBPACKAGES += $(top_builddir)/common/common.add
SUBPACKAGES += rtems-cpukit.add
SUBPACKAGES += $(top_builddir)/common/clean.add
SUBPACKAGES += target-cpukit.add

MKSPEC_DEPS = mkspec rtems-cpukit.spec.in $(top_builddir)/setup.cache

rtems-cpukit.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = rtems-cpukit.spec.in

@rpmprefix@h8300-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . h8300-rtems@osversion@ > $@
RPM_SPECS_DATA = @rpmprefix@h8300-rtems@osversion@-cpukit.spec

@rpmprefix@i386-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . i386-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@i386-rtems@osversion@-cpukit.spec

@rpmprefix@m68k-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . m68k-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@m68k-rtems@osversion@-cpukit.spec

@rpmprefix@mips-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . mips-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@mips-rtems@osversion@-cpukit.spec

@rpmprefix@powerpc-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . powerpc-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@powerpc-rtems@osversion@-cpukit.spec

@rpmprefix@sh-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . sh-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sh-rtems@osversion@-cpukit.spec

@rpmprefix@sparc-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . sparc-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sparc-rtems@osversion@-cpukit.spec

@rpmprefix@tic4x-rtems@osversion@-cpukit.spec: $(MKSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache -o . tic4x-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@tic4x-rtems@osversion@-cpukit.spec

EXTRA_DIST = $(SUBPACKAGES)

CLEANFILES += $(RPM_SPECS_DATA)

include $(top_srcdir)/../automake/local.am
