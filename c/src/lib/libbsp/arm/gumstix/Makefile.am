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

noinst_LIBRARIES = libbsp.a

libbsp_a_SOURCES =
libbsp_a_SOURCES += \
  ../../shared/bsppredriverhook.c ../../shared/bspgetworkarea.c \
  ../../shared/bootcard.c ../../shared/sbrk.c \
  ../../shared/getentropy-cpucounter.c \
  ../../shared/gnatinstallhandler.c \
  ../../shared/bspclean.c startup/bspstart.c startup/bspreset.c startup/memmap.c
libbsp_a_SOURCES += ../../shared/cpucounterread.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += clock/clock.c
libbsp_a_SOURCES += timer/timer.c

#console
libbsp_a_SOURCES += console/uarts.c ../../shared/console.c \
  ../../shared/console_select.c  ../../shared/console_control.c \
  ../../shared/console_read.c ../../shared/console_write.c 
libbsp_a_SOURCES += console/ffuart.c
# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c

# Cache
libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

if ON_SKYEYE
libbsp_a_SOURCES += fb/fb.c
if HAS_NETWORKING
libbsp_a_SOURCES += rtl8019/rtl8019.c rtl8019/wd80x3.h
endif
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/arm920.rel

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/gumstix/headers.am
