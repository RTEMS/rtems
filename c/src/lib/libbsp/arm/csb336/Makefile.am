ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

libbsp_a_CPPFLAGS = $(AM_CPPFLAGS)

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =


EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += \
    ../../shared/bspgetworkarea.c \
    ../../shared/bsppredriverhook.c startup/bspstart.c ../../shared/bspclean.c \
    ../../shared/bspreset.c startup/memmap.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/cpucounterread.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += clock/clockdrv.c
libbsp_a_SOURCES += timer/timer.c
# console
libbsp_a_SOURCES += console/uart.c
# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../shared/include/arm-cache-l1.h
libbsp_a_SOURCES += ../shared/armv467ar-basic-cache/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../shared/armv467ar-basic-cache

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network.c network/lan91c11x.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/arm920.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/csb336/headers.am
