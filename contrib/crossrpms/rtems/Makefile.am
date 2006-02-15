# $Id$

# Just testing
TARGET = sparc-rtems4.7

noinst_DATA =
CLEANFILES =

BINUTILS_VERS = 2.16.1
BINUTILS_RPMREL = 0.20060215.0

BINUTILS_SUBPACKAGES = $(top_builddir)/common/common.add
BINUTILS_SUBPACKAGES += ../binutils/binutils.add
BINUTILS_SUBPACKAGES += ../binutils/rpm-install.add
BINUTILS_SUBPACKAGES += $(top_builddir)/common/clean.add
BINUTILS_SUBPACKAGES += ../binutils/target-binutils.add
BINUTILS_SUBPACKAGES += ../binutils/base-binutils.add

$(TARGET)-binutils.spec.in: $(BINUTILS_SUBPACKAGES) binutils-sources.add
	cat $(BINUTILS_SUBPACKAGES) | sed \
	  -e "/[@]SOURCES[@]/r ./binutils-sources.add" \
	  -e "/[@]SOURCES[@]/d" \
	  -e '/^%define _prefix.*$$/d' \
	  -e '/^%define rpmgroup.*$$/d' \
	  -e '/^%define binutils_target.*$$/d' \
	  -e "s,[@]rpmprefix[@],,g" \
	  -e "s,rtems[@]osversion[@],rtems,g" \
	  -e "s,%{rpmgroup},Development/Tools,g" \
	  > $(TARGET)-binutils.spec.in
CLEANFILES += $(TARGET)-binutils.spec.in


$(TARGET)-binutils.spec: $(TARGET)-binutils.spec.in
	sed \
	  -e "s/[@]tool_target[@]/$(TARGET)/g" \
	  -e "s/[@]BINUTILS_VERS[@]/$(BINUTILS_VERS)/g" \
	  -e "s/[@]BINUTILS_RPMREL[@]/$(BINUTILS_RPMREL)/g" \
	  $(srcdir)/$(TARGET)-binutils.spec.in > $@
CLEANFILES += $(TARGET)-binutils.spec
noinst_DATA += $(TARGET)-binutils.spec


GCC_VERS = 4.0.2
GCC_RPMREL = 0.20060215.0

GCC_SUBPACKAGES = $(top_builddir)/common/common.add
GCC_SUBPACKAGES += ../gcc/gccnewlib.add
GCC_SUBPACKAGES += ../gcc/install.add
GCC_SUBPACKAGES += ../gcc/rpm-install.add
GCC_SUBPACKAGES += $(top_builddir)/common/clean.add
GCC_SUBPACKAGES += ../gcc/target-gcc.add
GCC_SUBPACKAGES += ../gcc/base-gcc.add
GCC_SUBPACKAGES += ../gcc/target-c++.add

$(TARGET)-gcc.spec.in: $(GCC_SUBPACKAGES) gcc-sources.add
	cat $(GCC_SUBPACKAGES) | sed \
	  -e "/[@]SOURCES[@]/r ./gcc-sources.add" \
	  -e "/[@]SOURCES[@]/d" \
	  -e '/^%define _prefix.*$$/d' \
	  -e '/^%define rpmgroup.*$$/d' \
	  -e '/^%define gcc_target.*$$/d' \
	  -e "s,[@]rpmprefix[@],,g" \
	  -e "s,rtems[@]osversion[@],rtems,g" \
	  -e "s,%{rpmgroup},Development/Tools,g" \
	  -e "/^%package -n @tool_target@-gcc/d" \
	  -e 's,^%define build_newlib.*$$,%define build_newlib 1,' \
	  > $(TARGET)-gcc.spec.in
CLEANFILES += $(TARGET)-gcc.spec.in

$(TARGET)-gcc.spec: $(TARGET)-gcc.spec.in
	sed \
	  -e "s/[@]tool_target[@]/$(TARGET)/g" \
	  -e "s/[@]GCC_VERS[@]/$(GCC_VERS)/g" \
	  -e "s/[@]GCC_RPMREL[@]/$(GCC_RPMREL)/g" \
	  -e "/@PATCH0@/d" \
	  -e "/@PATCH1@/d" \
	  -e "/@PATCH2@/d" \
	  -e "/@PATCH3@/d" \
	  -e "/@PATCH4@/d" \
	  -e "/@PATCH5@/d" \
	  $(srcdir)/$(TARGET)-gcc.spec.in > $@
CLEANFILES += $(TARGET)-gcc.spec
noinst_DATA += $(TARGET)-gcc.spec
