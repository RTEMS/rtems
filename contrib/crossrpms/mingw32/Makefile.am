# $Id$

TARGET = i686-pc-mingw32

noinst_DATA =
CLEANFILES =

MINGW32_VERS = 3.9
MINGW32_RPMREL = @VERSION@

LIBS_SUBPACKAGES = $(top_builddir)/common/common.add
LIBS_SUBPACKAGES += libs.add
LIBS_SUBPACKAGES += ../gcc/rpm-install.add
LIBS_SUBPACKAGES += $(top_builddir)/common/clean.add
LIBS_SUBPACKAGES += target-libs.add

$(TARGET)-libs.spec.in: $(LIBS_SUBPACKAGES) Makefile.am
	cat $(LIBS_SUBPACKAGES) | sed \
	  -e "s/[@]MINGW32_VERS[@]/$(MINGW32_VERS)/g" \
	  -e "s/[@]MINGW32_RPMREL[@]/$(MINGW32_RPMREL)/g" \
	> $(TARGET)-libs.spec.in
CLEANFILES += $(TARGET)-libs.spec.in
	                      
@rpmprefix@$(TARGET)-libs.spec: $(TARGET)-libs.spec.in
	sed \
	  -e "s,[@]rpmprefix[@],$(rpmprefix),g" \
	  -e "s,[@]_prefix[@],$(prefix),g" \
	  -e "s/[@]tool_target[@]/$(TARGET)/g" \
	  $(srcdir)/$(TARGET)-libs.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-libs.spec
noinst_DATA += @rpmprefix@$(TARGET)-libs.spec

W32API_VERS = 3.6
W32API_RPMREL = @VERSION@

W32API_SUBPACKAGES = $(top_builddir)/common/common.add
W32API_SUBPACKAGES += w32api.add
W32API_SUBPACKAGES += ../gcc/rpm-install.add
W32API_SUBPACKAGES += $(top_builddir)/common/clean.add
W32API_SUBPACKAGES += target-w32api.add

$(TARGET)-w32api.spec.in: $(W32API_SUBPACKAGES) Makefile.am
	cat $(W32API_SUBPACKAGES) | sed \
	  -e "s/[@]W32API_VERS[@]/$(W32API_VERS)/g" \
	  -e "s/[@]W32API_RPMREL[@]/$(W32API_RPMREL)/g" \
	> $(TARGET)-w32api.spec.in
CLEANFILES += $(TARGET)-w32api.spec.in
	                      
@rpmprefix@$(TARGET)-w32api.spec: $(TARGET)-w32api.spec.in
	sed \
	  -e "s,[@]rpmprefix[@],$(rpmprefix),g" \
	  -e "s,[@]_prefix[@],$(prefix),g" \
	  -e "s/[@]tool_target[@]/$(TARGET)/g" \
	  $(srcdir)/$(TARGET)-w32api.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-w32api.spec
noinst_DATA += @rpmprefix@$(TARGET)-w32api.spec


BINUTILS_VERS = 2.16.91-20060119-1
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
	  -e "s/[@]BINUTILS_VERS[@]/$(BINUTILS_VERS)/g" \
	  -e "s/[@]BINUTILS_RPMREL[@]/$(BINUTILS_RPMREL)/g" \
	  -e "s,rtems[@]osversion[@],rtems,g" \
	  > $(TARGET)-binutils.spec.in
CLEANFILES += $(TARGET)-binutils.spec.in

@rpmprefix@$(TARGET)-binutils.spec: $(TARGET)-binutils.spec.in
	sed \
	  -e "s,[@]rpmprefix[@],$(rpmprefix),g" \
	  -e "s,[@]_prefix[@],$(prefix),g" \
	  -e "s/[@]tool_target[@]/$(TARGET)/g" \
	  -e "/[@]PATCH/d" \
	  $(srcdir)/$(TARGET)-binutils.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-binutils.spec
noinst_DATA += @rpmprefix@$(TARGET)-binutils.spec


GCC_VERS = 4.0.2
GCC_RPMREL = @VERSION@

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

$(TARGET)-gcc.spec.in: $(GCC_SUBPACKAGES) Makefile.am gcc-sources.add
	cat $(GCC_SUBPACKAGES) | sed \
	  -e "/[@]SOURCES[@]/r ./gcc-sources.add" \
	  -e "/[@]SOURCES[@]/d" \
	  -e "s/[@]GCC_VERS[@]/$(GCC_VERS)/g" \
	  -e "s/[@]GCC_RPMREL[@]/$(GCC_RPMREL)/g" \
	  -e "s,rtems[@]osversion[@],rtems,g" \
	  -e "/[@]rpmprefix[@][@]tool_target[@]-libc/d" \
	  -e "s,^%define[ ]*_cxx[ 	]*1,%define _cxx		0," \
	  -e "s,[=<][ ]*%{newlib_version}-%{release},," \
	  -e "/^%package -n @rpmprefix@@tool_target@-gcc/d" \
	  -e "s/-n @rpmprefix@@tool_target@-gcc//" \
	  -e 's,^%define build_newlib.*$$,%define build_newlib 0,' \
	  > $(TARGET)-gcc.spec.in
CLEANFILES += $(TARGET)-gcc.spec.in

@rpmprefix@$(TARGET)-gcc.spec: $(TARGET)-gcc.spec.in
	sed \
	  -e "s,[@]rpmprefix[@],$(rpmprefix),g" \
	  -e "s,[@]_prefix[@],$(prefix),g" \
	  -e "s/[@]tool_target[@]/$(TARGET)/g" \
	  -e "/[@]PATCH/d" \
	  $(srcdir)/$(TARGET)-gcc.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-gcc.spec
noinst_DATA += @rpmprefix@$(TARGET)-gcc.spec
