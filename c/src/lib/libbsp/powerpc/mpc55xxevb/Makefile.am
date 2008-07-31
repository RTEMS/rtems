##
## $Id$
##

ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

libcpudir = ../../../libcpu/@RTEMS_CPU@

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =

# Compiler specs
dist_project_lib_DATA = bsp_specs

# Object files
EXTRA_DIST = startup/start.S
start.$(OBJEXT): startup/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

# Link commands
dist_project_lib_DATA += startup/linkcmds startup/linkcmds.memory

# Includes
include_HEADERS = include/bsp.h

nodist_include_HEADERS = include/bspopts.h ../../shared/tod.h
include_bspdir = $(includedir)/bsp
include_bsp_HEADERS = include/mpc55xxevb.h \
    include/irq-config.h \
    ../../shared/include/irq-generic.h \
    ../shared/include/tictac.h

startup_SOURCES = ../../shared/bsplibc.c ../../shared/bsppost.c \
    ../../shared/bootcard.c ../shared/src/tictac.c startup/bspclean.c \
    startup/bspstart.c

clock_SOURCES = ../shared/clock/clock.c

irq_generic_SOURCES = ../../shared/src/irq-generic.c \
    ../../shared/src/irq-legacy.c

tests_SOURCES = tests/tests.c startup/sd-card-init.c

# Network
if HAS_NETWORKING
noinst_PROGRAMS += network.rel
network_rel_SOURCES = network/network.c
network_rel_CPPFLAGS = $(AM_CPPFLAGS) -D__INSIDE_RTEMS_BSD_TCPIP_STACK__ -D__BSD_VISIBLE
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

# BSP library
noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(clock_SOURCES) $(irq_generic_SOURCES) \
    $(tests_SOURCES)

libbsp_a_LIBADD = $(libcpudir)/shared/cpuIdent.rel \
    $(libcpudir)/shared/cache.rel \
    $(libcpudir)/shared/stack.rel \
    $(libcpudir)/@RTEMS_CPU_MODEL@/misc.rel \
    $(libcpudir)/@RTEMS_CPU_MODEL@/irq.rel \
    $(libcpudir)/@RTEMS_CPU_MODEL@/edma.rel \
    $(libcpudir)/@RTEMS_CPU_MODEL@/dspi.rel \
    $(libcpudir)/@RTEMS_CPU_MODEL@/esci.rel \
    $(libcpudir)/@exceptions@/rtems-cpu.rel \
    $(libcpudir)/@exceptions@/raw_exception.rel \
    $(libcpudir)/@exceptions@/exc_bspsupport.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
