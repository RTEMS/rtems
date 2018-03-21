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
dist_project_lib_DATA += startup/linkcmds.tqm8xx
dist_project_lib_DATA += startup/linkcmds.tqm8xx_base

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# irq
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c

# clock
libbsp_a_SOURCES += ../shared/clock/clock.c
libbsp_a_SOURCES += clock/p_clock.c

# console
libbsp_a_SOURCES += console/console.c
# spi
libbsp_a_SOURCES += spi/spi.c
# timer
libbsp_a_SOURCES += timer/timer.c
# startup
libbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    ../shared/startup/bspidle.c \
    startup/bspstart.c startup/bspgetworkarea.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    startup/mmutlbtab.c startup/cpuinit.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/bspreset.c

if HAS_NETWORKING
libbsp_a_SOURCES += network/network_fec.c
libbsp_a_SOURCES += network/network_scc.c
endif

libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/ppc-dec-timer.c

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/console-generic.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/cpm.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/mmu.rel

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions.am
include $(srcdir)/../../../../../../bsps/powerpc/tqm8xx/headers.am
