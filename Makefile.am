#
# top level directory for RTEMS build tree
#
## The target's toplevel Makefile now is in c/src
##
## $Id$
##

ACLOCAL_AMFLAGS = -I aclocal

SUBDIRS = make $(build_SUBDIRS) $(host_SUBDIRS) $(target_SUBDIRS)

AUTOMAKE_FILES =
AUTOMAKE_FILES += automake/compile.am 
AUTOMAKE_FILES += automake/host.am
AUTOMAKE_FILES += automake/local.am
AUTOMAKE_FILES += automake/subdirs.am

ACLOCAL_FILES =
ACLOCAL_FILES += aclocal/bsp-alias.m4
ACLOCAL_FILES += aclocal/canonical-target-name.m4
ACLOCAL_FILES += aclocal/check-bsps.m4
ACLOCAL_FILES += aclocal/check-custom-bsp.m4
ACLOCAL_FILES += aclocal/config-subdirs.m4
ACLOCAL_FILES += aclocal/enable-bare.m4
ACLOCAL_FILES += aclocal/enable-cxx.m4
ACLOCAL_FILES += aclocal/enable-inlines.m4
ACLOCAL_FILES += aclocal/enable-itron.m4
ACLOCAL_FILES += aclocal/enable-multiprocessing.m4
ACLOCAL_FILES += aclocal/enable-networking.m4
ACLOCAL_FILES += aclocal/enable-posix.m4
ACLOCAL_FILES += aclocal/enable-rdbg.m4
ACLOCAL_FILES += aclocal/enable-rtemsbsp.m4
ACLOCAL_FILES += aclocal/enable-rtems-debug.m4
ACLOCAL_FILES += aclocal/enable-tests.m4
ACLOCAL_FILES += aclocal/multilib.m4
ACLOCAL_FILES += aclocal/path-ksh.m4
ACLOCAL_FILES += aclocal/project-root.m4
ACLOCAL_FILES += aclocal/quoting.m4
ACLOCAL_FILES += aclocal/rtems-cpu-subdirs.m4
ACLOCAL_FILES += aclocal/rtems-top.m4
ACLOCAL_FILES += aclocal/tool-paths.m4
ACLOCAL_FILES += aclocal/version.m4

noinst_SCRIPTS = bootstrap

EXTRA_DIST = README.configure SUPPORT \
    VERSION LICENSE $(noinst_SCRIPTS)
EXTRA_DIST += $(AUTOMAKE_FILES)
EXTRA_DIST += $(ACLOCAL_FILES)

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/host.am
