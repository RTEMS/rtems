ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/m68k/csb360/start/bsp_specs




start.$(OBJEXT): ../../../../../../bsps/m68k/csb360/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspgetworkarea-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspstart-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/csb360/start/init5272.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/csb360/console/console-io.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-polled.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/csb360/dev/ckinit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/csb360/dev/timer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/csb360/dev/timerisr.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/csb360/start/idle-mcf5272.c

include $(srcdir)/../../../../../../bsps/shared/irq-default-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/csb360/headers.am
