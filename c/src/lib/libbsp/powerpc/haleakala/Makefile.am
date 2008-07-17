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


noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)


noinst_PROGRAMS += dlentry.rel
dlentry_rel_SOURCES = dlentry/dlentry.S
dlentry_rel_CPPFLAGS = $(AM_CPPFLAGS)
dlentry_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS = ../../powerpc/shared/console/uart.h
include_bsp_HEADERS += ../../powerpc/shared/console/consoleIo.h

noinst_PROGRAMS += console.rel
console_rel_SOURCES = ../../powerpc/shared/console/uart.c \
    ../../powerpc/shared/console/console.c
console_rel_CPPFLAGS = $(AM_CPPFLAGS)
console_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS += irq/irq.h \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/vectors.h \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/irq_supp.h \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/ppc_exc_bspsupp.h

noinst_PROGRAMS += irq.rel
irq_rel_SOURCES = irq/irq_init.c irq/irq.c
irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)


## if HAS_NETWORKING
## network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
## noinst_PROGRAMS += network.rel
## network_rel_SOURCES = network/network.c
## network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
## network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
## endif


noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

libbsp_a_LIBADD = startup.rel dlentry.rel console.rel irq.rel

## if HAS_NETWORKING
## libbsp_a_LIBADD += network.rel
## endif

libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/irq_bspsupport.rel \
	../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
	../../../libcpu/@RTEMS_CPU@/ppc403/clock.rel \
	../../../libcpu/@RTEMS_CPU@/ppc403/timer.rel 


EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
