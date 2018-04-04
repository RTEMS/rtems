ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =


start.$(OBJEXT): ../../m68k/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += startup/bspclean.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/bspgetworkarea.c \
    startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    startup/page_table.c \
    ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
# clock
librtemsbsp_a_SOURCES += clock/ckinit.c
# console
librtemsbsp_a_SOURCES += console/console.c console/console-recording.h
# timer
librtemsbsp_a_SOURCES += timer/timer.c timer/timerisr.S

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/network.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/shared/fpsp-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mvme167/headers.am
