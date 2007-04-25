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

nodist_include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_PROGRAMS += startup.rel
include_HEADERS += ../../arm/shared/comm/uart.h
startup_rel_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
    startup/bspstart.c startup/exit.c ../../shared/bootcard.c \
    ../../shared/main.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)


noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/uart.c ../../shared/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)



if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =
libbsp_a_LIBADD =  startup.rel console.rel 
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

libbsp_a_LIBADD +=  \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/@RTEMS_CPU_MODEL@/irq.rel

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
