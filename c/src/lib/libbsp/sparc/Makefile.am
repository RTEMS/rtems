ACLOCAL_AMFLAGS = -I ../../../aclocal

# Descend into the @RTEMS_BSP_FAMILY@ directory
SUBDIRS = @RTEMS_BSP_FAMILY@

EXTRA_DIST =

EXTRA_DIST += shared/gnatcommon.c
EXTRA_DIST += shared/include/gnatcommon.h
EXTRA_DIST += shared/start/start.S

# Interrupt
EXTRA_DIST += shared/irq/irq-shared.c
EXTRA_DIST += shared/irq/bsp_isr_handler.c
EXTRA_DIST += shared/irq/genirq.c
EXTRA_DIST += shared/include/genirq.h

# AMBA Plug&Play bus
EXTRA_DIST += shared/include/ahbstat.h
EXTRA_DIST += shared/include/ambapp.h
EXTRA_DIST += shared/include/ambapp_ids.h
EXTRA_DIST += shared/include/grlib.h
EXTRA_DIST += shared/amba/ahbstat.c
EXTRA_DIST += shared/amba/ambapp.c
EXTRA_DIST += shared/amba/ambapp_alloc.c
EXTRA_DIST += shared/amba/ambapp_count.c
EXTRA_DIST += shared/amba/ambapp_depth.c
EXTRA_DIST += shared/amba/ambapp_find_by_idx.c
EXTRA_DIST += shared/amba/ambapp_freq.c
EXTRA_DIST += shared/amba/ambapp_parent.c
EXTRA_DIST += shared/amba/ambapp_names.c
EXTRA_DIST += shared/amba/ambapp_old.c
EXTRA_DIST += shared/amba/ambapp_show.c

# Clock Driver and Timer Library
EXTRA_DIST += shared/include/gptimer.h
EXTRA_DIST += shared/include/tlib.h
EXTRA_DIST += shared/timer/gptimer.c
EXTRA_DIST += shared/timer/tlib.c
EXTRA_DIST += shared/timer/tlib_ckinit.c

# PCI bus
EXTRA_DIST += shared/include/grpci.h
EXTRA_DIST += shared/include/grpci2.h
EXTRA_DIST += shared/include/pcif.h
EXTRA_DIST += shared/pci/grpci.c
EXTRA_DIST += shared/pci/grpci2.c
EXTRA_DIST += shared/pci/pcif.c
EXTRA_DIST += shared/pci/pci_memreg_sparc_le.c
EXTRA_DIST += shared/pci/pci_memreg_sparc_be.c

# PCI target boards
EXTRA_DIST += shared/include/gr_701.h
EXTRA_DIST += shared/include/gr_rasta_adcdac.h
EXTRA_DIST += shared/include/gr_rasta_io.h
EXTRA_DIST += shared/include/gr_rasta_spw_router.h
EXTRA_DIST += shared/include/gr_rasta_tmtc.h
EXTRA_DIST += shared/include/gr_tmtc_1553.h
EXTRA_DIST += shared/include/gr_leon4_n2x.h
EXTRA_DIST += shared/pci/gr_701.c
EXTRA_DIST += shared/pci/gr_rasta_adcdac.c
EXTRA_DIST += shared/pci/gr_rasta_io.c
EXTRA_DIST += shared/pci/gr_rasta_spw_router.c
EXTRA_DIST += shared/pci/gr_rasta_tmtc.c
EXTRA_DIST += shared/pci/gr_tmtc_1553.c
EXTRA_DIST += shared/pci/gr_leon4_n2x.c

# DEBUG
EXTRA_DIST += shared/include/debug_defs.h

# SpaceWire
EXTRA_DIST += shared/spw/grspw.c
EXTRA_DIST += shared/spw/grspw_router.c
EXTRA_DIST += shared/include/grspw.h
EXTRA_DIST += shared/include/grspw_router.h

