ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h


noinst_PROGRAMS =


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

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

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# Startup
librtemsbsp_a_SOURCES += \
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
librtemsbsp_a_SOURCES += clock/clock-config.c \
	../../shared/clockdrv_shell.h

# Timer
librtemsbsp_a_SOURCES += timer/timer.c

# IRQ
librtemsbsp_a_SOURCES += ../../shared/src/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-generic.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-info.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-legacy.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-server.c
librtemsbsp_a_SOURCES += ../../shared/src/irq-shell.c
librtemsbsp_a_SOURCES += irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../shared/console-termios-init.c
librtemsbsp_a_SOURCES += ../../shared/console-termios.c
librtemsbsp_a_SOURCES += console/uart-bridge-master.c
librtemsbsp_a_SOURCES += console/uart-bridge-slave.c
librtemsbsp_a_SOURCES += console/console-config.c

# RTC
librtemsbsp_a_SOURCES += ../../shared/tod.c \
	rtc/rtc-config.c

# MPCI
librtemsbsp_a_SOURCES += shmsupp/lock.S \
	shmsupp/intercom.c \
	shmsupp/intercom-mpci.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/e500-mmu.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/network.c \
	network/if_intercom.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/net/tsec.c
endif

if HAS_SMP
librtemsbsp_a_SOURCES += startup/bspsmp.c
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/qoriq/headers.am
