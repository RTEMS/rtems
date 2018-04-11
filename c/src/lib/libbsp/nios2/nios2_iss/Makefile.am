ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs



start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
crtnn.$(OBJEXT): start/crtnn.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT) crtnn.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =
librtemsbsp_a_CPPFLAGS = $(AM_CPPFLAGS)

# startup
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/bspreset_loop.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += startup/setvec.c
librtemsbsp_a_SOURCES += ../../shared/bsppredriverhook.c

# clock
librtemsbsp_a_SOURCES += clock/clock.c
# console
librtemsbsp_a_SOURCES += console/console.c
librtemsbsp_a_SOURCES += ../../shared/console-polled.c
# timer
librtemsbsp_a_SOURCES += timer/timer.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/nios2/nios2_iss/headers.am
