ACLOCAL_AMFLAGS = -I ../../../../aclocal
EXTRA_DIST =

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

EXTRA_DIST += README

noinst_PROGRAMS =


EXTRA_DIST += start/start.S
start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crti.S
rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

EXTRA_DIST += ../../powerpc/shared/start/rtems_crtn.S
rtems_crtn.$(OBJEXT): ../../powerpc/shared/start/rtems_crtn.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crtn.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base
dist_project_lib_DATA += startup/linkcmds.qoriq_core_0
dist_project_lib_DATA += startup/linkcmds.qoriq_core_1
dist_project_lib_DATA += startup/linkcmds.qoriq_e500
dist_project_lib_DATA += startup/linkcmds.qoriq_e6500_32
dist_project_lib_DATA += startup/linkcmds.qoriq_e6500_64

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# Startup
libbsp_a_SOURCES += \
	../../shared/bootcard.c \
	../../shared/getentropy-cpucounter.c \
	../../shared/sbrk.c \
	../../shared/gnatinstallhandler.c \
	../../shared/bspclean.c \
	../../shared/src/bsp-fdt.c \
	../shared/src/ppc-exc-handler-table.c \
	../shared/src/tictac.c \
	../shared/src/bsp-start-zero.S \
	../shared/startup/bspidle.c \
	startup/epapr_hcalls.S \
	startup/l1cache.S \
	startup/l2cache.S \
	startup/mmu.c \
	startup/mmu-tlb1.S \
	startup/mmu-config.c \
	startup/portal.c \
	startup/restart.S \
	startup/bsppredriverhook.c \
	startup/bspstart.c \
	startup/bspreset.c \
	startup/bsprestart.c

# Clock
libbsp_a_SOURCES += clock/clock-config.c \
	../../shared/clockdrv_shell.h

# Timer
libbsp_a_SOURCES += timer/timer.c

# IRQ
libbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
libbsp_a_SOURCES += ../../shared/src/irq-generic.c
libbsp_a_SOURCES += ../../shared/src/irq-info.c
libbsp_a_SOURCES += ../../shared/src/irq-legacy.c
libbsp_a_SOURCES += ../../shared/src/irq-server.c
libbsp_a_SOURCES += ../../shared/src/irq-shell.c
libbsp_a_SOURCES += irq/irq.c

# Console
libbsp_a_SOURCES += ../../shared/console-termios-init.c
libbsp_a_SOURCES += ../../shared/console-termios.c
libbsp_a_SOURCES += console/uart-bridge-master.c
libbsp_a_SOURCES += console/uart-bridge-slave.c
libbsp_a_SOURCES += console/console-config.c

# RTC
libbsp_a_SOURCES += ../../shared/tod.c \
	rtc/rtc-config.c

# MPCI
libbsp_a_SOURCES += shmsupp/lock.S \
	shmsupp/intercom.c \
	shmsupp/intercom-mpci.c

libbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
	../../../libcpu/@RTEMS_CPU@/e500/mmu.rel

if HAS_NETWORKING
libbsp_a_SOURCES += network/network.c \
	network/if_intercom.c
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/tsec.rel
endif

if HAS_SMP
libbsp_a_SOURCES += startup/bspsmp.c
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared.am
include $(srcdir)/../../../../../../bsps/powerpc/qoriq/headers.am