# UART
EXTRA_DIST += shared/uart/cons.c
EXTRA_DIST += shared/uart/apbuart_cons.c
EXTRA_DIST += shared/include/cons.h
EXTRA_DIST += shared/include/apbuart_cons.h
EXTRA_DIST += shared/uart/apbuart.c
EXTRA_DIST += shared/include/apbuart.h

# CAN (OC_CAN, GRCAN)
EXTRA_DIST += shared/can/occan.c
EXTRA_DIST += shared/can/grcan.c
EXTRA_DIST += shared/include/occan.h
EXTRA_DIST += shared/include/grcan.h

# MEM
EXTRA_DIST += shared/mem/mctrl.c

# MIL-STD-B1553 (Core1553BRM)
EXTRA_DIST += shared/1553/b1553brm.c
EXTRA_DIST += shared/1553/b1553rt.c
EXTRA_DIST += shared/include/b1553brm.h
EXTRA_DIST += shared/include/b1553rt.h

# MIL-STD-B1553 (GR1553B)
EXTRA_DIST += shared/1553/gr1553b.c
EXTRA_DIST += shared/1553/gr1553bc.c
EXTRA_DIST += shared/1553/gr1553bm.c
EXTRA_DIST += shared/1553/gr1553rt.c
EXTRA_DIST += shared/include/gr1553b.h
EXTRA_DIST += shared/include/gr1553bc.h
EXTRA_DIST += shared/include/gr1553bc_list.h
EXTRA_DIST += shared/include/gr1553bm.h
EXTRA_DIST += shared/include/gr1553rt.h

# I2C-master (I2CMST)
EXTRA_DIST += shared/i2c/i2cmst.c
EXTRA_DIST += shared/include/i2cmst.h

# SPI
EXTRA_DIST += shared/spi/spictrl.c
EXTRA_DIST += shared/include/spictrl.h

# TIME
EXTRA_DIST += shared/time/spwcuc.c
EXTRA_DIST += shared/time/grctm.c
EXTRA_DIST += shared/include/spwcuc.h
EXTRA_DIST += shared/include/grctm.h

# GPIO
EXTRA_DIST += shared/gpio/grgpio.c
EXTRA_DIST += shared/gpio/gpiolib.c
EXTRA_DIST += shared/include/grgpio.h
EXTRA_DIST += shared/include/gpiolib.h

# PWM
EXTRA_DIST += shared/pwm/grpwm.c
EXTRA_DIST += shared/include/grpwm.h

# ADC and DAC
EXTRA_DIST += shared/analog/gradcdac.c
EXTRA_DIST += shared/include/gradcdac.h

# GRETH
EXTRA_DIST += shared/net/greth.c
EXTRA_DIST += shared/include/greth.h

# Network configuration
EXTRA_DIST += shared/net/network_interface_add.c
EXTRA_DIST += shared/include/network_interface_add.h

# Driver Manager
EXTRA_DIST += shared/drvmgr/ambapp_bus.c
EXTRA_DIST += shared/drvmgr/ambapp_bus_grlib.c
EXTRA_DIST += shared/drvmgr/ambapp_bus_leon2.c
EXTRA_DIST += shared/drvmgr/leon2_amba_bus.c
EXTRA_DIST += shared/drvmgr/get_resarray_count.c

EXTRA_DIST += shared/include/drvmgr/ambapp_bus_grlib.h
EXTRA_DIST += shared/include/drvmgr/ambapp_bus.h
EXTRA_DIST += shared/include/drvmgr/leon2_amba_bus.h
EXTRA_DIST += shared/include/drvmgr/bspcommon.h

# GR712
EXTRA_DIST += shared/ascs/grascs.c
EXTRA_DIST += shared/include/grascs.h
EXTRA_DIST += shared/can/satcan.c
EXTRA_DIST += shared/include/satcan.h
EXTRA_DIST += shared/slink/grslink.c
EXTRA_DIST += shared/include/grslink.h

# TMTC
EXTRA_DIST += shared/tmtc/grtc.c
EXTRA_DIST += shared/include/grtc.h
EXTRA_DIST += shared/tmtc/grtm.c
EXTRA_DIST += shared/include/grtm.h

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
