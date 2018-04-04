ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.rom
dist_project_lib_DATA += startup/linkcmds.rom2ram

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += \
    ../../shared/bsppredriverhook.c ../shared/startup/bspstart.c \
    ../../shared/bspgetworkarea.c \
    ../../shared/bspclean.c ../../shared/sbrk.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/gnatinstallhandler.c ../shared/bsphwinit.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
librtemsbsp_a_SOURCES += startup/cpu_asm.c startup/ispsh7750.c
librtemsbsp_a_SOURCES += clock/ckinit.c
librtemsbsp_a_SOURCES += timer/timer.c
# console
librtemsbsp_a_SOURCES += console/console.c ../../shared/dummy_printk_support.c
librtemsbsp_a_SOURCES += console/sh4uart.c
# hw_init
librtemsbsp_a_SOURCES += hw_init/hw_init.c

# Cache
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sh/gensh4/headers.am
