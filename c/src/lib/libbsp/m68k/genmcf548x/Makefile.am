ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h

include_bsp_HEADERS =
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += include/irq.h

DISTCLEANFILES = include/bspopts.h
noinst_PROGRAMS =

include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S

dist_project_lib_DATA = bsp_specs
dist_project_lib_DATA += startup/linkcmds.m5484FireEngine.flash
dist_project_lib_DATA += startup/linkcmds.COBRA5475
dist_project_lib_DATA += startup/linkcmds.m5484FireEngine

project_lib_DATA = start.$(OBJEXT)
project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =
libbsp_a_CPPFLAGS =

# startup
libbsp_a_SOURCES += \
  ../../shared/bspclean.c ../../shared/bspgetworkarea.c \
  ../../shared/bspreset_loop.c \
  ../../shared/bsppredriverhook.c startup/init548x.c startup/bspstart.c \
  ../../shared/bootcard.c ../../shared/sbrk.c ../../shared/setvec.c \
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
libbsp_a_SOURCES += ../../../libcpu/shared/src/cache_manager.c
libbsp_a_SOURCES += ../include/cache_.h
libbsp_a_CPPFLAGS += -I$(srcdir)/include

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) \
    $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/misc.rel \
                  ../../../libcpu/@RTEMS_CPU@/mcf548x/mcdma.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
