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

nodist_include_HEADERS += ../../shared/tod.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

libbspstart_a_SOURCES += ../../powerpc/shared/start/rtems_crti.S
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds \
    startup/linkcmds.base \
    startup/linkcmds.mpc8313erdb \
    startup/linkcmds.mpc8349eamds \
    startup/linkcmds.hsc_cm01

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# mpc83xx_regs
libbsp_a_SOURCES += startup/mpc83xx_regs.c

# startup
libbsp_a_SOURCES += ../../shared/bsplibc.c \
	../../shared/bsppost.c \
	../../shared/bootcard.c \
	../../shared/bsppredriverhook.c \
	../../shared/bsppretaskinghook.c \
	../../shared/sbrk.c \
	../../shared/gnatinstallhandler.c \
	../shared/src/tictac.c \
	startup/cpuinit.c \
	startup/bspstart.c \
	../../shared/bspclean.c \
	startup/bspreset.c \
	../../shared/bspgetworkarea.c \
        startup/uboot_support.c \
        ../shared/uboot_getenv.c

# clock
libbsp_a_SOURCES += ../shared/clock/clock.c

include_bsp_HEADERS = include/irq.h \
	include/irq-config.h \
	../../shared/include/irq-generic.h \
	include/hwreg_vals.h \
	../shared/include/u-boot.h \
	../shared/include/tictac.h

# irq
libbsp_a_SOURCES += include/irq.h \
	include/irq-config.h \
	irq/irq.c \
	../../shared/src/irq-generic.c \
	../../shared/src/irq-legacy.c

# console
libbsp_a_SOURCES += console/console.c console/ns16550cfg.c 
# bsp_i2c
libbsp_a_SOURCES += i2c/i2c_init.c 
# bsp_spi
libbsp_a_SOURCES += spi/spi_init.c 

if HAS_NETWORKING
noinst_PROGRAMS += network.rel
network_rel_SOURCES   = network/network.c 
network_rel_CPPFLAGS  = $(AM_CPPFLAGS)
network_rel_CPPFLAGS += -D__INSIDE_RTEMS_BSD_TCPIP_STACK__  -D__BSD_VISIBLE
network_rel_LDFLAGS = $(RTEMS_RELLDFLAGS)
endif

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
	../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/raw_exception.rel \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
	../../../libcpu/@RTEMS_CPU@/mpc6xx/mmu.rel   \
	../../../libcpu/@RTEMS_CPU@/mpc6xx/timer.rel \
	../../../libcpu/@RTEMS_CPU@/mpc83xx/i2c.rel  \
	../../../libcpu/@RTEMS_CPU@/mpc83xx/spi.rel \
	../../../libcpu/@RTEMS_CPU@/mpc83xx/gtm.rel

if HAS_NETWORKING
libbsp_a_LIBADD += network.rel
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/mpc83xx/tsec.rel
endif

EXTRA_DIST = README.mpc8349eamds
EXTRA_DIST += times.mpc8349eamds
### XXX

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
