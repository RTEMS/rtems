ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/m68k/genmcf548x/start/bsp_specs

noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/m68k/genmcf548x/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/m68k/genmcf548x/start/linkcmds.COBRA5475
dist_project_lib_DATA += ../../../../../../bsps/m68k/genmcf548x/start/linkcmds.m5484FireEngine
dist_project_lib_DATA += ../../../../../../bsps/m68k/genmcf548x/start/linkcmds.m5484FireEngine.flash

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-loop.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/start/init548x.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/m68k/genmcf548x/clock/clock.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/console/console.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/btimer/btimer.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-info.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-legacy.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-lock.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-server.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-shell.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/irq/irq.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/irq/intc-icr-init-values.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/mcdma/MCD_dmaApi.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/mcdma/MCD_tasks.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/mcdma/MCD_tasksInit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/mcdma/mcdma_glue.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/start/cache.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/net/network.c
endif

include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/genmcf548x/headers.am
