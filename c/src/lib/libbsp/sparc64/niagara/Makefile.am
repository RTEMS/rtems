ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

noinst_PROGRAMS =



start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += ../shared/startup/linkcmds

startup_SOURCES = startup/bspclean.c startup/m5op_sparc.S \
    ../../shared/bsppredriverhook.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/bspgetworkarea.c \
    ../../shared/bspstart.c \
    ../../shared/bspreset.c \
    ../shared/startup/setvec.c \
    ../../shared/gnatinstallhandler.c  \
    ../shared/helenos/boot/genarch/balloc.c \
    ../shared/helenos/boot/genarch/ofw.c \
    ../shared/helenos/boot/genarch/ofw_tree.c \
    ../shared/helenos/boot/generic/string.c \
    ../shared/helenos/boot/sparc64/loader/ofwarch.c \
    ../shared/helenos/boot/sparc64/loader/main.c \
    ../shared/helenos/boot/sparc64/loader/ofwasm.S \
    start/bspinit.S \
    ../shared/asm/asm.S

#clock_SOURCES = ../../shared/clock_driver_simidle.c
clock_SOURCES = ../shared/clock/ckinit.c

console_SOURCES = ../shared/console/conscfg.c
console_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console.c
console_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-control.c
console_SOURCES += ../../../../../../bsps/shared/dev/serial/legacy-console-select.c

timer_SOURCES = ../../shared/timerstub.c

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES = $(startup_SOURCES)
librtemsbsp_a_SOURCES += $(clock_SOURCES)
librtemsbsp_a_SOURCES += $(console_SOURCES)
librtemsbsp_a_SOURCES += $(timer_SOURCES)

librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sparc64/niagara/headers.am
