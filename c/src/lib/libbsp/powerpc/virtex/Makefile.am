ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

if HAS_NETWORKING
endif

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =


EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

# Link commands
project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bspgetworkarea.c \
    ../../shared/bspreset_loop.c \
    startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../shared/src/bsp-start-zero.S

# bspconsole
libbsp_a_SOURCES += console/consolelite.c ../../shared/console.c \
    ../../shared/console_select.c ../../shared/console_control.c \
    ../../shared/console_read.c ../../shared/console_write.c 

# irq
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq_init.c

# clock
libbsp_a_SOURCES += ../shared/clock/clock.c

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
network_CPPFLAGS += -D__BSD_VISIBLE
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/xiltemac.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

# timer
libbsp_a_SOURCES += timer/timer-config.c
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/ppc403/timer.rel

libbsp_a_LIBADD += \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/virtex/headers.am
