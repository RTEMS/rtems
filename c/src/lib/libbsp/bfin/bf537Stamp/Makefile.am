ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += ../../shared/include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += ../../shared/include/coverhd.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

libbsp_a_SOURCES += ../../shared/bsplibc.c ../../shared/bsppost.c \
	../../shared/bsppredriverhook.c startup/bspstart.c \
	../../shared/bspclean.c ../../shared/sbrk.c ../../shared/setvec.c \
	../../shared/bootcard.c ../../shared/gnatinstallhandler.c \
	../../shared/bspgetworkarea.c

libbsp_a_SOURCES += console/console.c

if HAS_NETWORKING
networkconfig_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += networkconfig.rel
networkconfig_rel_SOURCES = network/networkconfig.c
networkconfig_rel_CPPFLAGS = $(AM_CPPFLAGS) $(networkconfig_CPPFLAGS)
networkconfig_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD  = ../../../libcpu/@RTEMS_CPU@/mmu.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/interrupt.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/cache.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/uart.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/clock.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/rtc.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/timer.rel

if HAS_NETWORKING
libbsp_a_LIBADD += networkconfig.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
