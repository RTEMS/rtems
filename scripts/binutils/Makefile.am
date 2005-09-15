##
## $Id$
##

include $(top_srcdir)/common/common.am

MKBINUTILSSPEC = $(SHELL) ./mkspec

SUBPACKAGES = header.add
SUBPACKAGES += $(top_builddir)/common/common.add binutils.add
SUBPACKAGES += $(top_builddir)/common/clean.add
SUBPACKAGES += target-binutils.add
SUBPACKAGES += base-binutils.add

binutils.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = binutils.spec.in
RPM_SPECS_DATA =

MKBINUTILSSPEC_DEPS = mkspec binutils.spec.in $(top_builddir)/setup.cache

@rpmprefix@arm-rtems@osversion@-binutils.spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache arm-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@arm-rtems@osversion@-binutils.spec

@rpmprefix@h8300-rtems@osversion@-binutils.spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache h8300-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@h8300-rtems@osversion@-binutils.spec

@rpmprefix@i386-rtems@osversion@-binutils.spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache i386-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@i386-rtems@osversion@-binutils.spec

@rpmprefix@m68k-rtems@osversion@-binutils.spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache m68k-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@m68k-rtems@osversion@-binutils.spec

@rpmprefix@mips-rtems@osversion@-binutils.spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache mips-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@mips-rtems@osversion@-binutils.spec

@rpmprefix@mips64-rtems@osversion@-binutils.spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache mips64-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@mips64-rtems@osversion@-binutils.spec

@rpmprefix@powerpc-rtems@osversion@-binutils.spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache powerpc-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@powerpc-rtems@osversion@-binutils.spec

@rpmprefix@sh-rtems@osversion@-binutils.spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache sh-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sh-rtems@osversion@-binutils.spec

@rpmprefix@sh-rtemscoff@osversion@-binutils.spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache sh-rtemscoff@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sh-rtemscoff@osversion@-binutils.spec

@rpmprefix@sparc-rtems@osversion@-binutils.spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache sparc-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sparc-rtems@osversion@-binutils.spec

@rpmprefix@tic4x-rtems@osversion@-binutils.spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache tic4x-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@tic4x-rtems@osversion@-binutils.spec

@rpmprefix@avr-rtems@osversion@-binutils.spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache avr-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@avr-rtems@osversion@-binutils.spec

CLEANFILES += $(RPM_SPECS_DATA)

EXTRA_DIST = $(SUBPACKAGES)

include $(top_srcdir)/../automake/local.am
