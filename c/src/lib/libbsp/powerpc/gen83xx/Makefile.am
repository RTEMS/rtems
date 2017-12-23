ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =


EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base
dist_project_lib_DATA += startup/linkcmds.br_uid
dist_project_lib_DATA += startup/linkcmds.hsc_cm01
dist_project_lib_DATA += startup/linkcmds.mpc8309som
dist_project_lib_DATA += startup/linkcmds.mpc8313erdb
dist_project_lib_DATA += startup/linkcmds.mpc8349eamds
dist_project_lib_DATA += startup/linkcmds.mpc83xx

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += \
	../../shared/bootcard.c \
	../../shared/getentropy-cpucounter.c \
	../../shared/bsppredriverhook.c \
	../../shared/sbrk.c \
	../../shared/gnatinstallhandler.c \
	../shared/src/tictac.c \
	startup/cpuinit.c \
	../shared/startup/bspidle.c \
	startup/bspstart.c \
	../../shared/bspclean.c \
	startup/bspreset.c \
	startup/bsprestart.c \
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
libbsp_a_SOURCES += ../../shared/console-termios-init.c
libbsp_a_SOURCES += ../../shared/console-termios.c
libbsp_a_SOURCES += console/console-config.c

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

EXTRA_DIST += README.mpc8349eamds

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/gen83xx/headers.am
