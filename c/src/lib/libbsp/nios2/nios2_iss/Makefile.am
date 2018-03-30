ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
crtnn.$(OBJEXT): start/crtnn.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT) crtnn.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_CPPFLAGS = $(AM_CPPFLAGS)

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bspreset_loop.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += startup/setvec.c
libbsp_a_SOURCES += ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c

# clock
libbsp_a_SOURCES += clock/clock.c ../../../shared/clockdrv_shell.h
# console
libbsp_a_SOURCES += console/console.c ../../shared/console-polled.c
# timer
libbsp_a_SOURCES += timer/timer.c

# Cache
libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/nios2/nios2_iss/headers.am
