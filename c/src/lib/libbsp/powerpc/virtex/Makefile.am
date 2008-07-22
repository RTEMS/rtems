##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h 
include_HEADERS += include/tm27.h
include_HEADERS += include/xparameters_dflt.h
if HAS_NETWORKING
include_HEADERS += network/xiltemac.h
endif

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
    ../../shared/bsppredriverhook.c \
    ../../shared/sbrk.c startup/setvec.c \
    ../../shared/gnatinstallhandler.c
dlentry_SOURCES = dlentry/dlentry.S
bspconsole_SOURCES = console/consolelite.c ../../shared/console.c

include_bsp_HEADERS = include/opbintctrl.h
opbintctrl_SOURCES = opbintctrl/opbintctrl.c

include_bsp_HEADERS += irq/irq.h
irq_SOURCES = irq/irq_init.c

include_bsp_HEADERS +=  \
  ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/vectors.h \
  ../../../libcpu/@RTEMS_CPU@/@exceptions@/bspsupport/ppc_exc_bspsupp.h

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
network_CPPFLAGS += -D__BSD_VISIBLE
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/xiltemac.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(dlentry_SOURCES) \
    $(bspconsole_SOURCES) $(opbintctrl_SOURCES) $(vectors_SOURCES) \
    $(irq_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/tty_drv.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
