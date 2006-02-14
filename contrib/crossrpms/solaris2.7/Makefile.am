# $Id$

TARGET = sparc-sun-solaris2.7

BINUTILS_VERS = 2.16.1
BINUTILS_RPMREL = 0.20060214.0

BINUTILS_SUBPACKAGES = $(top_builddir)/common/common.add
BINUTILS_SUBPACKAGES += ../binutils/binutils.add
BINUTILS_SUBPACKAGES += ../binutils/rpm-install.add
BINUTILS_SUBPACKAGES += $(top_builddir)/common/clean.add
BINUTILS_SUBPACKAGES += ../binutils/target-binutils.add
BINUTILS_SUBPACKAGES += ../binutils/base-binutils.add

$(TARGET)-binutils.spec.in: $(BINUTILS_SUBPACKAGES)
	cat $^ | sed \
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
	  -e "s,[@]PATCH0[@],," \
	  $(srcdir)/$(TARGET)-binutils.spec.in > $@
CLEANFILES += $(TARGET)-binutils.spec
noinst_DATA += $(TARGET)-binutils.spec
