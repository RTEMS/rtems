ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += startup/start_c.c \
    ../../shared/bspstart.c startup/bsppredriverhook.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/bspclean.c \
    ../../shared/setvec.c \
    ../../shared/bspgetworkarea.c ../../shared/gnatinstallhandler.c 
libbsp_a_SOURCES += ../../shared/bspreset.c
# clock
libbsp_a_SOURCES += clock/ckinit.c
# console
libbsp_a_SOURCES += console/console.c console/sci.c console/sci.h
# spurious
libbsp_a_SOURCES += spurious/spinit.c
# timer
libbsp_a_SOURCES += timer/timer.c

libbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/mrm332/headers.am
