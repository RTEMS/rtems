##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

nodist_include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

libbspstart_a_SOURCES += ../../powerpc/shared/start/rtems_crti.S
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

# startup
startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../shared/bsppretaskinghook.c \
    ../../shared/bspgetworkarea.c ../../shared/bsppost.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c \
    startup/cmain.c startup/bspstart.c startup/bsppanic.c
# pclock
clock_SOURCES = ../../shared/clock_driver_simidle.c
# console
console_SOURCES = ../../shared/console-polled.c console/console-io.c
#timer
timer_SOURCES = ../../shared/timerstub.c

# irq
include_bsp_HEADERS = irq/irq.h \
    ../../shared/include/irq-generic.h \
    irq/irq-config.h

irq_SOURCES = irq/irq_init.c \
	../../shared/src/irq-generic.c \
	../../shared/src/irq-legacy.c

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(timer_SOURCES) $(irq_SOURCES)

#    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel
include_bsp_HEADERS +=  ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/vectors.h	\
                       ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h	\
                       ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/ppc_exc_bspsupp.h


libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel


EXTRA_DIST = times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
