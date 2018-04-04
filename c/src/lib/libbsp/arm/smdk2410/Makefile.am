ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/bspidle.c
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += startup/memmap.c
librtemsbsp_a_SOURCES += ../../shared/bootcard.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c

librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterread.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterdiff.c
librtemsbsp_a_SOURCES += timer/timer.c
librtemsbsp_a_SOURCES += clock/clockdrv.c
librtemsbsp_a_SOURCES += clock/support.c
# console
librtemsbsp_a_SOURCES += console/uart.c
librtemsbsp_a_SOURCES += ../../shared/console.c
librtemsbsp_a_SOURCES += ../../shared/console_select.c
librtemsbsp_a_SOURCES += ../../shared/console_read.c
librtemsbsp_a_SOURCES += ../../shared/console_write.c
librtemsbsp_a_SOURCES += ../../shared/console_control.c
# IRQ
librtemsbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-generic.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-info.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-legacy.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-server.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-shell.c
librtemsbsp_a_SOURCES += irq/irq.c
# smc
librtemsbsp_a_SOURCES += smc/smc.c
librtemsbsp_a_SOURCES += smc/smc.h

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cache/cache-cp15.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/arm/shared/cp15/arm920-mmu.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/smdk2410/headers.am
