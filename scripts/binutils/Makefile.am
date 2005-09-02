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

@rpmprefix@arm-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache arm-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@arm-rtems@osversion@-$(BINUTILSVERS).spec

@rpmprefix@h8300-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache h8300-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@h8300-rtems@osversion@-$(BINUTILSVERS).spec

@rpmprefix@i386-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache i386-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@i386-rtems@osversion@-$(BINUTILSVERS).spec

@rpmprefix@m68k-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache m68k-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@m68k-rtems@osversion@-$(BINUTILSVERS).spec

@rpmprefix@mips-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache mips-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@mips-rtems@osversion@-$(BINUTILSVERS).spec

@rpmprefix@mips64-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache mips64-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@mips64-rtems@osversion@-$(BINUTILSVERS).spec

@rpmprefix@powerpc-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache powerpc-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@powerpc-rtems@osversion@-$(BINUTILSVERS).spec

@rpmprefix@sh-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache sh-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sh-rtems@osversion@-$(BINUTILSVERS).spec

@rpmprefix@sh-rtemscoff@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache sh-rtemscoff@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sh-rtemscoff@osversion@-$(BINUTILSVERS).spec

@rpmprefix@sparc-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache sparc-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sparc-rtems@osversion@-$(BINUTILSVERS).spec

@rpmprefix@tic4x-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache tic4x-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@tic4x-rtems@osversion@-$(BINUTILSVERS).spec

@rpmprefix@avr-rtems@osversion@-$(BINUTILSVERS).spec: $(MKBINUTILSSPEC_DEPS)
	$(MKBINUTILSSPEC) -cfg $(top_builddir)/setup.cache avr-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@avr-rtems@osversion@-$(BINUTILSVERS).spec

CLEANFILES += $(RPM_SPECS_DATA)

EXTRA_DIST = $(SUBPACKAGES)

include $(top_srcdir)/../automake/local.am
