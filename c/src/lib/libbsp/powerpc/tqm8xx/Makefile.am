ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/powerpc/tqm8xx/start/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/powerpc/tqm8xx/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/powerpc/shared/start/linkcmds.base

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# irq
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/irq/irq.c

# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/shared/clock/clock.c
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/tqm8xx/clock/p_clock.c

# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/console/console.c
# spi
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/spi/spi.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/btimer/btimer.c
# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/bspidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/start/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/start/mmutlbtab.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/start/cpuinit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/net/network_fec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/net/network_scc.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/btimer/btimer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/start/cp.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/start/dpram.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/tqm8xx/start/mmu.c

include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/tqm8xx/headers.am
