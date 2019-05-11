ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/m68k/mcf5225x/start/bsp_specs

noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/m68k/mcf5225x/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES = ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-loop.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5225x/start/init5225x.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5225x/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
librtemsbsp_a_SOURCES +=../../../../../../bsps/m68k/mcf5225x/clock/clock.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5225x/console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5225x/console/debugio.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5225x/btimer/btimer.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache-mcf5225x.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mcf5225x/headers.am
