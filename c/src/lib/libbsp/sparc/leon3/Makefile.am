ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../../sparc/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base
dist_project_lib_DATA += startup/linkcmds.gr712rc
dist_project_lib_DATA += startup/linkcmds.gr740
dist_project_lib_DATA += startup/linkcmds.leon3
dist_project_lib_DATA += startup/linkcmds.ut699
dist_project_lib_DATA += startup/linkcmds.ut700

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += startup/bspclean.c
librtemsbsp_a_SOURCES += ../../shared/bootcard.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += startup/bsppredriver.c
librtemsbsp_a_SOURCES += ../../sparc/shared/startup/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += startup/setvec.c
librtemsbsp_a_SOURCES += startup/spurious.c
librtemsbsp_a_SOURCES += startup/bspidle.S
librtemsbsp_a_SOURCES += startup/bspdelay.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
librtemsbsp_a_SOURCES += startup/cpucounter.c
librtemsbsp_a_SOURCES += ../../sparc/shared/startup/bsp_fatal_exit.c
librtemsbsp_a_SOURCES += startup/bsp_fatal_halt.c

# ISR Handler
librtemsbsp_a_SOURCES += ../../sparc/shared/cpu.c
librtemsbsp_a_SOURCES += ../../sparc/shared/irq_asm.S
# gnatsupp
librtemsbsp_a_SOURCES += gnatsupp/gnatsupp.c
librtemsbsp_a_SOURCES += ../../sparc/shared/gnatcommon.c
# AMBA bus
librtemsbsp_a_SOURCES += amba/amba.c
librtemsbsp_a_SOURCES += ../../sparc/shared/amba/ambapp.c
librtemsbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_alloc.c
librtemsbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_count.c
librtemsbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_depth.c
librtemsbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_find_by_idx.c
librtemsbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_freq.c
librtemsbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_parent.c
librtemsbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_old.c
librtemsbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_names.c
librtemsbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_show.c
librtemsbsp_a_SOURCES += ../../sparc/shared/amba/ahbstat.c

# Clock Driver and Timer Library
librtemsbsp_a_SOURCES += ../../sparc/shared/timer/gptimer.c
librtemsbsp_a_SOURCES += ../../sparc/shared/timer/tlib.c
librtemsbsp_a_SOURCES += ../../sparc/shared/timer/tlib_ckinit.c
# non-Driver Manager Clock Implementation
librtemsbsp_a_SOURCES += clock/ckinit.c

# console
librtemsbsp_a_SOURCES += ../../shared/console-termios.c
librtemsbsp_a_SOURCES += console/console.c
librtemsbsp_a_SOURCES += ../../sparc/shared/uart/cons.c
librtemsbsp_a_SOURCES += ../../sparc/shared/uart/apbuart_cons.c
# debugio
librtemsbsp_a_SOURCES += console/printk_support.c

# IRQ
librtemsbsp_a_SOURCES += startup/eirq.c
librtemsbsp_a_SOURCES += ../../sparc/shared/irq/genirq.c
librtemsbsp_a_SOURCES += ../../sparc/shared/irq/irq-shared.c
librtemsbsp_a_SOURCES += ../../sparc/shared/irq/bsp_isr_handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c

# PCI
librtemsbsp_a_SOURCES += ../../sparc/shared/pci/grpci.c
librtemsbsp_a_SOURCES += ../../sparc/shared/pci/grpci2.c
librtemsbsp_a_SOURCES += ../../sparc/shared/pci/grpci2dma.c
librtemsbsp_a_SOURCES +=  ../../sparc/shared/pci/pcif.c
librtemsbsp_a_SOURCES +=  ../../sparc/shared/pci/pci_memreg_sparc_le.c
librtemsbsp_a_SOURCES +=  ../../sparc/shared/pci/pci_memreg_sparc_be.c

# PCI target devices
librtemsbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_701.c
librtemsbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_rasta_adcdac.c
librtemsbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_rasta_io.c
librtemsbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_rasta_spw_router.c
librtemsbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_rasta_tmtc.c
librtemsbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_tmtc_1553.c
librtemsbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_leon4_n2x.c
librtemsbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_cpci_gr740.c

# B1553BRM
librtemsbsp_a_SOURCES += ../../sparc/shared/1553/b1553brm.c
librtemsbsp_a_SOURCES += ../../sparc/shared/1553/b1553rt.c

