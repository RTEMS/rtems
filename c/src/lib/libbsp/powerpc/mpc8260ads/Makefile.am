ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
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
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
network_CPPFLAGS += -D__BSD_VISIBLE
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network.c network/if_hdlcsubr.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8260/console-generic.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8260/cpm.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8260/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8260/timer.rel
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

EXTRA_DIST += times

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/mpc8260ads/headers.am
