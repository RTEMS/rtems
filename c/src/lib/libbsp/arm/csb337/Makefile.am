ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h
if ENABLE_LCD
include_HEADERS += include/sed1356.h
endif

include_bsp_HEADERS =

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

nodist_include_HEADERS += ../../shared/include/coverhd.h

if ENABLE_LCD
nodist_include_HEADERS += console/sed1356_16bit.h
nodist_include_HEADERS += console/font8x16.h
endif

noinst_LIBRARIES = libbspstart.a

libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += startup/linkcmds
EXTRA_DIST = startup/linkcmds.csb337
EXTRA_DIST += startup/linkcmds.csb637
EXTRA_DIST += README
EXTRA_DIST += README.kit637_v6

noinst_LIBRARIES += libbsp.a

# startup
libbsp_a_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/bsppredriverhook.c ../../shared/bspgetworkarea.c \
    ../../shared/bsppretaskinghook.c startup/bspstart.c \
    ../../shared/bspclean.c startup/bspreset.c \
    startup/memmap.c ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c
# console
libbsp_a_SOURCES += ../../shared/console.c console/uarts.c \
    ../../shared/console_select.c  ../../shared/console_control.c \
    ../../shared/console_read.c ../../shared/console_write.c 
# IRQ
include_bsp_HEADERS += ../../shared/include/irq-generic.h \
	../../shared/include/irq-info.h
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
if ENABLE_LCD
libbsp_a_SOURCES += console/sed1356.c console/fbcons.c
endif
# abort
libbsp_a_SOURCES += ../shared/abort/abort.c
# umon
if ENABLE_UMON
libbsp_a_SOURCES += ../../shared/umon/umonrtemsglue.c  \
    ../../shared/umon/monlib.c ../../shared/umon/tfsDriver.c \
    ../../shared/umon/umoncons.c startup/umonsupp.c
endif
if ENABLE_UMON_CONSOLE
libbsp_a_SOURCES += ../../shared/umon/umoncons.c
endif

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/arm920.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/dbgu.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/usart.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/pmc.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/irq.rel
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
