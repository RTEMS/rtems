#
# top level directory for RTEMS build tree
#
## The target's toplevel Makefile now is in c/src
##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/aclocal

SUBDIRS = doc make tools @TARGET_SUBDIRS@

AUTOMAKE_FILES = \
automake/subdirs.am \
automake/local.am \
automake/lib.am

ACLOCAL_FILES += \
aclocal/ar-s.m4 \
aclocal/bsp-alias.m4 \
aclocal/canonical-host.m4 \
aclocal/canonical-target-name.m4 \
aclocal/canonicalize-tools.m4 \
aclocal/check-bsps.m4 \
aclocal/check-cpu.m4 \
aclocal/check-files-in.m4 \
aclocal/check-makefile.m4 \
aclocal/check-newlib.m4 \
aclocal/check-rdbg.m4 \
aclocal/enable-bare.m4 \
aclocal/enable-cxx.m4 \
aclocal/enable-gcc28.m4 \
aclocal/enable-hwapi.m4 \
aclocal/enable-inlines.m4 \
aclocal/enable-libcdir.m4 \
aclocal/enable-multiprocessing.m4 \
aclocal/enable-networking.m4 \
aclocal/enable-posix.m4 \
aclocal/enable-rdbg.m4 \
aclocal/enable-rtemsbsp.m4 \
aclocal/enable-tests.m4 \
aclocal/gcc-pipe.m4 \
aclocal/gcc-specs.m4 \
aclocal/i386-gas-code16.m4 \
aclocal/path-ksh.m4 \
aclocal/path-perl.m4 \
aclocal/prog-cc.m4 \
aclocal/prog-cxx.m4 \
aclocal/project-root.m4 \
aclocal/rtems-top.m4 \
aclocal/sysv-ipc.m4 \
aclocal/target.m4 \
aclocal/tool-paths.m4 \
aclocal/tool-prefix.m4

noinst_SCRIPTS = \
bootstrap

EXTRA_DIST = \
$(AUTOMAKE_FILES) \
$(ACLOCAL_FILES) \
README.configure \
SUPPORT \
VERSION \
LICENSE \
$(noinst_SCRIPTS)

$(ACLOCAL_M4): @MAINTAINER_MODE_TRUE@ $(ACLOCAL_FILES)

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
