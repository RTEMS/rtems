##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += include/coverhd.h
include_bsp_HEADERS = include/tqm.h include/8xx_immap.h \
    include/irq.h include/irq-config.h \
    ../../shared/include/irq-generic.h\
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/vectors.h \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/ppc_exc_bspsupp.h

EXTRA_DIST = times-tqm866

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = startup/start.S
project_lib_DATA = start.$(OBJEXT)

libbspstart_a_SOURCES += ../../powerpc/shared/start/rtems_crti.S
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds.base \
    startup/linkcmds.tqm8xx

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# irq
libbsp_a_SOURCES += include/irq-config.h \
	irq/irq.c \
	../../shared/src/irq-generic.c \
	../../shared/src/irq-legacy.c

# clock
libbsp_a_SOURCES += ../shared/clock/clock.c
libbsp_a_SOURCES += clock/p_clock.c

# console
libbsp_a_SOURCES += console/console.c
# timer
libbsp_a_SOURCES += timer/timer.c
# startup
libbsp_a_SOURCES += ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bsppredriverhook.c \
    ../../shared/bsppretaskinghook.c startup/bspstart.c \
    startup/bspgetworkarea.c ../../shared/bootcard.c startup/mmutlbtab.c \
    startup/cpuinit.c ../../shared/sbrk.c ../../shared/gnatinstallhandler.c

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network_scc.c network/network_fec.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/console-generic.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/cpm.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/timer.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