# GR1553B
librtemsbsp_a_SOURCES += ../../sparc/shared/1553/gr1553b.c
librtemsbsp_a_SOURCES += ../../sparc/shared/1553/gr1553bc.c
librtemsbsp_a_SOURCES += ../../sparc/shared/1553/gr1553bm.c
librtemsbsp_a_SOURCES += ../../sparc/shared/1553/gr1553rt.c

# CAN
librtemsbsp_a_SOURCES += ../../sparc/shared/can/occan.c
librtemsbsp_a_SOURCES += ../../sparc/shared/can/grcan.c

# SpaceWire
if !HAS_SMP
librtemsbsp_a_SOURCES += ../../sparc/shared/spw/grspw.c
endif
librtemsbsp_a_SOURCES += ../../sparc/shared/spw/grspw_pkt.c
librtemsbsp_a_SOURCES += ../../sparc/shared/spw/grspw_router.c

# UART
librtemsbsp_a_SOURCES += ../../sparc/shared/uart/apbuart_termios.c

# I2CMST
librtemsbsp_a_SOURCES += ../../sparc/shared/i2c/i2cmst.c

# SPI
librtemsbsp_a_SOURCES += ../../sparc/shared/spi/spictrl.c

# TIME
librtemsbsp_a_SOURCES += ../../sparc/shared/time/spwcuc.c
librtemsbsp_a_SOURCES += ../../sparc/shared/time/grctm.c

# GPIO
librtemsbsp_a_SOURCES += ../../sparc/shared/gpio/grgpio.c
librtemsbsp_a_SOURCES += ../../sparc/shared/gpio/gpiolib.c

# PWM
librtemsbsp_a_SOURCES += ../../sparc/shared/pwm/grpwm.c

# ADC and DAC
librtemsbsp_a_SOURCES += ../../sparc/shared/analog/gradcdac.c

# Memory controllers
librtemsbsp_a_SOURCES += ../../sparc/shared/mem/mctrl.c
# l2cache
librtemsbsp_a_SOURCES += ../../sparc/shared/l2c/l2c.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon3/start/cache.c
# griommu
librtemsbsp_a_SOURCES += ../../sparc/shared/iommu/griommu.c
# timer
librtemsbsp_a_SOURCES += timer/timer.c
librtemsbsp_a_SOURCES += timer/watchdog.c

# GR712
librtemsbsp_a_SOURCES += ../../sparc/shared/ascs/grascs.c
librtemsbsp_a_SOURCES += ../../sparc/shared/can/satcan.c
librtemsbsp_a_SOURCES += ../../sparc/shared/can/canmux.c
librtemsbsp_a_SOURCES += ../../sparc/shared/slink/grslink.c

# TM/TC
librtemsbsp_a_SOURCES += ../../sparc/shared/tmtc/grtc.c
librtemsbsp_a_SOURCES += ../../sparc/shared/tmtc/grtm.c

# MEMSCRUB
librtemsbsp_a_SOURCES += ../../sparc/shared/scrub/memscrub.c
# L4STAT
librtemsbsp_a_SOURCES += ../../sparc/shared/stat/l4stat.c
# Driver Manager 
librtemsbsp_a_SOURCES += ../../sparc/shared/drvmgr/ambapp_bus.c
librtemsbsp_a_SOURCES += ../../sparc/shared/drvmgr/ambapp_bus_grlib.c
librtemsbsp_a_SOURCES += ../../sparc/shared/drvmgr/get_resarray_count.c

if HAS_SMP
librtemsbsp_a_SOURCES += startup/bspsmp.c
endif

if HAS_NETWORKING
librtemsbsp_a_SOURCES += leon_smc91111/leon_smc91111.c
librtemsbsp_a_SOURCES += leon_open_eth/leon_open_eth.c
librtemsbsp_a_SOURCES += ../../sparc/shared/net/greth.c
librtemsbsp_a_SOURCES += ../../sparc/shared/net/network_interface_add.c
if !HAS_SMP
librtemsbsp_a_SOURCES += leon_greth/leon_greth.c
endif
endif

if HAS_MP
# shmsupp
librtemsbsp_a_SOURCES += shmsupp/addrconv.c
librtemsbsp_a_SOURCES += shmsupp/getcfg.c
librtemsbsp_a_SOURCES += shmsupp/lock.c
librtemsbsp_a_SOURCES += shmsupp/mpisr.c
endif


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sparc/leon3/headers.am
