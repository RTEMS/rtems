#
# top level directory for RTEMS build tree
#
## The target's toplevel Makefile now is in c/src
##
## $Id$
##

ACLOCAL_AMFLAGS = -I aclocal

SUBDIRS = tools make $(BUILD_SUBDIRS) $(HOST_SUBDIRS) $(TARGET_SUBDIRS)

AUTOMAKE_FILES = automake/compile.am automake/host.am automake/lib.am \
    automake/local.am automake/subdirs.am \
    automake/leaf.am automake/force-preinstall.am

ACLOCAL_FILES = aclocal/bsp-alias.m4 aclocal/canonical-host.m4 \
    aclocal/canonical-target-name.m4 aclocal/canonicalize-tools.m4 \
    aclocal/check-bsp-cache.m4 aclocal/check-bsps.m4 aclocal/check-cpu.m4 \
    aclocal/check-cxx.m4 aclocal/check-itron.m4 \
    aclocal/check-multiprocessing.m4 aclocal/check-networking.m4 \
    aclocal/check-newlib.m4 aclocal/check-posix.m4 aclocal/check-rdbg.m4 \
    aclocal/enable-bare.m4 aclocal/enable-cxx.m4 aclocal/enable-gcc28.m4 \
    aclocal/enable-inlines.m4 aclocal/enable-itron.m4 aclocal/enable-libcdir.m4 \
    aclocal/enable-multiprocessing.m4 aclocal/enable-networking.m4 \
    aclocal/enable-posix.m4 aclocal/enable-rdbg.m4 aclocal/enable-rtemsbsp.m4 \
    aclocal/enable-tests.m4 aclocal/gcc-pipe.m4 aclocal/gcc-specs.m4 \
    aclocal/i386-gas-code16.m4 aclocal/multilib.m4 aclocal/path-ksh.m4 \
    aclocal/path-perl.m4 aclocal/prog-cc.m4 aclocal/prog-cxx.m4 \
    aclocal/project-root.m4 aclocal/rtems-debug.m4 \
    aclocal/rtems-test-no-pause.m4 aclocal/rtems-top.m4 \
    aclocal/target.m4 aclocal/tool-paths.m4 aclocal/bspopts.m4 \
    aclocal/ppc.m4 aclocal/bsp-configure.m4

noinst_SCRIPTS = bootstrap

EXTRA_DIST = $(AUTOMAKE_FILES) $(ACLOCAL_FILES) README.configure SUPPORT \
    VERSION LICENSE $(noinst_SCRIPTS)

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/host.am
