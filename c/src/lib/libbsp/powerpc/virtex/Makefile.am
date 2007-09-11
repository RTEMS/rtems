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


noinst_PROGRAMS += startup.rel
startup_rel_SOURCES = startup/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/sbrk.c startup/setvec.c \
    ../../shared/gnatinstallhandler.c
startup_rel_CPPFLAGS = $(AM_CPPFLAGS)
startup_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += dlentry.rel
dlentry_rel_SOURCES = dlentry/dlentry.S
dlentry_rel_CPPFLAGS = $(AM_CPPFLAGS)
dlentry_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

noinst_PROGRAMS += bspconsole.rel
bspconsole_rel_SOURCES = console/consolelite.c ../../shared/console.c
bspconsole_rel_CPPFLAGS = $(AM_CPPFLAGS)
bspconsole_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS = include/opbintctrl.h
noinst_PROGRAMS += opbintctrl.rel
opbintctrl_rel_SOURCES = opbintctrl/opbintctrl.c
opbintctrl_rel_CPPFLAGS = $(AM_CPPFLAGS)
opbintctrl_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS += irq/irq.h
noinst_PROGRAMS += irq.rel
irq_rel_SOURCES = irq/irq_init.c ../shared/irq/irq_asm.S 
irq_rel_CPPFLAGS = $(AM_CPPFLAGS)
irq_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

include_bsp_HEADERS += ../../powerpc/shared/vectors/vectors.h
noinst_PROGRAMS += vectors.rel
vectors_rel_SOURCES = ../../powerpc/shared/vectors/vectors.h \
    ../../powerpc/shared/vectors/vectors_init.c \
    ../../powerpc/shared/vectors/vectors.S
vectors_rel_CPPFLAGS = $(AM_CPPFLAGS)
vectors_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)

if HAS_NETWORKING
network_CPPFLAGS = -D__INSIDE_RTEMS_BSD_TCPIP_STACK__
network_CPPFLAGS += -D__BSD_VISIBLE
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/xiltemac.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS)
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

libbsp_a_LIBADD = startup.rel dlentry.rel bspconsole.rel opbintctrl.rel \
	vectors.rel irq.rel
if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/ppc403/tty_drv.rel

EXTRA_DIST += times

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
