# $Id$

TARGET = sparc-sun-solaris2.7

noinst_DATA =
CLEANFILES =

SOLARIS_VERS = 2.7
SOLARIS_RPMREL = 0.20060213.0

LIBS_SUBPACKAGES = $(top_builddir)/common/common.add
LIBS_SUBPACKAGES += libs.add
LIBS_SUBPACKAGES += ../gcc/rpm-install.add
LIBS_SUBPACKAGES += $(top_builddir)/common/clean.add
LIBS_SUBPACKAGES += target-libs.add

$(TARGET)-libs.spec.in: $(LIBS_SUBPACKAGES) Makefile.am
	cat $(LIBS_SUBPACKAGES) | sed \
	  -e "s,[@]_prefix[@],$(prefix),g" \
	  -e "s,[@]rpmprefix[@],$(rpmprefix),g" \
	> $(TARGET)-libs.spec.in
CLEANFILES += $(TARGET)-libs.spec.in
	                      
$(TARGET)-libs.spec: $(TARGET)-libs.spec.in
	sed \
	  -e "s/@tool_target@/$(TARGET)/g" \
	  -e "s/[@]SOLARIS_VERS[@]/$(SOLARIS_VERS)/g" \
	  -e "s/[@]SOLARIS_RPMREL[@]/$(SOLARIS_RPMREL)/g" \
	  $(srcdir)/$(TARGET)-libs.spec.in > $@
CLEANFILES += $(TARGET)-libs.spec
noinst_DATA += $(TARGET)-libs.spec


BINUTILS_VERS = 2.16.1
BINUTILS_RPMREL = 0.20060214.0

BINUTILS_SUBPACKAGES = $(top_builddir)/common/common.add
BINUTILS_SUBPACKAGES += ../binutils/binutils.add
BINUTILS_SUBPACKAGES += ../binutils/rpm-install.add
BINUTILS_SUBPACKAGES += $(top_builddir)/common/clean.add
BINUTILS_SUBPACKAGES += ../binutils/target-binutils.add
BINUTILS_SUBPACKAGES += ../binutils/base-binutils.add

$(TARGET)-binutils.spec.in: $(BINUTILS_SUBPACKAGES) Makefile.am binutils-sources.add
	cat $(BINUTILS_SUBPACKAGES) | sed \
	  -e "/[@]SOURCES[@]/r ./binutils-sources.add" \
	  -e "/[@]SOURCES[@]/d" \
	  -e "s,[@]_prefix[@],$(prefix),g" \
	  -e "s,[@]rpmprefix[@],$(rpmprefix),g" \
	  -e "s,rtems[@]osversion[@],rtems,g" \
	  > $(TARGET)-binutils.spec.in
CLEANFILES += $(TARGET)-binutils.spec.in

$(TARGET)-binutils.spec: $(TARGET)-binutils.spec.in
	sed \
	  -e "s/[@]tool_target[@]/$(TARGET)/g" \
	  -e "s/[@]BINUTILS_VERS[@]/$(BINUTILS_VERS)/g" \
	  -e "s/[@]BINUTILS_RPMREL[@]/$(BINUTILS_RPMREL)/g" \
	  -e "/[@]PATCH/d" \
	  $(srcdir)/$(TARGET)-binutils.spec.in > $@
CLEANFILES += $(TARGET)-binutils.spec
noinst_DATA += $(TARGET)-binutils.spec


GCC_VERS = 4.0.2
GCC_RPMREL = 0.20060213.0

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
	  -e "s,[@]_prefix[@],$(prefix),g" \
	  -e "s,[@]rpmprefix[@],$(rpmprefix),g" \
	  -e "s,rtems[@]osversion[@],rtems,g" \
	  -e "s,[@]tool_target[@]-libc,@tool_target@-sys-root,g" \
	  -e "s,[=<][ ]*%{newlib_version}-%{release},," \
	  -e "/--with-newlib/d" \
	  -e "/^%package -n @rpmprefix@@tool_target@-gcc/d" \
	  -e "s/-n @rpmprefix@@tool_target@-gcc//" \
	  -e 's,^%define build_newlib.*$$,%define build_newlib 0,' \
	  > $(TARGET)-gcc.spec.in
CLEANFILES += $(TARGET)-gcc.spec.in

$(TARGET)-gcc.spec: $(TARGET)-gcc.spec.in
	sed \
	  -e "s/[@]tool_target[@]/$(TARGET)/g" \
	  -e "s/[@]GCC_VERS[@]/$(GCC_VERS)/g" \
	  -e "s/[@]GCC_RPMREL[@]/$(GCC_RPMREL)/g" \
	  -e "/[@]PATCH/d" \
	  $(srcdir)/$(TARGET)-gcc.spec.in > $@
CLEANFILES += $(TARGET)-gcc.spec
noinst_DATA += $(TARGET)-gcc.spec
