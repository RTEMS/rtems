ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h
include_HEADERS += include/at91rm9200_dbgu.h
include_HEADERS += include/at91rm9200_emac.h
include_HEADERS += include/at91rm9200_gpio.h
include_HEADERS += include/at91rm9200.h
include_HEADERS += include/at91rm9200_mem.h
include_HEADERS += include/at91rm9200_pmc.h
include_HEADERS += include/at91rm9200_usart.h
include_HEADERS += include/bits.h

if ENABLE_LCD
include_HEADERS += include/sed1356.h
endif

include_bsp_HEADERS =
libbsp_a_CPPFLAGS =

if ENABLE_UMON
include_umondir = $(includedir)/umon
include_umon_HEADERS  = ../../shared/umon/cli.h
include_umon_HEADERS += ../../shared/umon/monlib.h
include_umon_HEADERS += ../../shared/umon/tfs.h

include_rtemsdir = $(includedir)/rtems
include_rtems_HEADERS  = ../../shared/umon/umon.h
endif

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =


if ENABLE_LCD
nodist_include_HEADERS += console/sed1356_16bit.h
nodist_include_HEADERS += console/font8x16.h
endif

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += startup/linkcmds
EXTRA_DIST += startup/linkcmds.csb337
EXTRA_DIST += startup/linkcmds.csb637
EXTRA_DIST += README
EXTRA_DIST += README.kit637_v6

noinst_LIBRARIES += libbsp.a

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
include_bsp_HEADERS += ../../shared/include/irq-generic.h \
	../../shared/include/irq-info.h
include_bsp_HEADERS += include/bsp/irq.h
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
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/arm920.rel
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
