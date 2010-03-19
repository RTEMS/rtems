# $Id$

TARGET = i686-pc-mingw32

include $(top_srcdir)/mkspec.am

GMP_VERS = 4.3.2
GMP_RPMREL = 0.20100319.1%{?dist}
include ../gmp.am

MPC_VERS = 0.8.1
MPC_RPMREL = 0.20100319.1%{?dist}
include ../mpc.am

MPFR_VERS = 2.4.1
MPFR_RPMREL = 0.20100319.1%{?dist}
include ../mpfr.am

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

@rpmprefix@$(TARGET)-termcap-helper.spec: $(TARGET)-termcap-helper.spec.in  
	$(MKSPEC) $(TARGET)-termcap-helper.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-termcap-helper.spec
noinst_DATA += @rpmprefix@$(TARGET)-termcap-helper.spec

@rpmprefix@$(TARGET)-w32api-helper.spec: $(TARGET)-w32api-helper.spec.in  
	$(MKSPEC) $(TARGET)-w32api-helper.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-w32api-helper.spec
noinst_DATA += @rpmprefix@$(TARGET)-w32api-helper.spec

@rpmprefix@$(TARGET)-zlib-helper.spec: $(TARGET)-zlib-helper.spec.in  
	$(MKSPEC) $(TARGET)-zlib-helper.spec.in | $(SPECSTRIP) > $@
CLEANFILES += @rpmprefix@$(TARGET)-zlib-helper.spec
noinst_DATA += @rpmprefix@$(TARGET)-zlib-helper.spec

