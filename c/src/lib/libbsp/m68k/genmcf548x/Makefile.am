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
dist_project_lib_DATA += startup/linkcmds.COBRA5475
dist_project_lib_DATA += startup/linkcmds.m5484FireEngine
dist_project_lib_DATA += startup/linkcmds.m5484FireEngine.flash

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_CPPFLAGS = $(AM_CPPFLAGS)

# startup
libbsp_a_SOURCES += \
  ../../shared/bspclean.c ../../shared/bspgetworkarea.c \
  ../../shared/bspreset_loop.c \
  ../../shared/bsppredriverhook.c startup/init548x.c startup/bspstart.c \
  ../../shared/bootcard.c ../../shared/sbrk.c ../../shared/setvec.c \
  ../../shared/getentropy-cpucounter.c \
  ../../shared/gnatinstallhandler.c
# clock
libbsp_a_SOURCES += clock/clock.c ../../../shared/clockdrv_shell.h
# console
libbsp_a_SOURCES += console/console.c
# timer
libbsp_a_SOURCES += timer/timer.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c
libbsp_a_SOURCES += irq/intc-icr-init-values.c

# Cache
libbsp_a_SOURCES += ../../../../../../bsps/m68k/genmcf548x/start/cache.c

if HAS_NETWORKING
libbsp_a_SOURCES += network/network.c
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/misc.rel \
                  ../../../libcpu/@RTEMS_CPU@/mcf548x/mcdma.rel

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/m68k/genmcf548x/headers.am
