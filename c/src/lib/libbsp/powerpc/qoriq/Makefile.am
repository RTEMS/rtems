ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/powerpc/qoriq/start/bsp_specs



noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/powerpc/qoriq/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

rtems_crti.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/rtems_crti.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crti.$(OBJEXT)

rtems_crtn.$(OBJEXT): ../../../../../../bsps/powerpc/shared/start/rtems_crtn.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA += rtems_crtn.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/powerpc/shared/start/linkcmds.base
dist_project_lib_DATA += ../../../../../../bsps/powerpc/qoriq/start/linkcmds.qoriq_core_0
dist_project_lib_DATA += ../../../../../../bsps/powerpc/qoriq/start/linkcmds.qoriq_core_1
dist_project_lib_DATA += ../../../../../../bsps/powerpc/qoriq/start/linkcmds.qoriq_e500
dist_project_lib_DATA += ../../../../../../bsps/powerpc/qoriq/start/linkcmds.qoriq_e6500_32
dist_project_lib_DATA += ../../../../../../bsps/powerpc/qoriq/start/linkcmds.qoriq_e6500_64

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# Startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspfatal-default.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bsp-fdt.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/exceptions/ppc-exc-handler-table.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/tictac.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/bsp-start-zero.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/start/bspidle.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/start/epapr_hcalls.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/start/l1cache.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/start/l2cache.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/start/mmu.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/start/mmu-tlb1.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/start/mmu-config.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/start/portal.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/start/restart.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/start/bspreset.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/start/bsprestart.c

# Clock
librtemsbsp_a_SOURCES +=../../../../../../bsps/powerpc/qoriq/clock/clock-config.c

# Timer
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/btimer/btimer.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/irq/irq.c

# Console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios-init.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/console/uart-bridge-master.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/console/uart-bridge-slave.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/console/console-config.c

# RTC
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/rtc/rtc-support.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/rtc/rtc-config.c

# MPCI
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/mpci/lock.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/mpci/intercom.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/mpci/intercom-mpci.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/cache/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/mmu/e500-mmu.c

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/net/network.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/net/if_intercom.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/shared/net/tsec.c
endif

if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/powerpc/qoriq/start/bspsmp.c
endif

include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/shared/exceptions-sources.am
include $(srcdir)/../../../../../../bsps/powerpc/qoriq/headers.am
