# $Id$

TARGET = i686-pc-cygwin

include ../mkspec.am

noinst_DATA =
CLEANFILES =
EXTRA_DIST =

CYGWIN_VERS = 1.5.19-4
CYGWIN_RPMREL = @VERSION@
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add

LIBS_SUBPACKAGES = $(top_builddir)/common/common.add
LIBS_SUBPACKAGES += libs.add
LIBS_SUBPACKAGES += ../gcc/rpm-install.add
LIBS_SUBPACKAGES += $(top_builddir)/common/clean.add
LIBS_SUBPACKAGES += target-libs.add

$(TARGET)-libs.spec.in: $(LIBS_SUBPACKAGES) Makefile.am
	cat $(LIBS_SUBPACKAGES) | sed \
	  -e "s/[@]CYGWIN_VERS[@]/$(CYGWIN_VERS)/g" \
	  -e "s/[@]CYGWIN_RPMREL[@]/$(CYGWIN_RPMREL)/g" \
	  -e "s,%{gcc_rpmvers},%{cygwin_rpmvers},g" \
	> $(TARGET)-libs.spec.in
CLEANFILES += $(TARGET)-libs.spec.in
	                      
@rpmprefix@$(TARGET)-libs.spec: $(TARGET)-libs.spec.in
	$(MKSPEC) $(srcdir)/$(TARGET)-libs.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-libs.spec
noinst_DATA += @rpmprefix@$(TARGET)-libs.spec


W32API_VERS = 3.6-1
W32API_RPMREL = @VERSION@
EXTRA_DIST += target-w32api.add
EXTRA_DIST += w32api.add

W32API_SUBPACKAGES = $(top_builddir)/common/common.add
W32API_SUBPACKAGES += w32api.add
W32API_SUBPACKAGES += ../gcc/rpm-install.add
W32API_SUBPACKAGES += $(top_builddir)/common/clean.add
W32API_SUBPACKAGES += target-w32api.add

$(TARGET)-w32api.spec.in: $(W32API_SUBPACKAGES) Makefile.am
	cat $(W32API_SUBPACKAGES) | sed \
	  -e "s/[@]W32API_VERS[@]/$(W32API_VERS)/g" \
	  -e "s/[@]W32API_RPMREL[@]/$(W32API_RPMREL)/g" \
	  -e "s,%{gcc_rpmvers},%{w32api_rpmvers},g" \
	> $(TARGET)-w32api.spec.in
CLEANFILES += $(TARGET)-w32api.spec.in
	                      
@rpmprefix@$(TARGET)-w32api.spec: $(TARGET)-w32api.spec.in
	$(MKSPEC) $(srcdir)/$(TARGET)-w32api.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-w32api.spec
noinst_DATA += @rpmprefix@$(TARGET)-w32api.spec




BINUTILS_VERS = 20050610-1
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
	  > $(TARGET)-binutils.spec.in
CLEANFILES += $(TARGET)-binutils.spec.in

@rpmprefix@$(TARGET)-binutils.spec: $(TARGET)-binutils.spec.in
	$(MKSPEC) $(srcdir)/$(TARGET)-binutils.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-binutils.spec
noinst_DATA += @rpmprefix@$(TARGET)-binutils.spec


GCC_VERS = 3.4.4-1
GCC_RPMREL = @VERSION@
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
GCC_SUBPACKAGES += ../gcc/target-gfortran.add ../gcc/base-gfortran.add
GCC_SUBPACKAGES += ../gcc/target-objc.add

$(TARGET)-gcc.spec.in: $(GCC_SUBPACKAGES) Makefile.am gcc-sources.add gcc-prep.add
	cat $(GCC_SUBPACKAGES) | sed \
	  -e "/[@]SOURCES[@]/r ./gcc-sources.add" \
	  -e "/[@]SOURCES[@]/d" \
	  -e "/[@]PREP[@]/r ./gcc-prep.add" \
	  -e "/[@]PREP[@]/d" \
	  -e "s/[@]GCC_VERS[@]/$(GCC_VERS)/g" \
	  -e "s/[@]GCC_RPMREL[@]/$(GCC_RPMREL)/g" \
	  -e "/[@]tool_target[@]-newlib/d" \
	  -e "/^BuildRequ.*[@]tool_target[@]-w32api-sys-root/d" \
	  -e "s,[@]tool_target[@]-w32api-sys-root,@tool_target@-w32api," \
	  -e 's,^%define build_newlib.*$$,%define build_newlib 0,' \
	  > $(TARGET)-gcc.spec.in
CLEANFILES += $(TARGET)-gcc.spec.in

@rpmprefix@$(TARGET)-gcc.spec: $(TARGET)-gcc.spec.in
	$(MKSPEC) $(srcdir)/$(TARGET)-gcc.spec.in > $@
CLEANFILES += @rpmprefix@$(TARGET)-gcc.spec
noinst_DATA += @rpmprefix@$(TARGET)-gcc.spec
