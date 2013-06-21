ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h
include_HEADERS += include/xparameters_dflt.h
if HAS_NETWORKING
include_HEADERS += network/xiltemac.h
endif

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h

include_bsp_HEADERS =

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += include/coverhd.h

include_bsp_HEADERS += ../shared/include/linker-symbols.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

libbspstart_a_SOURCES += ../../powerpc/shared/start/rtems_crti.S
project_lib_DATA += rtems_crti.$(OBJEXT)

# Link commands
project_lib_DATA += startup/linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bspgetworkarea.c \
    ../../shared/bspreset_loop.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/bsppredriverhook.c ../../shared/bsppretaskinghook.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c
libbsp_a_SOURCES += ../shared/src/bsp-start-zero.S

# bspconsole
libbsp_a_SOURCES += console/consolelite.c ../../shared/console.c \
    ../../shared/console_select.c ../../shared/console_control.c \
    ../../shared/console_read.c ../../shared/console_write.c 

# irq
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += include/opbintctrl.h
include_bsp_HEADERS += irq/irq.h
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq_init.c

# clock
libbsp_a_SOURCES += ../shared/clock/clock.c

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
network_CPPFLAGS += -D__BSD_VISIBLE
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/xiltemac.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

# timer
libbsp_a_SOURCES += timer/timer-config.c
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/ppc403/timer.rel

libbsp_a_LIBADD += \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
