##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

libcpudir = ../../../libcpu/@RTEMS_CPU@

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_bspdir = $(includedir)/bsp

include_HEADERS += include/coverhd.h
include_bsp_HEADERS = include/tqm.h include/8xx_immap.h \
    include/irq.h include/irq-config.h \
    ../../shared/include/irq-generic.h\
    $(libcpudir)/@exceptions@/bspsupport/vectors.h \
    $(libcpudir)/@exceptions@/bspsupport/ppc_exc_bspsupp.h

EXTRA_DIST = times-tqm866

EXTRA_DIST += startup/start.S
start.$(OBJEXT): startup/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds.base\
                         startup/linkcmds.tqm8xx

irq_SOURCES = include/irq-config.h \
	irq/irq.c \
	../../shared/src/irq-generic.c \
	../../shared/src/irq-legacy.c

# clock_SOURCES = ../shared/clock/clock.c
p_clock_SOURCES = clock/p_clock.c

console_SOURCES = console/console.c
timer_SOURCES = timer/timer.c
startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
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

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(irq_SOURCES) \
    $(startup_SOURCES) $(console_SOURCES) \
    $(p_clock_SOURCES) $(timer_SOURCES)

libbsp_a_LIBADD = \
    $(libcpudir)/shared/cpuIdent.rel \
    $(libcpudir)/shared/cache.rel \
    $(libcpudir)/@exceptions@/rtems-cpu.rel \
    $(libcpudir)/@exceptions@/raw_exception.rel \
    $(libcpudir)/@exceptions@/exc_bspsupport.rel \
    $(libcpudir)/mpc8xx/console-generic.rel \
    $(libcpudir)/mpc8xx/cpm.rel \
    $(libcpudir)/mpc8xx/clock.rel \
    $(libcpudir)/mpc8xx/mmu.rel \
    $(libcpudir)/mpc8xx/timer.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
