ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = ../../../../../../bsps/sparc/leon3/start/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../../../../../../bsps/sparc/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../../../../../../bsps/sparc/shared/start/linkcmds.base
dist_project_lib_DATA += ../../../../../../bsps/sparc/leon3/start/linkcmds.gr712rc
dist_project_lib_DATA += ../../../../../../bsps/sparc/leon3/start/linkcmds.gr740
dist_project_lib_DATA += ../../../../../../bsps/sparc/leon3/start/linkcmds.leon3
dist_project_lib_DATA += ../../../../../../bsps/sparc/leon3/start/linkcmds.ut699
dist_project_lib_DATA += ../../../../../../bsps/sparc/leon3/start/linkcmds.ut700

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/bspclean.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/bspstart.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/getentropy/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/start/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/sbrk.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/setvec.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/spurious.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/bspidle.S
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/bspdelay.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/start/bspreset-empty.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/cpucounter.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/start/bsp_fatal_exit.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/bsp_fatal_halt.c

# gnatsupp
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/gnatsupp/gnatsupp.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/gnatcommon.c
# AMBA bus
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/amba.c

# non-Driver Manager Clock Implementation
librtemsbsp_a_SOURCES +=../../../../../../bsps/sparc/leon3/clock/ckinit.c

# console
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/dev/serial/console-termios.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/console/console.c
# debugio
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/console/printk_support.c

# IRQ
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/eirq.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/irq/irq-shared.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/shared/irq/bsp_isr_handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c

# PCI
librtemsbsp_a_SOURCES +=  ../../../../../../bsps/sparc/shared/pci/pci_memreg_sparc_le.c
librtemsbsp_a_SOURCES +=  ../../../../../../bsps/sparc/shared/pci/pci_memreg_sparc_be.c

# PCI target devices
librtemsbsp_a_SOURCES +=  ../../../../../../bsps/sparc/shared/pci/gr_leon4_n2x.c
librtemsbsp_a_SOURCES +=  ../../../../../../bsps/sparc/shared/pci/gr_cpci_gr740.c

librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/cache.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/btimer/btimer.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/btimer/watchdog.c

if HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/bspsmp.c
endif

if HAS_NETWORKING
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/net/leon_smc91111.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/net/leon_open_eth.c
if !HAS_SMP
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/net/leon_greth.c
endif
endif

if HAS_MP
# shmsupp
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/mpci/addrconv.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/mpci/getcfg.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/mpci/lock.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/mpci/mpisr.c
endif


include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/grlib-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sparc/leon3/headers.am
