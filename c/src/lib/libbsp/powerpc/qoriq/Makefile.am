ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am

include_bspdir = $(includedir)/bsp

dist_project_lib_DATA = bsp_specs

include_HEADERS = include/bsp.h
include_HEADERS += include/tm27.h

nodist_include_HEADERS = include/bspopts.h
DISTCLEANFILES = include/bspopts.h

EXTRA_DIST = README

noinst_PROGRAMS =

include_HEADERS += ../../shared/include/coverhd.h

include_bsp_HEADERS = include/irq.h \
	../../shared/include/irq-generic.h \
	../../shared/include/irq-info.h \
	../../shared/include/bootcard.h \
	../../shared/include/utility.h \
	../shared/include/start.h \
	../shared/include/tictac.h \
	../shared/include/linker-symbols.h \
	include/tsec-config.h \
	include/mmu.h \
	include/intercom.h \
	include/uart-bridge.h \
	include/qoriq.h

noinst_LIBRARIES = libbspstart.a
libbspstart_a_SOURCES = start/start.S
project_lib_DATA = start.$(OBJEXT)

libbspstart_a_SOURCES += ../../powerpc/shared/start/rtems_crti.S
project_lib_DATA += rtems_crti.$(OBJEXT)

dist_project_lib_DATA += startup/linkcmds \
	../shared/startup/linkcmds.base \
	startup/linkcmds.qoriq_core_0 \
	startup/linkcmds.qoriq_core_1 \
	startup/linkcmds.qoriq_p1020rdb \
	startup/linkcmds.qoriq_t2080rdb \
	startup/linkcmds.qoriq_t4240rdb

noinst_LIBRARIES += libbsp.a
libbsp_a_SOURCES =

# Startup
libbsp_a_SOURCES += \
	../../shared/bootcard.c \
	../../shared/sbrk.c \
	../../shared/gnatinstallhandler.c \
	../../shared/bspclean.c \
	../../shared/src/bsp-fdt.c \
	../shared/src/ppc-exc-handler-table.c \
	../shared/src/tictac.c \
	../shared/src/bsp-start-zero.S \
	../shared/startup/bspidle.c \
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

libbsp_a_LIBADD = ../../../libcpu/@RTEMS_CPU@/shared/cpuIdent.rel \
	../../../libcpu/@RTEMS_CPU@/shared/cache.rel \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/rtems-cpu.rel \
	../../../libcpu/@RTEMS_CPU@/@exceptions@/exc_bspsupport.rel \
	../../../libcpu/@RTEMS_CPU@/e500/mmu.rel

if HAS_NETWORKING
libbsp_a_SOURCES += network/network.c \
	network/if_intercom.c
libbsp_a_LIBADD += ../../../libcpu/@RTEMS_CPU@/tsec.rel
endif

if HAS_SMP
libbsp_a_SOURCES += startup/bspsmp.c
endif

include $(srcdir)/preinstall.am
include $(top_srcdir)/../../../../automake/local.am
