ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

noinst_PROGRAMS =

start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/bootcard.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterread.c
librtemsbsp_a_SOURCES += ../../shared/cpucounterdiff.c

# clock
librtemsbsp_a_SOURCES += clock/clockdrv.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c
librtemsbsp_a_SOURCES += console/uart.c
# timer
librtemsbsp_a_SOURCES += timer/timer.c

# irq
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += irq/irq.c
librtemsbsp_a_SOURCES += irq/bsp_irq_asm.S

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/network.c
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/arm/edb7312/headers.am
