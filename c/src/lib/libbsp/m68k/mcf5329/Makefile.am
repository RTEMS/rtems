ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/m68k/mcf5329/start/bsp_specs

noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/m68k/mcf5329/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/m68k/mcf5329/start/linkcmdsflash

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-loop.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5329/start/init5329.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5329/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5329/start/cfinit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/m68k/mcf5329/clock/clock.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5329/console/console.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5329/btimer/btimer.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mcf5329/net/network.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache-mcf532x.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mcf5329/headers.am
