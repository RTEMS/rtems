ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs


noinst_PROGRAMS =


start.$(OBJEXT): ../shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base
dist_project_lib_DATA += startup/linkcmds.at697f
dist_project_lib_DATA += startup/linkcmds.leon2

project_lib_LIBRARIES = librtemsbsp.a
librtemsbsp_a_SOURCES =

# startup
librtemsbsp_a_SOURCES += ../../shared/bspclean.c
librtemsbsp_a_SOURCES += startup/bsppredriver.c
librtemsbsp_a_SOURCES += ../../shared/bspreset.c
librtemsbsp_a_SOURCES += startup/bspstart.c
librtemsbsp_a_SOURCES += ../shared/startup/bspgetworkarea.c
librtemsbsp_a_SOURCES += ../../shared/bootcard.c
librtemsbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
librtemsbsp_a_SOURCES += ../../shared/sbrk.c
librtemsbsp_a_SOURCES += startup/setvec.c
librtemsbsp_a_SOURCES += startup/spurious.c
librtemsbsp_a_SOURCES += startup/bspidle.c
librtemsbsp_a_SOURCES += startup/bspdelay.c
librtemsbsp_a_SOURCES += ../shared/startup/bsp_fatal_exit.c
librtemsbsp_a_SOURCES += ../shared/startup/bsp_fatal_halt.c

# ISR Handler
librtemsbsp_a_SOURCES += ../shared/cpu.c
librtemsbsp_a_SOURCES += ../shared/irq_asm.S
# gnatsupp
librtemsbsp_a_SOURCES += gnatsupp/gnatsupp.c
librtemsbsp_a_SOURCES += ../shared/gnatcommon.c
# console
librtemsbsp_a_SOURCES += console/console.c
librtemsbsp_a_SOURCES += console/debugputs.c
# clock
librtemsbsp_a_SOURCES += clock/ckinit.c
# IRQ
librtemsbsp_a_SOURCES += ../shared/irq/genirq.c
librtemsbsp_a_SOURCES += ../shared/irq/irq-shared.c
librtemsbsp_a_SOURCES += ../shared/irq/bsp_isr_handler.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/shared/irq/irq-default-handler.c

# AMBA bus
librtemsbsp_a_SOURCES += ../shared/amba/ambapp.c
librtemsbsp_a_SOURCES += ../shared/amba/ambapp_alloc.c
librtemsbsp_a_SOURCES += ../shared/amba/ambapp_count.c
librtemsbsp_a_SOURCES += ../shared/amba/ambapp_depth.c
librtemsbsp_a_SOURCES += ../shared/amba/ambapp_find_by_idx.c
librtemsbsp_a_SOURCES += ../shared/amba/ambapp_freq.c
librtemsbsp_a_SOURCES += ../shared/amba/ambapp_parent.c
librtemsbsp_a_SOURCES += ../shared/amba/ambapp_old.c
librtemsbsp_a_SOURCES += ../shared/amba/ambapp_names.c
librtemsbsp_a_SOURCES += ../shared/amba/ambapp_show.c
librtemsbsp_a_SOURCES += ../shared/amba/ahbstat.c

# Clock Driver and Timer Library
librtemsbsp_a_SOURCES += ../shared/timer/gptimer.c
librtemsbsp_a_SOURCES += ../shared/timer/tlib.c

# PCI
librtemsbsp_a_SOURCES += ../shared/pci/grpci2.c
librtemsbsp_a_SOURCES += ../shared/pci/grpci2dma.c
librtemsbsp_a_SOURCES += ../shared/pci/grpci.c
librtemsbsp_a_SOURCES += ../shared/pci/pcif.c
librtemsbsp_a_SOURCES +=  ../shared/pci/pci_memreg_sparc_le.c
librtemsbsp_a_SOURCES +=  ../shared/pci/pci_memreg_sparc_be.c
librtemsbsp_a_SOURCES +=  pci/at697_pci.c

