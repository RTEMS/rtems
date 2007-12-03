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

include_bspdir = $(includedir)/bsp

include_HEADERS += include/coverhd.h
include_bsp_HEADERS = irq/irq.h

EXTRA_DIST = times

dist_project_lib_DATA += startup/linkcmds

noinst_PROGRAMS += pclock.rel
pclock_rel_SOURCES = clock/p_clock.c
pclock_rel_CPPFLAGS = $(AM_CPPFLAGS)
pclock_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bsppredriverhook.c \
    startup/bspstart.c ../../shared/bootcard.c startup/iss555.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c startup/start.S
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += tm27supp.rel
tm27supp_rel_SOURCES = startup/tm27supp.c
tm27supp_rel_CPPFLAGS = $(AM_CPPFLAGS)
tm27supp_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD = pclock.rel console.rel startup.rel tm27supp.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/console-generic.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/exceptions.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/irq.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc5xx/vectors.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
