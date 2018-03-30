ACLOCAL_AMFLAGS = -I ../../../../aclocal

include $(top_srcdir)/../../../../automake/compile.am
include $(top_srcdir)/../../bsp.am

dist_project_lib_DATA = startup/bsp_specs

DISTCLEANFILES = include/bspopts.h

noinst_PROGRAMS =


start.$(OBJEXT): ../../sparc/shared/start/start.S
	$(CPPASCOMPILE) -o $@ -c $<
project_lib_DATA = start.$(OBJEXT)

project_lib_DATA += linkcmds
dist_project_lib_DATA += ../shared/startup/linkcmds.base
dist_project_lib_DATA += startup/linkcmds.at697f
dist_project_lib_DATA += startup/linkcmds.leon2

noinst_LIBRARIES = libbsp.a
libbsp_a_SOURCES =

# startup
libbsp_a_SOURCES += ../../shared/bspclean.c
libbsp_a_SOURCES += startup/bsppredriver.c
libbsp_a_SOURCES += ../../shared/bspreset.c
libbsp_a_SOURCES += startup/bspstart.c
libbsp_a_SOURCES += ../../sparc/shared/startup/bspgetworkarea.c
libbsp_a_SOURCES += ../../shared/bootcard.c
libbsp_a_SOURCES += ../../shared/getentropy-cpucounter.c
libbsp_a_SOURCES += ../../shared/sbrk.c
libbsp_a_SOURCES += startup/setvec.c
libbsp_a_SOURCES += startup/spurious.c
libbsp_a_SOURCES += startup/bspidle.c
libbsp_a_SOURCES += startup/bspdelay.c
libbsp_a_SOURCES += ../../sparc/shared/startup/bsp_fatal_exit.c
libbsp_a_SOURCES += ../../sparc/shared/startup/bsp_fatal_halt.c

# ISR Handler
libbsp_a_SOURCES += ../../sparc/shared/cpu.c
libbsp_a_SOURCES += ../../sparc/shared/irq_asm.S
# gnatsupp
libbsp_a_SOURCES += gnatsupp/gnatsupp.c ../../sparc/shared/gnatcommon.c
# console
libbsp_a_SOURCES += console/console.c console/debugputs.c
# clock
libbsp_a_SOURCES += clock/ckinit.c
libbsp_a_SOURCES += ../../shared/clockdrv_shell.h
# IRQ
libbsp_a_SOURCES += ../../sparc/shared/irq/genirq.c
libbsp_a_SOURCES += \
    ../../sparc/shared/irq/irq-shared.c \
    ../../sparc/shared/irq/bsp_isr_handler.c \
    ../../shared/src/irq-default-handler.c \
    ../../shared/src/irq-generic.c \
    ../../shared/src/irq-info.c \
    ../../shared/src/irq-legacy.c \
    ../../shared/src/irq-server.c \
    ../../shared/src/irq-shell.c

# AMBA bus
libbsp_a_SOURCES += ../../sparc/shared/amba/ambapp.c
libbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_alloc.c
libbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_count.c
libbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_depth.c
libbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_find_by_idx.c
libbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_freq.c
libbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_parent.c
libbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_old.c
libbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_names.c
libbsp_a_SOURCES += ../../sparc/shared/amba/ambapp_show.c
libbsp_a_SOURCES += ../../sparc/shared/amba/ahbstat.c

# Clock Driver and Timer Library
libbsp_a_SOURCES += ../../sparc/shared/timer/gptimer.c
libbsp_a_SOURCES += ../../sparc/shared/timer/tlib.c

# PCI
libbsp_a_SOURCES += ../../sparc/shared/pci/grpci2.c
libbsp_a_SOURCES += ../../sparc/shared/pci/grpci2dma.c
libbsp_a_SOURCES += ../../sparc/shared/pci/grpci.c
libbsp_a_SOURCES += ../../sparc/shared/pci/pcif.c
libbsp_a_SOURCES +=  ../../sparc/shared/pci/pci_memreg_sparc_le.c
libbsp_a_SOURCES +=  ../../sparc/shared/pci/pci_memreg_sparc_be.c
libbsp_a_SOURCES +=  pci/at697_pci.c

