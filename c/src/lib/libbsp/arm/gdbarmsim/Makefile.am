ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =


start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += \
    ../../shared/bspgetworkarea.c \
    ../../shared/bsppredriverhook.c ../../shared/bspstart.c \
    ../../shared/bspclean.c startup/bspreset.c ../../shared/bootcard.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c startup/syscalls.c
libbsp_a_SOURCES += ../../shared/cpucounterread.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
# console
libbsp_a_SOURCES += ../../shared/console-polled.c console/console-io.c
# clock
libbsp_a_SOURCES += ../../shared/clock_driver_simidle.c
# timer
libbsp_a_SOURCES += ../../shared/timerstub.c
# start hooks
libbsp_a_SOURCES += startup/bspstarthooks.c
libbsp_a_SOURCES += ../shared/startup/bsp-start-memcpy.S

# IRQ
libbsp_a_SOURCES += irq/irq.c
libbsp_a_SOURCES += irq/irq-dispatch.c
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c

# Cache
libbsp_a_SOURCES += ../../../../../../bsps/shared/cache/nocache.c

# Shared
if shared
libbsp_a_SOURCES += ../shared/arm-cp15-set-exception-handler.c
libbsp_a_SOURCES += ../shared/arm-cp15-set-ttb-entries.c
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/gdbarmsim/headers.am
