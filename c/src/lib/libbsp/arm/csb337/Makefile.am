ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

if ENABLE_LCD
endif

libbsp_a_CPPFLAGS = $(AM_CPPFLAGS)

if ENABLE_UMON
endif

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =


if ENABLE_LCD
endif

EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += startup/linkcmds.csb337
dist_project_lib_DATA += startup/linkcmds.csb637

EXTRA_DIST += README
EXTRA_DIST += README.kit637_v6

noinst_LIBRARIES = libbsp.a

# startup
libbsp_a_SOURCES = \
    ../../shared/bsppredriverhook.c ../../shared/bspgetworkarea.c \
    startup/bspstart.c \
    ../../shared/bspclean.c startup/bspreset.c \
    startup/memmap.c ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/getentropy-cpucounter.c \
    ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../../shared/cpucounterread.c
libbsp_a_SOURCES += ../../shared/cpucounterdiff.c
libbsp_a_SOURCES += startup/pmc.c
libbsp_a_SOURCES += clock/clock.c
libbsp_a_SOURCES += timer/timer.c
# console
libbsp_a_SOURCES += ../../shared/console.c console/uarts.c \
    ../../shared/console_select.c  ../../shared/console_control.c \
    ../../shared/console_read.c ../../shared/console_write.c 
libbsp_a_SOURCES += console/dbgu.c
libbsp_a_SOURCES += console/usart.c
# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c
if ENABLE_LCD
libbsp_a_SOURCES += console/sed1356.c console/fbcons.c
endif
# umon
if ENABLE_UMON
libbsp_a_SOURCES += ../../shared/umon/umonrtemsglue.c  \
    ../../shared/umon/monlib.c ../../shared/umon/tfsDriver.c \
    ../../shared/umon/umoncons.c startup/umonsupp.c
endif
if ENABLE_UMON_CONSOLE
libbsp_a_SOURCES += ../../shared/umon/umoncons.c
endif

# Cache
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../shared/include/arm-cache-l1.h
libbsp_a_SOURCES += ../shared/armv467ar-basic-cache/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/../shared/armv467ar-basic-cache

if HAS_NETWORKING
libbsp_a_SOURCES += network/network.c
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/arm920.rel

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/arm/csb337/headers.am
