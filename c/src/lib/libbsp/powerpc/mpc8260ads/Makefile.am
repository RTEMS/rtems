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

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds

pclock_SOURCES = clock/p_clock.c
console_SOURCES = console/console.c

include_bsp_HEADERS = irq/irq.h
irq_SOURCES = irq/irq.c irq/irq_init.c irq/irq.h irq/irq_asm.S

include_bsp_HEADERS += vectors/vectors.h
startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppredriverhook.c ../../shared/bsppost.c \
    startup/bspstart.c ../../shared/bootcard.c ../../shared/sbrk.c \
    ../../shared/gnatinstallhandler.c startup/cpuinit.c \
    startup/bspgetworkarea.c ../../shared/bsppretaskinghook.c

vectors_SOURCES = vectors/vectors_init.c vectors/vectors.h \
    vectors/vectors.S

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
network_CPPFLAGS += -D__BSD_VISIBLE
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network.c network/if_hdlcsubr.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(pclock_SOURCES) $(irq_SOURCES) $(startup_SOURCES) \
    $(console_SOURCES) $(vectors_SOURCES)

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8260/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8260/console-generic.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8260/cpm.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8260/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8260/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
