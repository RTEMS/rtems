##
## $Id$
##

include $(top_srcdir)/common/common.am

MKSPEC = $(SHELL) ./mkspec

SUBPACKAGES = header.add
SUBPACKAGES += $(top_builddir)/common/common.add
SUBPACKAGES += gdb.add
SUBPACKAGES += $(top_builddir)/common/clean.add
SUBPACKAGES += base-gdb.add target-gdb.add

noinst_DATA = gdb.spec.in

gdb.spec.in: $(SUBPACKAGES)
	cat $^ > $@
CLEANFILES = gdb.spec.in

MKGDBSPEC_DEPS = mkspec gdb.spec.in $(top_builddir)/setup.cache

RPM_SPECS_DATA =

arm-rtems@osversion@-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache arm-rtems@osversion@ > $@
RPM_SPECS_DATA += arm-rtems@osversion@-$(GDBVERS).spec

h8300-rtems@osversion@-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache h8300-rtems@osversion@ > $@
RPM_SPECS_DATA += h8300-rtems@osversion@-$(GDBVERS).spec

RPM_SPECS_DATA += i386-rtems@osversion@-$(GDBVERS).spec
i386-rtems@osversion@-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i386-rtems@osversion@ > $@

# does not build with 5.2
# i386-RTEMS-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
# 	$(MKSPEC) -cfg $(top_builddir)/setup.cache i386-RTEMS > $@
# RPM_SPECS_DATA += i386-RTEMS-$(GDBVERS).spec

i960-rtems@osversion@-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache i960-rtems@osversion@ > $@
RPM_SPECS_DATA += i960-rtems@osversion@-$(GDBVERS).spec

m68k-rtems@osversion@-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache m68k-rtems@osversion@ > $@
RPM_SPECS_DATA += m68k-rtems@osversion@-$(GDBVERS).spec

mips-rtems@osversion@-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mips-rtems@osversion@ > $@
RPM_SPECS_DATA += mips-rtems@osversion@-$(GDBVERS).spec

mipstx39-rtems@osversion@-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache mipstx39-rtems@osversion@ > $@
RPM_SPECS_DATA += mipstx39-rtems@osversion@-$(GDBVERS).spec

# does not build with 5.2
# powerpc-RTEMS-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
# 	$(MKSPEC) -cfg $(top_builddir)/setup.cache powerpc-RTEMS > $@
# RPM_SPECS_DATA += powerpc-RTEMS-$(GDBVERS).spec

powerpc-rtems@osversion@-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache powerpc-rtems@osversion@ > $@
RPM_SPECS_DATA += powerpc-rtems@osversion@-$(GDBVERS).spec

sh-rtems@osversion@-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtems@osversion@ > $@
RPM_SPECS_DATA += sh-rtems@osversion@-$(GDBVERS).spec

sh-rtemself@osversion@-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sh-rtemself@osversion@ > $@
RPM_SPECS_DATA += sh-rtemself@osversion@-$(GDBVERS).spec

sparc-rtems@osversion@-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
	$(MKSPEC) -cfg $(top_builddir)/setup.cache sparc-rtems@osversion@ > $@
RPM_SPECS_DATA += sparc-rtems@osversion@-$(GDBVERS).spec

# does not build with 5.2
# tic4x-rtems@osversion@-$(GDBVERS).spec: $(MKGDBSPEC_DEPS)
# 	$(MKSPEC) -cfg $(top_builddir)/setup.cache tic4x-rtems@osversion@ > $@
# RPM_SPECS_DATA += tic4x-rtems@osversion@-$(GDBVERS).spec


CLEANFILES += $(RPM_SPECS_DATA)

EXTRA_DIST = $(SUBPACKAGES)

include $(top_srcdir)/../automake/local.am
