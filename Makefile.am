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
ACLOCAL_FILES += bsp-alias.m4
ACLOCAL_FILES += canonical-host.m4
ACLOCAL_FILES += canonical-target-name.m4
ACLOCAL_FILES += check-bsps.m4
ACLOCAL_FILES += check-cpu.m4
ACLOCAL_FILES += check-custom-bsp.m4
ACLOCAL_FILES += config-subdirs.m4
ACLOCAL_FILES += enable-bare.m4
ACLOCAL_FILES += enable-cxx.m4
ACLOCAL_FILES += enable-inlines.m4
ACLOCAL_FILES += enable-itron.m4
ACLOCAL_FILES += enable-multiprocessing.m4
ACLOCAL_FILES += enable-networking.m4
ACLOCAL_FILES += enable-posix.m4
ACLOCAL_FILES += enable-rdbg.m4
ACLOCAL_FILES += enable-rtemsbsp.m4
ACLOCAL_FILES += enable-rtems-debug.m4
ACLOCAL_FILES += enable-tests.m4
ACLOCAL_FILES += multilib.m4
ACLOCAL_FILES += path-ksh.m4
ACLOCAL_FILES += project-root.m4
ACLOCAL_FILES += quoting.m4
ACLOCAL_FILES += rtems-cpu-subdirs.m4
ACLOCAL_FILES += rtems-top.m4
ACLOCAL_FILES += tool-paths.m4
ACLOCAL_FILES += version.m4

noinst_SCRIPTS = bootstrap

EXTRA_DIST = README.configure SUPPORT \
    VERSION LICENSE $(noinst_SCRIPTS)
EXTRA_DIST += $(AUTOMAKE_FILES)
EXTRA_DIST += $(ACLOCAL_FILES)

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/host.am
