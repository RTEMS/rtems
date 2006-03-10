# $Id$

TARGET = @target_alias@

include ../mkspec.am

noinst_DATA =
CLEANFILES =
EXTRA_DIST =

BINUTILS_VERS = 2.16.1
BINUTILS_RPMREL = @VERSION@
EXTRA_DIST += binutils-sources.add

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
	  -e "s/[@]BINUTILS_VERS[@]/$(BINUTILS_VERS)/g" \
	  -e "s/[@]BINUTILS_RPMREL[@]/$(BINUTILS_RPMREL)/g" \
	  -e "/--with-sysroot/d" \
	  > $(TARGET)-binutils.spec.in
CLEANFILES += $(TARGET)-binutils.spec.in

@rpmprefix@$(TARGET)-binutils.spec: $(TARGET)-binutils.spec.in
	$(MKSPEC) $(srcdir)/$(TARGET)-binutils.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-binutils.spec
noinst_DATA += @rpmprefix@$(TARGET)-binutils.spec


GCC_VERS = 4.1.0
GCC_RPMREL = @VERSION@
NEWLIB_VERS = 1.14.0
EXTRA_DIST += gcc-prep.add
EXTRA_DIST += gcc-sources.add

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
GCC_SUBPACKAGES += ../gcc/target-gfortran.add
GCC_SUBPACKAGES += ../gcc/base-gfortran.add
GCC_SUBPACKAGES += ../gcc/target-objc.add
GCC_SUBPACKAGES += ../gcc/target-newlib.add
GCC_SUBPACKAGES += ../gcc/base-newlib.add

$(TARGET)-gcc.spec.in: $(GCC_SUBPACKAGES) Makefile.am gcc-sources.add gcc-prep.add
	cat $(GCC_SUBPACKAGES) | sed \
	  -e "/[@]SOURCES[@]/r ./gcc-sources.add" \
	  -e "/[@]SOURCES[@]/d" \
	  -e "/[@]PREP[@]/r ./gcc-prep.add" \
	  -e "/[@]PREP[@]/d" \
	  -e "s/[@]GCC_VERS[@]/$(GCC_VERS)/g" \
	  -e "s/[@]GCC_RPMREL[@]/$(GCC_RPMREL)/g" \
	  -e "s/[@]NEWLIB_VERS[@]/$(NEWLIB_VERS)/g" \
	  -e "/--with-sysroot/d" \
	  -e "/[@]tool_target[@]-sys-root/d" \
	  -e "/[@]tool_target[@]-w32api-sys-root/d" \
	  -e 's,^%define build_newlib.*$$,%define build_newlib 1,' \
	  > $(TARGET)-gcc.spec.in
CLEANFILES += $(TARGET)-gcc.spec.in

@rpmprefix@$(TARGET)-gcc.spec: $(TARGET)-gcc.spec.in
	$(MKSPEC) $(srcdir)/$(TARGET)-gcc.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-gcc.spec
noinst_DATA += @rpmprefix@$(TARGET)-gcc.spec


GDB_VERS = 6.4
GDB_RPMREL = @VERSION@
EXTRA_DIST += gdb-sources.add

GDB_SUBPACKAGES = $(top_builddir)/common/common.add
GDB_SUBPACKAGES += ../gdb/gdb.add
GDB_SUBPACKAGES += ../gdb/prep.add
GDB_SUBPACKAGES += ../gdb/build.add
GDB_SUBPACKAGES += ../gdb/install.add
GDB_SUBPACKAGES += ../gdb/rpm-install.add
GDB_SUBPACKAGES += $(top_builddir)/common/clean.add
GDB_SUBPACKAGES += ../gdb/target-gdb.add
GDB_SUBPACKAGES += ../gdb/base-gdb.add

$(TARGET)-gdb.spec.in: $(GDB_SUBPACKAGES) Makefile.am gdb-sources.add
	cat $(GDB_SUBPACKAGES) | sed \
	  -e "/[@]SOURCES[@]/r ./gdb-sources.add" \
	  -e "/[@]SOURCES[@]/d" \
	  -e "s/[@]GDB_VERS[@]/$(GDB_VERS)/g" \
	  -e "s/[@]GDB_RPMREL[@]/$(GDB_RPMREL)/g" \
	  -e "/--with-sysroot/d" \
	  > $(TARGET)-gdb.spec.in
CLEANFILES += $(TARGET)-gdb.spec.in

@rpmprefix@$(TARGET)-gdb.spec: $(TARGET)-gdb.spec.in
	$(MKSPEC) $(srcdir)/$(TARGET)-gdb.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-gdb.spec
noinst_DATA += @rpmprefix@$(TARGET)-gdb.spec
