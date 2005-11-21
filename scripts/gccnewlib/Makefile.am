##
## $Id$
##

include $(top_srcdir)/common/common.am

MKSPEC = $(SHELL) ./mkspec

SUBPACKAGES = header.add
SUBPACKAGES += $(top_builddir)/common/common.add
SUBPACKAGES += gccnewlib.add
SUBPACKAGES += $(top_builddir)/common/clean.add
SUBPACKAGES += target-gcc.add base-gcc.add
SUBPACKAGES += target-g77.add base-g77.add
SUBPACKAGES += target-gcj.add base-gcj.add
SUBPACKAGES += target-gfortran.add base-gfortran.add
SUBPACKAGES += target-objc.add
SUBPACKAGES += target-c++.add
SUBPACKAGES += target-gnat.add base-gnat.add
SUBPACKAGES += target-libc.add

noinst_DATA = gccnewlib.spec.in

gccnewlib.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = gccnewlib.spec.in

MKGCCNEWLIBSPEC_DEPS = mkspec gccnewlib.spec.in $(top_builddir)/setup.cache

RPM_SPECS_DATA =

@rpmprefix@arm-rtems@osversion@-gcc-newlib.spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache arm-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@arm-rtems@osversion@-gcc-newlib.spec

@rpmprefix@avr-rtems@osversion@-gcc-newlib.spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache avr-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@avr-rtems@osversion@-gcc-newlib.spec

@rpmprefix@h8300-rtems@osversion@-gcc-newlib.spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache h8300-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@h8300-rtems@osversion@-gcc-newlib.spec

@rpmprefix@i386-rtems@osversion@-gcc-newlib.spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i386-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@i386-rtems@osversion@-gcc-newlib.spec

@rpmprefix@m68k-rtems@osversion@-gcc-newlib.spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache m68k-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@m68k-rtems@osversion@-gcc-newlib.spec

@rpmprefix@mips-rtems@osversion@-gcc-newlib.spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mips-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@mips-rtems@osversion@-gcc-newlib.spec

@rpmprefix@mips64-rtems@osversion@-gcc-newlib.spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mips64-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@mips64-rtems@osversion@-gcc-newlib.spec

@rpmprefix@powerpc-rtems@osversion@-gcc-newlib.spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache powerpc-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@powerpc-rtems@osversion@-gcc-newlib.spec

@rpmprefix@sh-rtems@osversion@-gcc-newlib.spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sh-rtems@osversion@-gcc-newlib.spec

@rpmprefix@sh-rtemscoff@osversion@-gcc-newlib.spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtemscoff@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sh-rtemscoff@osversion@-gcc-newlib.spec

@rpmprefix@sparc-rtems@osversion@-gcc-newlib.spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sparc-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sparc-rtems@osversion@-gcc-newlib.spec

@rpmprefix@tic4x-rtems@osversion@-gcc-newlib.spec: $(MKGCCNEWLIBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache tic4x-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@tic4x-rtems@osversion@-gcc-newlib.spec

CLEANFILES += $(RPM_SPECS_DATA)

EXTRA_DIST = $(SUBPACKAGES)

include $(top_srcdir)/../automake/local.am