# PCI target devices
libbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_701.c
libbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_rasta_adcdac.c
libbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_rasta_io.c
libbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_rasta_spw_router.c
libbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_rasta_tmtc.c
libbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_leon4_n2x.c
libbsp_a_SOURCES +=  ../../sparc/shared/pci/gr_cpci_gr740.c

# B1553BRM
libbsp_a_SOURCES += ../../sparc/shared/1553/b1553brm.c
libbsp_a_SOURCES += ../../sparc/shared/1553/b1553rt.c

# GR1553B
libbsp_a_SOURCES += ../../sparc/shared/1553/gr1553b.c
libbsp_a_SOURCES += ../../sparc/shared/1553/gr1553bc.c
libbsp_a_SOURCES += ../../sparc/shared/1553/gr1553bm.c
libbsp_a_SOURCES += ../../sparc/shared/1553/gr1553rt.c

# CAN
libbsp_a_SOURCES += ../../sparc/shared/can/occan.c \
    ../../sparc/shared/can/grcan.c

# SpaceWire
if !HAS_SMP
libbsp_a_SOURCES += ../../sparc/shared/spw/grspw.c
endif
libbsp_a_SOURCES += ../../sparc/shared/spw/grspw_pkt.c
libbsp_a_SOURCES += ../../sparc/shared/spw/grspw_router.c

# UART (RAW)
# I2CMST
libbsp_a_SOURCES += ../../sparc/shared/i2c/i2cmst.c

# SPI
libbsp_a_SOURCES += ../../sparc/shared/spi/spictrl.c

# TIME
libbsp_a_SOURCES += ../../sparc/shared/time/spwcuc.c
libbsp_a_SOURCES += ../../sparc/shared/time/grctm.c

# GPIO
libbsp_a_SOURCES += ../../sparc/shared/gpio/grgpio.c
libbsp_a_SOURCES += ../../sparc/shared/gpio/gpiolib.c

# PWM
libbsp_a_SOURCES += ../../sparc/shared/pwm/grpwm.c

# ADC and DAC
libbsp_a_SOURCES += ../../sparc/shared/analog/gradcdac.c

# Memory controllers
libbsp_a_SOURCES += ../../sparc/shared/mem/mctrl.c
# l2cache
libbsp_a_SOURCES += ../../sparc/shared/l2c/l2c.c
libbsp_a_SOURCES += ../../../../../../bsps/sparc/leon2/start/cache.c
# griommu
libbsp_a_SOURCES += ../../sparc/shared/iommu/griommu.c
# timer
libbsp_a_SOURCES += timer/timer.c

# TM/TC
libbsp_a_SOURCES += ../../sparc/shared/tmtc/grtc.c
libbsp_a_SOURCES += ../../sparc/shared/tmtc/grtm.c

# MEMSCRUB
libbsp_a_SOURCES += ../../sparc/shared/scrub/memscrub.c
# L4STAT
libbsp_a_SOURCES += ../../sparc/shared/stat/l4stat.c

# Driver Manager
libbsp_a_SOURCES += ../../sparc/shared/drvmgr/ambapp_bus.c
libbsp_a_SOURCES += ../../sparc/shared/drvmgr/ambapp_bus_leon2.c
libbsp_a_SOURCES += ../../sparc/shared/drvmgr/leon2_amba_bus.c
libbsp_a_SOURCES += ../../sparc/shared/drvmgr/get_resarray_count.c

if HAS_SMP
libbsp_a_SOURCES += ../../shared/bspsmp.c
libbsp_a_SOURCES += ../../shared/bspsmpgetcurrentprocessor.c
endif

if HAS_NETWORKING
libbsp_a_SOURCES += leon_smc91111/leon_smc91111.c
libbsp_a_SOURCES += leon_open_eth/leon_open_eth.c
libbsp_a_SOURCES += ../../sparc/shared/net/greth.c
libbsp_a_SOURCES += ../../sparc/shared/net/network_interface_add.c
endif


include $(top_srcdir)/../../../../automake/local.am
include $(srcdir)/../../../../../../bsps/sparc/leon2/headers.am
