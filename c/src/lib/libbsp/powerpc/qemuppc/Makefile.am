ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =


EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds

# startup
startup_SOURCES = ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/bspgetworkarea.c \
    ../../shared/bspreset.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/gnatinstallhandler.c \
    startup/cmain.c startup/bspstart.c
# pclock
# clock_SOURCES = ../../shared/clock_driver_simidle.c
clock_SOURCES = ../shared/clock/clock.c
# console
console_SOURCES = ../../shared/console-polled.c console/console-io.c

# irq
irq_SOURCES =
irq_SOURCES += ../../shared/src/irq-default-handler.c
irq_SOURCES += ../../shared/src/irq-generic.c
irq_SOURCES += ../../shared/src/irq-info.c
irq_SOURCES += ../../shared/src/irq-legacy.c
irq_SOURCES += ../../shared/src/irq-server.c
irq_SOURCES += ../../shared/src/irq-shell.c
irq_SOURCES += irq/irq_init.c

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(irq_SOURCES)

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/stack.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/qemuppc/headers.am
