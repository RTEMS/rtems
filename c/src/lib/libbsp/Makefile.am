##
## $Id$
##

include $(top_srcdir)/automake/compile.am

SUBDIRS = @libbsp_cpu_subdir@
DIST_SUBDIRS = @libbsp_cpu_subdir@

EXTRA_DIST = MERGE.PROCEDURE bsp.am

# shared
EXTRA_DIST += shared/bootcard.c shared/bspclean.c shared/bsplibc.c \
    shared/bsppost.c shared/console-polled.c shared/console.c \
    shared/gnatinstallhandler.c shared/main.c shared/sbrk.c shared/tod.c \
    shared/tod.h
EXTRA_DIST += shared/vmeUniverse/vmeUniverse.c \
    shared/vmeUniverse/vmeUniverse.h
EXTRA_DIST += shared/include/coverhd.h
EXTRA_DIST += shared/gdbstub/rtems-stub-glue.c

include $(top_srcdir)/automake/subdirs.am
include $(top_srcdir)/automake/local.am
