ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

#isr
DISTCLEANFILES = include/bspopts.h


EXTRA_DIST += start/start.S ../shared/liblnk/regs.h
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += ../../shared/bsppredriverhook.c
libbsp_a_SOURCES += ../../shared/bspgetworkarea.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/sbrk.c
# clock
libbsp_a_SOURCES += clock/clockdrv.c
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
# console
libbsp_a_SOURCES += ../../shared/console-polled.c
libbsp_a_SOURCES += console/console-io.c
# timer
libbsp_a_SOURCES += timer/timer.c
#isr
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../shared/irq/vectorexceptions.c
libbsp_a_SOURCES += ../shared/irq/irq.c
libbsp_a_SOURCES += irq/vectorisrs.c
libbsp_a_SOURCES += ../shared/irq/interruptmask.c

libbsp_a_LIBADD  = ../../../libcpu/@RTEMS_CPU@/shared/cache.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/shared/interrupts.rel

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/mips/jmr3904/headers.am
