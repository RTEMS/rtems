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

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += startup/bspclean.c \
    startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/bspgetworkarea.c ../../shared/sbrk.c \
    ../../shared/setvec.c ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/bspreset.c
# clock
libbsp_a_SOURCES += clock/ckinit.c
# console
libbsp_a_SOURCES += console/console.c ../../shared/dummy_printk_support.c
# timer
libbsp_a_SOURCES += timer/timer.c timer/timerisr.S

libbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/m68k/mvme147/headers.am
