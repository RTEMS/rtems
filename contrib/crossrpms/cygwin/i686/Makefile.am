# $Id$

TARGET = i686-pc-cygwin

include $(top_srcdir)/mkspec.am

CYGWIN_VERS = 1.5.25-11
CYGWIN_RPMREL = 0.20080319.1%{?dist}
EXTRA_DIST += libs.add
EXTRA_DIST += target-libs.add

LIBS_SUBPACKAGES = $(top_srcdir)/common/common.add
LIBS_SUBPACKAGES += $(srcdir)/libs.add
LIBS_SUBPACKAGES += $(top_srcdir)/gcc/rpm-install.add
LIBS_SUBPACKAGES += $(top_srcdir)/common/clean.add
LIBS_SUBPACKAGES += $(srcdir)/target-libs.add

$(TARGET)-libs.spec.in: $(LIBS_SUBPACKAGES) Makefile.am
	cat $(LIBS_SUBPACKAGES) | sed \
	  -e "s/[@]CYGWIN_VERS[@]/$(CYGWIN_VERS)/g" \
	  -e "s/[@]CYGWIN_RPMREL[@]/$(CYGWIN_RPMREL)/g" \
	  -e "s,%{gcc_rpmvers},%{cygwin_rpmvers},g" \
	| $(MKSPEC0) > $(TARGET)-libs.spec.in
CLEANFILES += $(TARGET)-libs.spec.in
	                      
@rpmprefix@$(TARGET)-libs.spec: $(TARGET)-libs.spec.in
	$(MKSPEC) $(TARGET)-libs.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-libs.spec
noinst_DATA += @rpmprefix@$(TARGET)-libs.spec


W32API_VERS = 3.11-1
W32API_RPMREL = 0.20080319.1%{?dist}
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
	| $(MKSPEC0) > $(TARGET)-w32api.spec.in
CLEANFILES += $(TARGET)-w32api.spec.in
	                      
@rpmprefix@$(TARGET)-w32api.spec: $(TARGET)-w32api.spec.in
	$(MKSPEC) $(TARGET)-w32api.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-w32api.spec
noinst_DATA += @rpmprefix@$(TARGET)-w32api.spec

NCURSES_VERS = 5.5-3
NCURSES_RPMREL = 0.20080321.1%{?dist}
include ../ncurses.am

READLINE_VERS = 5.2.12-10
READLINE_RPMREL = 0.20080322.1%{?dist}
include ../readline.am

ZLIB_VERS = 1.2.3-2
ZLIB_RPMREL = 0.20080314.1%{?dist}
include ../zlib.am

GMP_VERS = 4.2.1-1
GMP_RPMREL = 0.20080320.1%{?dist}
include ../gmp.am

MPFR_VERS = 2.2.1-1
MPFR_RPMREL = 0.20080320.1%{?dist}
include ../mpfr.am

# Cygwin's binutils' versioning is bizarre
# Check bfd/configure for the real version number
BINUTILS_VERS = 2.17.50
BINUTILS_PKGVERS = 20060817-1
BINUTILS_RPMREL = 0.20080314.1%{?dist}
include ../binutils.am
BINUTILS_OPTS += --disable-infos

GCC_VERS = 3.4.4
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 0.20080314.1%{?dist}
include ../gcc.am
GCC_OPTS += --languages=c,cxx
GCC_OPTS += --disable-infos
