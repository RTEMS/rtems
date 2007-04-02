##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += include/coverhd.h

dist_project_lib_DATA += startup/linkcmds startup/linkcmds.dl

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/sbrk.c startup/setvec.c \
    ../../shared/gnatinstallhandler.c ../../shared/dummy_printk_support.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += dlentry.rel
dlentry_rel_SOURCES = dlentry/dlentry.S
dlentry_rel_CPPFLAGS = $(AM_CPPFLAGS)
dlentry_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

libbsp_a_LIBADD = startup.rel dlentry.rel

libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/console.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/tty_drv.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/vectors.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/ictrl.rel

EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
