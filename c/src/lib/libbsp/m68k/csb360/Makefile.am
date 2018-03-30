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
libbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/bspgetworkarea.c \
    ../../shared/bspstart.c startup/init5272.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/bspreset.c
# console
libbsp_a_SOURCES += console/console-io.c ../../shared/console-polled.c

libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/csb360/dev/ckinit.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/csb360/dev/timer.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/csb360/dev/timerisr.S
libbsp_a_SOURCES += ../../../../../../bsps/m68k/csb360/start/idle-mcf5272.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/m68k/csb360/headers.am
