# $Id$

TARGET = i586-pc-freebsd6.1

include $(top_srcdir)/mkspec.am

FREEBSD_VERS = 6.1
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


BINUTILS_VERS = 2.17
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = @VERSION@
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.1.2
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = @VERSION@%{?dist}
include ../gcc.am
GCC_OPTS += --languages=c,cxx,f95,objc
