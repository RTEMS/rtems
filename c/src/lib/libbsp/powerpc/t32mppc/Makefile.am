ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/powerpc/t32mppc/start/bsp_specs



noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/powerpc/t32mppc/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/powerpc/shared/start/linkcmds.base

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/exceptions/ppc-exc-handler-table.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/tictac.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/bsp-start-zero.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/t32mppc/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/t32mppc/start/bspreset.c

# Clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/t32mppc/clock/clock-config.c

# Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/btimer/btimer-stub.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/t32mppc/irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/t32mppc/console/console.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c

if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspsmp-dummy.c
endif

include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/t32mppc/headers.am