# PCI target devices
librtemsbsp_a_SOURCES +=  ../shared/pci/gr_701.c
librtemsbsp_a_SOURCES +=  ../shared/pci/gr_rasta_adcdac.c
librtemsbsp_a_SOURCES +=  ../shared/pci/gr_rasta_io.c
librtemsbsp_a_SOURCES +=  ../shared/pci/gr_rasta_spw_router.c
librtemsbsp_a_SOURCES +=  ../shared/pci/gr_rasta_tmtc.c
librtemsbsp_a_SOURCES +=  ../shared/pci/gr_leon4_n2x.c
librtemsbsp_a_SOURCES +=  ../shared/pci/gr_cpci_gr740.c

# B1553BRM
librtemsbsp_a_SOURCES += ../shared/1553/b1553brm.c
librtemsbsp_a_SOURCES += ../shared/1553/b1553rt.c

# GR1553B
librtemsbsp_a_SOURCES += ../shared/1553/gr1553b.c
librtemsbsp_a_SOURCES += ../shared/1553/gr1553bc.c
librtemsbsp_a_SOURCES += ../shared/1553/gr1553bm.c
librtemsbsp_a_SOURCES += ../shared/1553/gr1553rt.c

# CAN
librtemsbsp_a_SOURCES += ../shared/can/occan.c
librtemsbsp_a_SOURCES += ../shared/can/grcan.c

# SpaceWire
if !HAS_SMP
librtemsbsp_a_SOURCES += ../shared/spw/grspw.c
endif
librtemsbsp_a_SOURCES += ../shared/spw/grspw_pkt.c
librtemsbsp_a_SOURCES += ../shared/spw/grspw_router.c

# UART (RAW)
# I2CMST
librtemsbsp_a_SOURCES += ../shared/i2c/i2cmst.c

# SPI
librtemsbsp_a_SOURCES += ../shared/spi/spictrl.c

# TIME
librtemsbsp_a_SOURCES += ../shared/time/spwcuc.c
librtemsbsp_a_SOURCES += ../shared/time/grctm.c

# GPIO
librtemsbsp_a_SOURCES += ../shared/gpio/grgpio.c
librtemsbsp_a_SOURCES += ../shared/gpio/gpiolib.c

# PWM
librtemsbsp_a_SOURCES += ../shared/pwm/grpwm.c

# ADC and DAC
librtemsbsp_a_SOURCES += ../shared/analog/gradcdac.c

# Memory controllers
librtemsbsp_a_SOURCES += ../shared/mem/mctrl.c
# l2cache
librtemsbsp_a_SOURCES += ../shared/l2c/l2c.c
librtemsbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/start/cache.c
# griommu
librtemsbsp_a_SOURCES += ../shared/iommu/griommu.c
# timer
librtemsbsp_a_SOURCES += timer/timer.c

# TM/TC
librtemsbsp_a_SOURCES += ../shared/tmtc/grtc.c
librtemsbsp_a_SOURCES += ../shared/tmtc/grtm.c

# MEMSCRUB
librtemsbsp_a_SOURCES += ../shared/scrub/memscrub.c
# L4STAT
librtemsbsp_a_SOURCES += ../shared/stat/l4stat.c

# Driver Manager
librtemsbsp_a_SOURCES += ../shared/drvmgr/ambapp_bus.c
librtemsbsp_a_SOURCES += ../shared/drvmgr/ambapp_bus_leon2.c
librtemsbsp_a_SOURCES += ../shared/drvmgr/leon2_amba_bus.c
librtemsbsp_a_SOURCES += ../shared/drvmgr/get_resarray_count.c

if HAS_SMP
librtemsbsp_a_SOURCES += ../../shared/bspsmp.c
librtemsbsp_a_SOURCES += ../../shared/bspsmpgetcurrentprocessor.c
endif

if HAS_NETWORKING
librtemsbsp_a_SOURCES += leon_smc91111/leon_smc91111.c
librtemsbsp_a_SOURCES += leon_open_eth/leon_open_eth.c
librtemsbsp_a_SOURCES += ../shared/net/greth.c
librtemsbsp_a_SOURCES += ../shared/net/network_interface_add.c
endif


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/shared/irq-sources.am
include $(srcdir)/../../../../../../bsps/shared/shared-sources.am
include $(srcdir)/../../../../../../bsps/sparc/leon2/headers.am
