# $Id$

TARGET = i586-pc-freebsd5.2

include $(top_srcdir)/mkspec.am

FREEBSD_VERS = 5.2.1
FREEBSD_RPMREL = @VERSION@
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add

LIBS_SUBPACKAGES = $(top_srcdir)/common/common.add
LIBS_SUBPACKAGES += $(srcdir)/libs.add
LIBS_SUBPACKAGES += $(top_srcdir)/gcc/rpm-install.add
LIBS_SUBPACKAGES += $(top_srcdir)/common/clean.add
LIBS_SUBPACKAGES += $(srcdir)/target-libs.add

$(TARGET)-libs.spec.in: $(LIBS_SUBPACKAGES) Makefile.am
	cat $(LIBS_SUBPACKAGES) | sed \
	  -e "s/[@]FREEBSD_VERS[@]/$(FREEBSD_VERS)/g" \
	  -e "s/[@]FREEBSD_RPMREL[@]/$(FREEBSD_RPMREL)/g" \
	  -e "s,%{gcc_rpmvers},%{freebsd_rpmvers},g" \
	> $(TARGET)-libs.spec.in
CLEANFILES += $(TARGET)-libs.spec.in
	                      
@rpmprefix@$(TARGET)-libs.spec: $(TARGET)-libs.spec.in
	$(MKSPEC) $(TARGET)-libs.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-libs.spec
noinst_DATA += @rpmprefix@$(TARGET)-libs.spec


BINUTILS_VERS = 2.16.1
BINUTILS_RPMREL = @VERSION@
EXTRA_DIST += binutils-sources.add

BINUTILS_SUBPACKAGES = $(top_srcdir)/common/common.add
BINUTILS_SUBPACKAGES += $(top_srcdir)/binutils/binutils.add
BINUTILS_SUBPACKAGES += $(top_srcdir)/binutils/prep.add
BINUTILS_SUBPACKAGES += $(top_srcdir)/binutils/build.add
BINUTILS_SUBPACKAGES += $(top_srcdir)/binutils/install.add
BINUTILS_SUBPACKAGES += $(top_srcdir)/binutils/rpm-install.add
BINUTILS_SUBPACKAGES += $(top_srcdir)/common/clean.add
BINUTILS_SUBPACKAGES += $(top_srcdir)/binutils/target-binutils.add
BINUTILS_SUBPACKAGES += $(top_srcdir)/binutils/base-binutils.add

$(TARGET)-binutils.spec.in: $(BINUTILS_SUBPACKAGES) Makefile.am binutils-sources.add
	cat $(BINUTILS_SUBPACKAGES) | sed \
	  -e "/[@]SOURCES[@]/r $(srcdir)/binutils-sources.add" \
	  -e "/[@]SOURCES[@]/d" \
	  -e "s/[@]BINUTILS_VERS[@]/$(BINUTILS_VERS)/g" \
	  -e "s/[@]BINUTILS_RPMREL[@]/$(BINUTILS_RPMREL)/g" \
	  > $(TARGET)-binutils.spec.in
CLEANFILES += $(TARGET)-binutils.spec.in

@rpmprefix@$(TARGET)-binutils.spec: $(TARGET)-binutils.spec.in
	$(MKSPEC) $(TARGET)-binutils.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-binutils.spec
noinst_DATA += @rpmprefix@$(TARGET)-binutils.spec


GCC_VERS = 4.1.0
GCC_RPMREL = @VERSION@
GCC_OPTS = --languages=c,cxx,f95
EXTRA_DIST += ../gcc-prep.add
EXTRA_DIST += gcc-sources.add

GCC_SUBPACKAGES = $(top_srcdir)/common/common.add
GCC_SUBPACKAGES += $(top_srcdir)/gcc/gccnewlib.add
GCC_SUBPACKAGES += $(top_srcdir)/gcc/prep.add
GCC_SUBPACKAGES += $(top_srcdir)/gcc/build.add
GCC_SUBPACKAGES += $(top_srcdir)/gcc/install.add
GCC_SUBPACKAGES += $(top_srcdir)/gcc/rpm-install.add
GCC_SUBPACKAGES += $(top_srcdir)/common/clean.add
GCC_SUBPACKAGES += $(top_srcdir)/gcc/target-gcc.add
GCC_SUBPACKAGES += $(top_srcdir)/gcc/base-gcc.add
GCC_SUBPACKAGES += $(top_srcdir)/gcc/target-c++.add
GCC_SUBPACKAGES += $(top_srcdir)/gcc/target-gfortran.add
GCC_SUBPACKAGES += $(top_srcdir)/gcc/base-gfortran.add
GCC_SUBPACKAGES += $(top_srcdir)/gcc/target-objc.add

$(TARGET)-gcc.spec.in: $(GCC_SUBPACKAGES) Makefile.am $(srcdir)/gcc-sources.add $(srcdir)/../gcc-prep.add
	cat $(GCC_SUBPACKAGES) | sed \
	  -e "/[@]SOURCES[@]/r $(srcdir)/gcc-sources.add" \
	  -e "/[@]SOURCES[@]/d" \
	  -e "/[@]PREP[@]/r $(srcdir)/../gcc-prep.add" \
	  -e "/[@]PREP[@]/d" \
	  -e "s/[@]GCC_VERS[@]/$(GCC_VERS)/g" \
	  -e "s/[@]GCC_RPMREL[@]/$(GCC_RPMREL)/g" \
	  -e "/[@]tool_target[@]-newlib/d" \
	  -e "/[@]tool_target[@]-w32api-sys-root/d" \
	  -e "/--with-newlib/d" \
	  > $(TARGET)-gcc.spec.in
CLEANFILES += $(TARGET)-gcc.spec.in

@rpmprefix@$(TARGET)-gcc.spec: $(TARGET)-gcc.spec.in
	$(MKSPEC) $(TARGET)-gcc.spec.in | $(SPECSTRIP) $(GCC_OPTS) > $@
CLEANFILES += @rpmprefix@$(TARGET)-gcc.spec
noinst_DATA += @rpmprefix@$(TARGET)-gcc.spec
