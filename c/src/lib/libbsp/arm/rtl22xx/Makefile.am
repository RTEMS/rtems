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

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += \
    ../../shared/bsppredriverhook.c \
    ../../shared/bspgetworkarea.c startup/bspstart.c \
    ../../shared/bspclean.c startup/bspreset.c \
    ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/cpucounterread.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += timer/timer.c
libbsp_a_SOURCES += clock/clockdrv.c
# console
libbsp_a_SOURCES += ../../shared/console.c \
    ../../shared/console_select.c ../../shared/console_control.c \
    ../../shared/console_read.c ../../shared/console_write.c
# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c
# debugio
libbsp_a_SOURCES += console/uart.c

# Cache
libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

if HAS_NETWORKING
libbsp_a_SOURCES += network/network.c
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/rtl22xx/headers.am
