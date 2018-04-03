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
dist_project_lib_DATA += startup/linkcmds.bootp
dist_project_lib_DATA += startup/linkcmds.prom

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += startup/alloc360.c ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/bspgetworkarea.c ../../shared/bspstart.c \
    startup/init68360.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/sbrk.c ../../shared/setvec.c \
    ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/bspreset.c
# clock
libbsp_a_SOURCES += clock/clock.c ../../../shared/clockdrv_shell.h
# console
libbsp_a_SOURCES += console/console.c ../../shared/dummy_printk_support.c
# timer
libbsp_a_SOURCES += timer/timer.c

if HAS_NETWORKING
libbsp_a_SOURCES += network/network.c
endif

libbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/m68kidle.c
libbsp_a_SOURCES += ../../../../../../bsps/m68k/shared/memProbe.c

if HAS_FPSP
include $(srcdir)/../../../../../../bsps/m68k/shared/fpsp.am
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/m68k/gen68360/headers.am
