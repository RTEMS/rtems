# $Id$

TARGET = i386-pc-netbsd4.0.1

include $(top_srcdir)/mkspec.am

NETBSD_VERS = 4.0.1
NETBSD_RPMREL = @VERSION@
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add

LIBS_SUBPACKAGES = $(top_srcdir)/common/common.add
LIBS_SUBPACKAGES += $(srcdir)/libs.add
LIBS_SUBPACKAGES += $(top_srcdir)/gcc/rpm-install.add
LIBS_SUBPACKAGES += $(top_srcdir)/common/clean.add
LIBS_SUBPACKAGES += $(srcdir)/target-libs.add

$(TARGET)-libs.spec.in: $(LIBS_SUBPACKAGES) Makefile.am
	cat $(LIBS_SUBPACKAGES) | sed \
	  -e "s/[@]NETBSD_VERS[@]/$(NETBSD_VERS)/g" \
	  -e "s/[@]NETBSD_RPMREL[@]/$(NETBSD_RPMREL)/g" \
	  -e "s,%{gcc_rpmvers},%{netbsd_rpmvers},g" \
	> $(TARGET)-libs.spec.in
CLEANFILES += $(TARGET)-libs.spec.in
	                      
@rpmprefix@$(TARGET)-libs.spec: $(TARGET)-libs.spec.in
	$(MKSPEC) $(TARGET)-libs.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-libs.spec
noinst_DATA += @rpmprefix@$(TARGET)-libs.spec


BINUTILS_VERS = 2.19
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = @VERSION@%{?dist}
include ../binutils.am
BINUTILS_OPTS +=

GCC_VERS = 4.3.3
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = @VERSION@%{?dist}
include ../gcc.am
GCC_OPTS += --languages=cxx,f95,objc
