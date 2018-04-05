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

rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# clock
librtemsbsp_a_SOURCES += ../shared/clock/clock.c

# console
librtemsbsp_a_SOURCES += console/console.c

# irq
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += irq/irq.c

# startup
librtemsbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    ../shared/startup/bspidle.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c startup/cpuinit.c \
    ../../shared/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/if_hdlcsubr.c
librtemsbsp_a_SOURCES += network/network.c
endif

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/dev/console-generic.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/brg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/cp.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/dpram.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/mmu.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/mpc8260ads/headers.am
