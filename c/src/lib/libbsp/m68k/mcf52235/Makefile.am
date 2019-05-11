ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/m68k/mcf52235/start/bsp_specs

noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/m68k/mcf52235/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-loop.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf52235/start/init52235.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspstart-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf52235/start/cfinit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf52235/start/bspgetcpuclockspeed.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/m68k/mcf52235/clock/clock.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf52235/console/console.c
# debugio
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf52235/console/debugio.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf52235/btimer/btimer.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache-mcf5223x.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mcf52235/headers.am
