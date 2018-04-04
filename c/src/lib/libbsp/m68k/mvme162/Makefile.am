ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

## Zilog component header files

start.$(OBJEXT): ../../m68k/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += startup/bspclean.c \
    ../../shared/bsppredriverhook.c \
    startup/bspstart.c ../../shared/bootcard.c  \
    ../../shared/getentropy-cpucounter.c \
    startup/page_table.c \
    ../../shared/bspgetworkarea.c startup/page_table.c \
    ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
# clock
librtemsbsp_a_SOURCES += clock/ckinit.c
# console
librtemsbsp_a_SOURCES += console/console.c
# timer
librtemsbsp_a_SOURCES += timer/timer.c timer/timerisr.S
# tod
librtemsbsp_a_SOURCES += tod/tod.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../mvme167/network/network.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c

if HAS_FPSP
include $(srcdir)/../../../../../../bsps/m68k/shared/fpsp-sources.am
endif


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mvme162/headers.am
