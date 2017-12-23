ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.rom
dist_project_lib_DATA += startup/linkcmds.rom2ram

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += \
    ../../shared/bsppredriverhook.c ../shared/startup/bspstart.c \
    ../../shared/bspgetworkarea.c \
    ../../shared/bspclean.c ../../shared/sbrk.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/gnatinstallhandler.c ../shared/bsphwinit.c
libbsp_a_SOURCES += ../../shared/bspreset.c
libbsp_a_SOURCES += startup/cpu_asm.c startup/ispsh7750.c
libbsp_a_SOURCES += clock/ckinit.c
libbsp_a_SOURCES += timer/timer.c
# console
libbsp_a_SOURCES += console/console.c ../../shared/dummy_printk_support.c
libbsp_a_SOURCES += console/sh4uart.c
# hw_init
libbsp_a_SOURCES += hw_init/hw_init.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../../shared/include/cache_.h
libbsp_a_CPPFLAGS = $(AM_CPPFLAGS) -I$(srcdir)/../../shared/include

EXTRA_DIST += times

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/sh/gensh4/headers.am
