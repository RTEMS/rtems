ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# irq
librtemsbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-generic.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-info.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-legacy.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-server.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-shell.c
librtemsbsp_a_SOURCES += irq/irq.c

# clock
librtemsbsp_a_SOURCES += ../shared/clock/clock.c
librtemsbsp_a_SOURCES += clock/p_clock.c

# console
librtemsbsp_a_SOURCES += console/console.c
# spi
librtemsbsp_a_SOURCES += spi/spi.c
# timer
librtemsbsp_a_SOURCES += timer/timer.c
# startup
librtemsbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    ../shared/startup/bspidle.c \
    startup/bspstart.c startup/bspgetworkarea.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    startup/mmutlbtab.c startup/cpuinit.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/network_fec.c
librtemsbsp_a_SOURCES += network/network_scc.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/dev/console-generic.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/start/cp.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/start/dpram.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/start/mmu.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/tqm8xx/headers.am
