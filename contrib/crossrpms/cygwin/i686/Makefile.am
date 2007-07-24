# $Id$

TARGET = i686-pc-cygwin

include $(top_srcdir)/mkspec.am

CYGWIN_VERS = 1.5.24-2
CYGWIN_RPMREL = 0.20070723.1
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


W32API_VERS = 3.8-1
W32API_RPMREL = 0.20070723.1
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
NCURSES_RPMREL = 0.20070723.2
EXTRA_DIST += target-libncurses-devel.add
EXTRA_DIST += libncurses-devel.add

NCURSES_SUBPACKAGES = $(top_srcdir)/common/common.add
NCURSES_SUBPACKAGES += $(srcdir)/libncurses-devel.add
NCURSES_SUBPACKAGES += $(top_srcdir)/gcc/rpm-install.add
NCURSES_SUBPACKAGES += $(top_srcdir)/common/clean.add
NCURSES_SUBPACKAGES += $(srcdir)/target-libncurses-devel.add

$(TARGET)-libncurses-devel.spec.in: $(NCURSES_SUBPACKAGES) Makefile.am
	cat $(NCURSES_SUBPACKAGES) | sed \
	  -e "s/[@]NCURSES_VERS[@]/$(NCURSES_VERS)/g" \
	  -e "s/[@]NCURSES_RPMREL[@]/$(NCURSES_RPMREL)/g" \
	  -e "s,%{gcc_rpmvers},%{libncurses_devel_rpmvers},g" \
	| $(MKSPEC0) > $(TARGET)-libncurses-devel.spec.in
CLEANFILES += $(TARGET)-libncurses-devel.spec.in
	                      
@rpmprefix@$(TARGET)-libncurses-devel.spec: $(TARGET)-libncurses-devel.spec.in
	$(MKSPEC) $(TARGET)-libncurses-devel.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-libncurses-devel.spec
noinst_DATA += @rpmprefix@$(TARGET)-libncurses-devel.spec

ZLIB_VERS = 1.2.3-2
ZLIB_RPMREL = 0.20070723.1
EXTRA_DIST += target-zlib.add
EXTRA_DIST += zlib.add

ZLIB_SUBPACKAGES = $(top_srcdir)/common/common.add
ZLIB_SUBPACKAGES += $(srcdir)/zlib.add
ZLIB_SUBPACKAGES += $(top_srcdir)/gcc/rpm-install.add
ZLIB_SUBPACKAGES += $(top_srcdir)/common/clean.add
ZLIB_SUBPACKAGES += $(srcdir)/target-zlib.add

$(TARGET)-zlib.spec.in: $(ZLIB_SUBPACKAGES) Makefile.am
	cat $(ZLIB_SUBPACKAGES) | sed \
	  -e "s/[@]ZLIB_VERS[@]/$(ZLIB_VERS)/g" \
	  -e "s/[@]ZLIB_RPMREL[@]/$(ZLIB_RPMREL)/g" \
	  -e "s,%{gcc_rpmvers},%{zlib_rpmvers},g" \
	| $(MKSPEC0) > $(TARGET)-zlib.spec.in
CLEANFILES += $(TARGET)-zlib.spec.in
	                      
@rpmprefix@$(TARGET)-zlib.spec: $(TARGET)-zlib.spec.in
	$(MKSPEC) $(TARGET)-zlib.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-zlib.spec
noinst_DATA += @rpmprefix@$(TARGET)-zlib.spec

# Cygwin's binutils' versioning is bizarre
# Check bfd/configure for the real version number
BINUTILS_VERS = 2.17.50
BINUTILS_PKGVERS = 20060817-1
BINUTILS_RPMREL = 0.20070723.1
include ../binutils.am
BINUTILS_OPTS += --disable-infos

GCC_VERS = 3.4.4
GCC_PKGVERS = $(GCC_VERS)
GCC_RPMREL = 0.20070723.1
include ../gcc.am
GCC_OPTS += --languages=c,cxx
