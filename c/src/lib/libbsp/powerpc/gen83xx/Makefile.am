ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
nodist_include_bsp_HEADERS = ../../shared/include/bootcard.h
DISTCLEANFILES = include/bspopts.h

include_bsp_HEADERS =
include_bsp_HEADERS += ../../shared/include/irq-generic.h
include_bsp_HEADERS += ../../shared/include/irq-info.h
include_bsp_HEADERS += ../../shared/include/u-boot.h
include_bsp_HEADERS += ../../shared/include/utility.h
include_bsp_HEADERS += ../shared/include/tictac.h
include_bsp_HEADERS += ../shared/include/u-boot-board-info.h
include_bsp_HEADERS += include/hwreg_vals.h
include_bsp_HEADERS += include/irq.h
include_bsp_HEADERS += include/tsec-config.h
include_bsp_HEADERS += include/u-boot-config.h

noinst_PROGRAMS =

include_HEADERS += ../../shared/include/coverhd.h

nodist_include_HEADERS += ../../shared/tod.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

libbspstart_a_SOURCES += ../../powerpc/shared/start/rtems_crti.S
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds \
    startup/linkcmds.base \
    startup/linkcmds.mpc8309som \
    startup/linkcmds.mpc8313erdb \
    startup/linkcmds.br_uid \
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
	../shared/startup/bspidle.c \
	startup/bspstart.c \
	../../shared/bspclean.c \
	startup/bspreset.c \
	../../shared/bspgetworkarea.c \
        ../../shared/src/bsp-uboot-board-info.c \
        ../shared/uboot_getenv.c

# clock
libbsp_a_SOURCES += ../shared/clock/clock.c

# irq
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c

# console
libbsp_a_SOURCES += ../../shared/console.c \
        ../../shared/console_select.c \
	console/console-config.c \
        ../../shared/console_read.c \
        ../../shared/console_write.c \
        ../../shared/console_control.c
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

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
