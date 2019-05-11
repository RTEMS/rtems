ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/m68k/mrm332/start/bsp_specs



start.$(OBJEXT): ../../../../../../bsps/m68k/mrm332/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mrm332/start/start_c.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mrm332/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspstart-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/m68k/mrm332/clock/ckinit.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mrm332/console/console.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mrm332/console/sci.c
# spurious
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mrm332/start/spinit.c
# timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/mrm332/btimer/btimer.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mrm332/headers.am
