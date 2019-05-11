ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/sh/gensh4/start/bsp_specs


start.$(OBJEXT): ../../../../../../bsps/sh/gensh4/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/sh/gensh4/start/linkcmds.rom
dist_project_lib_DATA += ../../../../../../bsps/sh/gensh4/start/linkcmds.rom2ram

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shared/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/shared/start/bsphwinit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh4/start/cpu_asm.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh4/start/ispsh7750.c
librtemsbsp_a_SOURCES +=../../../../../../bsps/sh/gensh4/clock/ckinit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh4/btimer/btimer.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh4/console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/printk-dummy.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh4/console/sh4uart.c
# hw_init
librtemsbsp_a_SOURCES += ../../../../../../bsps/sh/gensh4/start/hw_init.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sh/gensh4/headers.am
