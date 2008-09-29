##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

include_HEADERS += include/coverhd.h
include_bsp_HEADERS = include/mbx.h include/commproc.h include/8xx_immap.h \
    irq/irq.h vectors/vectors.h

EXTRA_DIST = times-mbx821 times-mbx860

dist_project_lib_DATA += startup/linkcmds

noinst_LIBRARIES = libbsp.a

pclock_SOURCES = clock/p_clock.c
console_SOURCES = console/console.c
irq_SOURCES = irq/irq.c irq/irq_asm.S irq/irq_init.c
ide_SOURCES = ide/idecfg.c ide/pcmcia_ide.c
vectors_SOURCES = vectors/vectors.h vectors/vectors_init.c \
    vectors/vectors.S
startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c ../../shared/bsppredriverhook.c \
    ../../shared/bsppretaskinghook.c ../../shared/bspgetworkarea.c \
    startup/bspstart.c ../../shared/bootcard.c \
    startup/imbx8xx.c startup/mmutlbtab.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c startup/start.S

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) $(network_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_SOURCES = $(pclock_SOURCES) $(irq_SOURCES) $(ide_SOURCES) \
    $(startup_SOURCES) $(console_SOURCES) $(vectors_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/console-generic.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/cpm.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/mmu.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc8xx/timer.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
