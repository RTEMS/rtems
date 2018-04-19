ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs



noinst_PROGRAMS =


start.$(OBJEXT): start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

rtems_crtn.$(OBJEXT): ../shared/start/rtems_crtn.S
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
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bsp-fdt.c
librtemsbsp_a_SOURCES += ../shared/src/ppc-exc-handler-table.c
librtemsbsp_a_SOURCES += ../shared/src/tictac.c
librtemsbsp_a_SOURCES += ../shared/src/bsp-start-zero.S
librtemsbsp_a_SOURCES += ../shared/startup/bspidle.c
librtemsbsp_a_SOURCES += startup/epapr_hcalls.S
librtemsbsp_a_SOURCES += startup/l1cache.S
librtemsbsp_a_SOURCES += startup/l2cache.S
librtemsbsp_a_SOURCES += startup/mmu.c
librtemsbsp_a_SOURCES += startup/mmu-tlb1.S
librtemsbsp_a_SOURCES += startup/mmu-config.c
librtemsbsp_a_SOURCES += startup/portal.c
librtemsbsp_a_SOURCES += startup/restart.S
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += startup/bspreset.c
librtemsbsp_a_SOURCES += startup/bsprestart.c

# Clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/qoriq/clock/clock-config.c

# Timer
librtemsbsp_a_SOURCES += timer/timer.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../shared/console-termios-init.c
librtemsbsp_a_SOURCES += ../../shared/console-termios.c
librtemsbsp_a_SOURCES += console/uart-bridge-master.c
librtemsbsp_a_SOURCES += console/uart-bridge-slave.c
librtemsbsp_a_SOURCES += console/console-config.c

# RTC
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += rtc/rtc-config.c

# MPCI
librtemsbsp_a_SOURCES += shmsupp/lock.S
librtemsbsp_a_SOURCES += shmsupp/intercom.c
librtemsbsp_a_SOURCES += shmsupp/intercom-mpci.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/e500-mmu.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += network/network.c
librtemsbsp_a_SOURCES += network/if_intercom.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/net/tsec.c
endif

if HAS_SMP
librtemsbsp_a_SOURCES += startup/bspsmp.c
endif

include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/qoriq/headers.am
