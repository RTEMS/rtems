#
# top level directory for RTEMS build tree
#
## The target's toplevel Makefile now is in c/src
##
## $Id$
##

AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS = -I $(RTEMS_TOPdir)/macros

SUBDIRS = doc make tools $(HOST_SUBDIRS) $(TARGET_SUBDIRS)

AUTOMAKE_FILES = automake/compile.am automake/host.am automake/lib.am \
    automake/local.am automake/multilib.am automake/subdirs.am

ACLOCAL_FILES += macros/bsp-alias.m4 macros/canonical-host.m4 \
    macros/canonical-target-name.m4 macros/canonicalize-tools.m4 \
    macros/check-bsp-cache.m4 macros/check-bsps.m4 macros/check-cpu.m4 \
    macros/check-cxx.m4 macros/check-itron.m4 \
    macros/check-multiprocessing.m4 macros/check-networking.m4 \
    macros/check-newlib.m4 macros/check-posix.m4 macros/check-rdbg.m4 \
    macros/enable-bare.m4 macros/enable-cxx.m4 macros/enable-gcc28.m4 \
    macros/enable-inlines.m4 macros/enable-itron.m4 macros/enable-libcdir.m4 \
    macros/enable-multiprocessing.m4 macros/enable-networking.m4 \
    macros/enable-posix.m4 macros/enable-rdbg.m4 macros/enable-rtemsbsp.m4 \
    macros/enable-tests.m4 macros/gcc-pipe.m4 macros/gcc-specs.m4 \
    macros/i386-gas-code16.m4 macros/multilib.m4 macros/path-ksh.m4 \
    macros/path-perl.m4 macros/prog-cc.m4 macros/prog-cxx.m4 \
    macros/project-root.m4 macros/rtems-debug.m4 \
    macros/rtems-test-no-pause.m4 macros/rtems-top.m4 macros/sysv-ipc.m4 \
    macros/target.m4 macros/tool-paths.m4

noinst_SCRIPTS = bootstrap

EXTRA_DIST = $(AUTOMAKE_FILES) $(ACLOCAL_FILES) README.configure SUPPORT \
    VERSION LICENSE $(noinst_SCRIPTS)

$(ACLOCAL_M4): @MAINTAINER_MODE_TRUE@ $(ACLOCAL_FILES)

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/host.am
