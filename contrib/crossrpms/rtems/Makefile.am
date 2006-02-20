# $Id$

TARGET = @target_alias@

noinst_DATA =
CLEANFILES =

BINUTILS_VERS = 2.16.1
BINUTILS_RPMREL = @VERSION@

BINUTILS_SUBPACKAGES = $(top_builddir)/common/common.add
BINUTILS_SUBPACKAGES += ../binutils/binutils.add
BINUTILS_SUBPACKAGES += ../binutils/prep.add
BINUTILS_SUBPACKAGES += ../binutils/build.add
BINUTILS_SUBPACKAGES += ../binutils/install.add
BINUTILS_SUBPACKAGES += ../binutils/rpm-install.add
BINUTILS_SUBPACKAGES += $(top_builddir)/common/clean.add
BINUTILS_SUBPACKAGES += ../binutils/target-binutils.add
BINUTILS_SUBPACKAGES += ../binutils/base-binutils.add

$(TARGET)-binutils.spec.in: $(BINUTILS_SUBPACKAGES) Makefile.am binutils-sources.add
	cat $(BINUTILS_SUBPACKAGES) | sed \
	  -e "/[@]SOURCES[@]/r ./binutils-sources.add" \
	  -e "/[@]SOURCES[@]/d" \
	  -e "s,[@]_prefix[@],$(prefix),g" \
	  -e "s,rtems[@]osversion[@],rtems,g" \
	  > $(TARGET)-binutils.spec.in
CLEANFILES += $(TARGET)-binutils.spec.in

@rpmprefix@$(TARGET)-binutils.spec: $(TARGET)-binutils.spec.in
	sed \
	  -e "s,[@]rpmprefix[@],$(rpmprefix),g" \
	  -e "s/[@]tool_target[@]/$(TARGET)/g" \
	  -e "s/[@]BINUTILS_VERS[@]/$(BINUTILS_VERS)/g" \
	  -e "s/[@]BINUTILS_RPMREL[@]/$(BINUTILS_RPMREL)/g" \
	  -e "/[@]PATCH/d" \
	  $(srcdir)/$(TARGET)-binutils.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-binutils.spec
noinst_DATA += @rpmprefix@$(TARGET)-binutils.spec


GCC_VERS = 4.0.2
GCC_RPMREL = @VERSION@
NEWLIB_VERS = 1.14.0

GCC_SUBPACKAGES = $(top_builddir)/common/common.add
GCC_SUBPACKAGES += ../gcc/gccnewlib.add
GCC_SUBPACKAGES += ../gcc/prep.add
GCC_SUBPACKAGES += ../gcc/build.add
GCC_SUBPACKAGES += ../gcc/install.add
GCC_SUBPACKAGES += ../gcc/rpm-install.add
GCC_SUBPACKAGES += $(top_builddir)/common/clean.add
GCC_SUBPACKAGES += ../gcc/target-gcc.add
GCC_SUBPACKAGES += ../gcc/base-gcc.add
GCC_SUBPACKAGES += ../gcc/target-c++.add
GCC_SUBPACKAGES += ../gcc/base-libc.add
GCC_SUBPACKAGES += ../gcc/target-libc.add

$(TARGET)-gcc.spec.in: $(GCC_SUBPACKAGES) Makefile.am gcc-sources.add
	cat $(GCC_SUBPACKAGES) | sed \
	  -e "/[@]SOURCES[@]/r ./gcc-sources.add" \
	  -e "/[@]SOURCES[@]/d" \
	  -e "s,[@]_prefix[@],$(prefix),g" \
	  -e "s,rtems[@]osversion[@],rtems,g" \
	  -e "/^%package -n @rpmprefix@@tool_target@-gcc/d" \
	  -e "s/-n @rpmprefix@@tool_target@-gcc//" \
	  -e "/--with-sysroot/d" \
	  -e "/BuildRequires:.*@tool_target@-sys-root/d" \
	  -e 's,^%define build_newlib.*$$,%define build_newlib 1,' \
	  > $(TARGET)-gcc.spec.in
CLEANFILES += $(TARGET)-gcc.spec.in

@rpmprefix@$(TARGET)-gcc.spec: $(TARGET)-gcc.spec.in
	sed \
	  -e "s,[@]rpmprefix[@],$(rpmprefix),g" \
	  -e "s/[@]tool_target[@]/$(TARGET)/g" \
	  -e "s/[@]GCC_VERS[@]/$(GCC_VERS)/g" \
	  -e "s/[@]GCC_RPMREL[@]/$(GCC_RPMREL)/g" \
	  -e "s/[@]NEWLIB_VERS[@]/$(NEWLIB_VERS)/g" \
	  -e "/[@]PATCH/d" \
	  $(srcdir)/$(TARGET)-gcc.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-gcc.spec
noinst_DATA += @rpmprefix@$(TARGET)-gcc.spec
