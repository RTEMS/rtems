ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds
dist_project_lib_DATA += startup/linkcmdsflash
dist_project_lib_DATA += startup/linkcmdsram

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsppredriverhook.c \
    ../../shared/bspreset_loop.c \
    startup/bspgetcpuclockspeed.c \
    ../../shared/bspgetworkarea.c startup/init5235.c startup/bspstart.c \
    ../../shared/bootcard.c ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/gnatinstallhandler.c \
    startup/copyvectors.c
# clock
librtemsbsp_a_SOURCES += clock/clock.c ../../../shared/clockdrv_shell.h
# console
librtemsbsp_a_SOURCES += console/console.c
# timer
librtemsbsp_a_SOURCES += timer/timer.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/network.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache-mcf5235.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mcf5235/headers.am
