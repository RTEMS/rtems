ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

## these are the include files used by the boot process from HelenOS Sparc64
## These include files mimic the HelenOS kernel include layout, which 
## in HelenOS-0.4.2 is more complicated than the boot include dirs.

# assumed by the HelenOS sources to be in the root includedir. We place them
# in a kernel include directory.


DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =



EXTRA_DIST += ../shared/start/start.S 
start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += ../shared/startup/linkcmds

startup_SOURCES = ../../shared/bspclean.c \
    ../../shared/bspstart.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/bspgetworkarea.c \
    ../../shared/bsppredriverhook.c \
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
    ../shared/asm/asm.S \
    start/bspinit.S

mmu_SOURCES = \
    ../shared/helenos/kernel/sparc64/src/cache.S \
    ../shared/helenos/kernel/sparc64/src/sun4u/takemmu.S \
    ../shared/start/trap_table.S

#clock_SOURCES = ../../shared/clock_driver_simidle.c
clock_SOURCES = ../shared/clock/ckinit.c

console_SOURCES = ../../shared/console.c  ../shared/console/conscfg.c \
    ../../shared/console_select.c  ../../shared/console_control.c \
    ../../shared/console_read.c ../../shared/console_write.c

timer_SOURCES = ../../shared/timerstub.c

noinst_LIBRARIES = libbsp.a 
libbsp_a_SOURCES = $(startup_SOURCES) $(mmu_SOURCES) $(clock_SOURCES) $(console_SOURCES) \
    $(timer_SOURCES)

libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/sparc64/usiii/headers.am
