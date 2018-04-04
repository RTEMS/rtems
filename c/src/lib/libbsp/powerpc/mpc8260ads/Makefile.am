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

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# clock
libbsp_a_SOURCES += ../shared/clock/clock.c

# console
libbsp_a_SOURCES += console/console.c

# irq
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c \
    ../../shared/bsppredriverhook.c \
    ../shared/startup/bspidle.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c startup/cpuinit.c \
    ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bspreset.c

if HAS_NETWORKING
libbsp_a_SOURCES += network/if_hdlcsubr.c
libbsp_a_SOURCES += network/network.c
endif

libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/dev/timer-ppc-dec.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/dev/console-generic.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/brg.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/cp.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/dpram.c
libbsp_a_SOURCES += ../../../../../../bsps/powerpc/mpc8260ads/start/mmu.c


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/mpc8260ads/headers.am
