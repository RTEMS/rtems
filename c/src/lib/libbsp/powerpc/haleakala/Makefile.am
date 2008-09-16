##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h 
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_bspdir = $(includedir)/bsp

include_HEADERS += include/coverhd.h

EXTRA_DIST = ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

startup_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/bsppredriverhook.c ../../shared/bspgetworkarea.c \
    ../../shared/bsppretaskinghook.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c

dlentry_SOURCES = dlentry/dlentry.S

include_bsp_HEADERS = ../../powerpc/shared/console/uart.h \
    ../../powerpc/shared/console/consoleIo.h
console_SOURCES = ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c

include_bsp_HEADERS += irq/irq.h \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/vectors.h \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/ppc_exc_bspsupp.h

irq_SOURCES = irq/irq_init.c irq/irq.c

## if HAS_NETWORKING
## network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
## noinst_PROGRAMS += network.rel
## network_rel_SOURCES = network/network.c
## network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
## network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
## endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(dlentry_SOURCES) $(console_SOURCES) \
    $(irq_SOURCES)

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/timer.rel 

## if HAS_NETWORKING
## libbsp_a_LIBADD += network.rel
## endif

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
