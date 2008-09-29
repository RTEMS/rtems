##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += ../../shared/include/coverhd.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -DASM -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
	../../shared/bsppredriverhook.c startup/bspstart.c \
	../../shared/bspclean.c ../../shared/sbrk.c ../../shared/setvec.c \
	../../shared/bootcard.c ../../shared/gnatinstallhandler.c \
	../../shared/bspgetworkarea.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += console.rel
console_rel_SOURCES = console/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_NETWORKING
networkconfig_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += networkconfig.rel
networkconfig_rel_SOURCES = network/networkconfig.c
networkconfig_rel_CPPFLAGS = $(AM_CPPFLAGS) $(networkconfig_CPPFLAGS)
networkconfig_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD = startup.rel console.rel
if HAS_NETWORKING
libbsp_a_LIBADD += networkconfig.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/network.rel
endif

libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/mmu.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/interrupt.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/cache.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/uart.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/clock.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/rtc.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/timer.rel

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
