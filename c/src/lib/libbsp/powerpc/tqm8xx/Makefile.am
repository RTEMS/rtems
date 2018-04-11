ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# irq
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
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
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../shared/startup/bspidle.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += startup/mmutlbtab.c
librtemsbsp_a_SOURCES += startup/cpuinit.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
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
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/tqm8xx/headers.am
