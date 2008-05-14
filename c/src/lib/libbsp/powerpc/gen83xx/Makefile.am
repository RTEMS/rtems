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

nodist_include_HEADERS += ../../shared/tod.h

EXTRA_DIST = start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds \
                         startup/linkcmds.mpc8349eamds \
                         startup/linkcmds.hsc_cm01

mpc83xx_regs_SOURCES = startup/mpc83xx_regs.c
startup_SOURCES = ../../shared/bspclean.c ../../shared/bsplibc.c \
    ../../shared/bsppost.c startup/bspstart.c ../../shared/bootcard.c \
    ../../shared/bsppredriverhook.c \
    ../../shared/sbrk.c ../../shared/gnatinstallhandler.c startup/cpuinit.c
pclock_SOURCES = ../../powerpc/shared/clock/p_clock.c

include_bsp_HEADERS = ./irq/irq.h \
    ../../powerpc/shared/vectors/vectors.h
vectors_SOURCES = ../../powerpc/shared/vectors/vectors.h \
    ../../powerpc/shared/vectors/vectors_init.c \
    ../../powerpc/shared/vectors/vectors.S
irq_SOURCES = ./irq/irq.h ./irq/irq_init.c ./irq/ipic.c \
	../shared/irq/irq_asm.S
console_SOURCES = console/console.c console/ns16550cfg.c 
bsp_i2c_SOURCES   = i2c/i2c_init.c 
bsp_spi_SOURCES   = spi/spi_init.c 

if HAS_NETWORKING
noinst_PROGRAMS += network.rel
network_rel_SOURCES   = network/network.c 
network_rel_CPPFLAGS  = $(AM_CPPFLAGS)
network_rel_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__  -D__BSD_VISIBLE
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES = $(startup_SOURCES) $(pclock_SOURCES) $(console_SOURCES) \
    $(vectors_SOURCES) $(irq_SOURCES) $(mpc83xx_regs_SOURCES) \
    $(bsp_i2c_SOURCES) $(bsp_spi_SOURCES)

libbsp_a_LIBADD = \
    ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
    ../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
    ../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel   \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc6xx/clock.rel \
    ../../../libcpu/@RTEMS_CPU@/mpc83xx/i2c.rel  \
    ../../../libcpu/@RTEMS_CPU@/mpc83xx/spi.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/mpc83xx/tsec.rel
endif

EXTRA_DIST += README.mpc8349eamds
EXTRA_DIST += times.mpc8349eamds
### XXX

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
