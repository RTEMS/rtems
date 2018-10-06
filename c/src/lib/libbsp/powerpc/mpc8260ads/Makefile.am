ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/powerpc/mpc8260ads/start/bsp_specs


noinst_PROGRAMS =

start.$(OBJEXT): ../../../../../../bsps/powerpc/mpc8260ads/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

rtems_crti.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/shared/clock/clock.c

# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/console/console.c

# irq
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/irq/irq.c

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/bspidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/cpuinit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/net/if_hdlcsubr.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/net/network.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/btimer/btimer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/dev/console-generic.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/brg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/cp.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/dpram.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/mmu.c


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/mpc8260ads/headers.am
