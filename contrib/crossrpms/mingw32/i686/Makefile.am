# $Id$

TARGET = i686-pc-mingw32

include $(top_srcdir)/mkspec.am

MINGW32_VERS = 3.11
MINGW32_RPMREL = 1
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add

LIBS_SUBPACKAGES = $(top_srcdir)/common/common.add
LIBS_SUBPACKAGES += $(srcdir)/libs.add
LIBS_SUBPACKAGES += $(top_srcdir)/gcc/rpm-install.add
LIBS_SUBPACKAGES += $(top_srcdir)/common/clean.add
LIBS_SUBPACKAGES += $(srcdir)/target-libs.add

$(TARGET)-libs.spec.in: $(LIBS_SUBPACKAGES) Makefile.am
	cat $(LIBS_SUBPACKAGES) | sed \
	  -e "s/[@]MINGW32_VERS[@]/$(MINGW32_VERS)/g" \
	  -e "s/[@]MINGW32_RPMREL[@]/$(MINGW32_RPMREL)/g" \
	  -e "s,%{gcc_rpmvers},%{mingw32_rpmvers},g" \
	> $(TARGET)-libs.spec.in
CLEANFILES += $(TARGET)-libs.spec.in

@rpmprefix@$(TARGET)-libs.spec: $(TARGET)-libs.spec.in
	$(MKSPEC) $(TARGET)-libs.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-libs.spec
noinst_DATA += @rpmprefix@$(TARGET)-libs.spec

W32API_VERS = 3.8
W32API_RPMREL = 1
EXTRA_DIST += target-w32api.add
EXTRA_DIST += w32api.add

W32API_SUBPACKAGES = $(top_srcdir)/common/common.add
W32API_SUBPACKAGES += $(srcdir)/w32api.add
W32API_SUBPACKAGES += $(top_srcdir)/gcc/rpm-install.add
W32API_SUBPACKAGES += $(top_srcdir)/common/clean.add
W32API_SUBPACKAGES += $(srcdir)/target-w32api.add

$(TARGET)-w32api.spec.in: $(W32API_SUBPACKAGES) Makefile.am
	cat $(W32API_SUBPACKAGES) | sed \
	  -e "s/[@]W32API_VERS[@]/$(W32API_VERS)/g" \
	  -e "s/[@]W32API_RPMREL[@]/$(W32API_RPMREL)/g" \
	  -e "s,%{gcc_rpmvers},%{w32api_rpmvers},g" \
	> $(TARGET)-w32api.spec.in
CLEANFILES += $(TARGET)-w32api.spec.in

@rpmprefix@$(TARGET)-w32api.spec: $(TARGET)-w32api.spec.in
	$(MKSPEC) $(TARGET)-w32api.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-w32api.spec
noinst_DATA += @rpmprefix@$(TARGET)-w32api.spec

GMP_VERS = 4.2.4
GMP_RPMREL = 0.20081219.1%{?dist}
include ../gmp.am

MPFR_VERS = 2.3.1
MPFR_RPMREL = 0.20081219.1%{?dist}
include ../mpfr.am

BINUTILS_VERS = 2.16.91-20060119-1
BINUTILS_PKGVERS = $(BINUTILS_VERS)
BINUTILS_RPMREL = 1%{?dist}
include ../binutils.am
BINUTILS_OPTS += --disable-infos

GCC_VERS = 4.1.0
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = @VERSION@%{?dist}
include ../gcc.am
GCC_OPTS += --languages=c

@rpmprefix@$(TARGET)-binutils-helper.spec: $(TARGET)-binutils-helper.spec.in  
	$(MKSPEC) $(TARGET)-binutils-helper.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-binutils-helper.spec
noinst_DATA += @rpmprefix@$(TARGET)-binutils-helper.spec

@rpmprefix@$(TARGET)-curses-helper.spec: $(TARGET)-curses-helper.spec.in  
	$(MKSPEC) $(TARGET)-curses-helper.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-curses-helper.spec
noinst_DATA += @rpmprefix@$(TARGET)-curses-helper.spec

@rpmprefix@$(TARGET)-expat-helper.spec: $(TARGET)-expat-helper.spec.in  
	$(MKSPEC) $(TARGET)-expat-helper.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-expat-helper.spec
noinst_DATA += @rpmprefix@$(TARGET)-expat-helper.spec

@rpmprefix@$(TARGET)-gcc-helper.spec: $(TARGET)-gcc-helper.spec.in  
	$(MKSPEC) $(TARGET)-gcc-helper.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-gcc-helper.spec
noinst_DATA += @rpmprefix@$(TARGET)-gcc-helper.spec

@rpmprefix@$(TARGET)-libs-helper.spec: $(TARGET)-libs-helper.spec.in  
	$(MKSPEC) $(TARGET)-libs-helper.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-libs-helper.spec
noinst_DATA += @rpmprefix@$(TARGET)-libs-helper.spec

@rpmprefix@$(TARGET)-readline-helper.spec: $(TARGET)-readline-helper.spec.in  
	$(MKSPEC) $(TARGET)-readline-helper.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-readline-helper.spec
noinst_DATA += @rpmprefix@$(TARGET)-readline-helper.spec

@rpmprefix@$(TARGET)-w32api-helper.spec: $(TARGET)-w32api-helper.spec.in  
	$(MKSPEC) $(TARGET)-w32api-helper.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-w32api-helper.spec
noinst_DATA += @rpmprefix@$(TARGET)-w32api-helper.spec

@rpmprefix@$(TARGET)-zlib-helper.spec: $(TARGET)-zlib-helper.spec.in  
	$(MKSPEC) $(TARGET)-zlib-helper.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-zlib-helper.spec
noinst_DATA += @rpmprefix@$(TARGET)-zlib-helper.spec
