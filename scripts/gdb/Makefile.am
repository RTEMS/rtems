##
## $Id$
##

include $(top_srcdir)/common/common.am

MKSPEC = $(SHELL) ./mkspec

SUBPACKAGES = header.add
SUBPACKAGES += $(top_builddir)/common/common.add
SUBPACKAGES += gdb.add
SUBPACKAGES += $(top_builddir)/common/clean.add
SUBPACKAGES += target-gdb.add
SUBPACKAGES += base-gdb.add

noinst_DATA = gdb.spec.in

gdb.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = gdb.spec.in

MKGDBSPEC_DEPS = mkspec gdb.spec.in $(top_builddir)/setup.cache

RPM_SPECS_DATA =

@rpmprefix@arm-rtems@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache arm-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@arm-rtems@osversion@-gdb.spec

@rpmprefix@avr-rtems@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache avr-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@avr-rtems@osversion@-gdb.spec

@rpmprefix@h8300-rtems@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache h8300-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@h8300-rtems@osversion@-gdb.spec

@rpmprefix@i386-rtems@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i386-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@i386-rtems@osversion@-gdb.spec

@rpmprefix@m68k-rtems@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache m68k-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@m68k-rtems@osversion@-gdb.spec

@rpmprefix@mips-rtems@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mips-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@mips-rtems@osversion@-gdb.spec

@rpmprefix@mips64-rtems@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mips64-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@mips64-rtems@osversion@-gdb.spec

@rpmprefix@mipstx39-rtems@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mipstx39-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@mipstx39-rtems@osversion@-gdb.spec

@rpmprefix@powerpc-rtems@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache powerpc-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@powerpc-rtems@osversion@-gdb.spec

@rpmprefix@sh-rtems@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sh-rtems@osversion@-gdb.spec

@rpmprefix@sh-rtemscoff@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtemscoff@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sh-rtemscoff@osversion@-gdb.spec

@rpmprefix@sparc-rtems@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sparc-rtems@osversion@ > $@
RPM_SPECS_DATA += @rpmprefix@sparc-rtems@osversion@-gdb.spec

# does not build with 5.2
# @rpmprefix@tic4x-rtems@osversion@-gdb.spec: $(MKGDBSPEC_DEPS)
# 	$(MKSPEC) -cfg $(top_builddir)/setup.cache tic4x-rtems@osversion@ > $@
# RPM_SPECS_DATA += @rpmprefix@tic4x-rtems@osversion@-gdb.spec

CLEANFILES += $(RPM_SPECS_DATA)

EXTRA_DIST = $(SUBPACKAGES)

include $(top_srcdir)/../automake/local.am
