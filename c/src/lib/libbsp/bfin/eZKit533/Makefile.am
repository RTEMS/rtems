ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =


EXTRA_DIST += ../shared/start/start.S
start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

libbsp_a_SOURCES += \
	startup/bspstart.c \
	../../shared/bspclean.c ../../shared/sbrk.c ../../shared/setvec.c \
	../../shared/bootcard.c ../../shared/gnatinstallhandler.c \
	../../shared/getentropy-cpucounter.c \
	../../shared/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bspreset.c

libbsp_a_SOURCES += console/console-io.c

libbsp_a_LIBADD  = ../../../libcpu/@RTEMS_CPU@/mmu.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/interrupt.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/cache.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/uart.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/clock.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/rtc.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/timer.rel

EXTRA_DIST += times

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/bfin/eZKit533/headers.am
