ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs



start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += startup/start_c.c
librtemsbsp_a_SOURCES += ../../shared/bspstart.c
librtemsbsp_a_SOURCES += startup/bsppredriverhook.c
librtemsbsp_a_SOURCES += ../../shared/bootcard.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/setvec.c
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/gnatinstallhandler.c 
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
# clock
librtemsbsp_a_SOURCES += clock/ckinit.c
# console
librtemsbsp_a_SOURCES += console/console.c
librtemsbsp_a_SOURCES += console/sci.c
# spurious
librtemsbsp_a_SOURCES += spurious/spinit.c
# timer
librtemsbsp_a_SOURCES += timer/timer.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mrm332/headers.am
