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

# AMBA Plug&Play bus
EXTRA_DIST += shared/include/ambapp.h
EXTRA_DIST += shared/include/ambapp_ids.h
EXTRA_DIST += shared/include/grlib.h
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
EXTRA_DIST += shared/include/tlib.h
EXTRA_DIST += shared/timer/gptimer.c
EXTRA_DIST += shared/timer/tlib.c
EXTRA_DIST += shared/timer/tlib_ckinit.c

# PCI bus
EXTRA_DIST += shared/include/grpci2.h
EXTRA_DIST += shared/pci/grpci.c
EXTRA_DIST += shared/pci/grpci2.c
EXTRA_DIST += shared/pci/pcif.c
EXTRA_DIST += shared/pci/pci_memreg_sparc_le.c
EXTRA_DIST += shared/pci/pci_memreg_sparc_be.c

# DEBUG
EXTRA_DIST += shared/include/debug_defs.h

# SpaceWire (GRSPW)
EXTRA_DIST += shared/spw/grspw.c
EXTRA_DIST += shared/spw/grspw_pci.c
EXTRA_DIST += shared/spw/grspw_rasta.c
EXTRA_DIST += shared/include/grspw.h
EXTRA_DIST += shared/include/grspw_pci.h
EXTRA_DIST += shared/include/grspw_rasta.h

# UART
EXTRA_DIST += shared/uart/cons.c
EXTRA_DIST += shared/uart/apbuart_cons.c
EXTRA_DIST += shared/include/cons.h
EXTRA_DIST += shared/uart/apbuart.c
EXTRA_DIST += shared/uart/apbuart_pci.c
EXTRA_DIST += shared/uart/apbuart_rasta.c
EXTRA_DIST += shared/include/apbuart.h
EXTRA_DIST += shared/include/apbuart_pci.h
EXTRA_DIST += shared/include/apbuart_rasta.h

# CAN (OC_CAN, GRCAN)
EXTRA_DIST += shared/can/occan.c
EXTRA_DIST += shared/can/occan_pci.c
EXTRA_DIST += shared/can/grcan.c
EXTRA_DIST += shared/can/grcan_rasta.c
EXTRA_DIST += shared/include/occan.h
EXTRA_DIST += shared/include/occan_pci.h
EXTRA_DIST += shared/include/grcan.h
EXTRA_DIST += shared/include/grcan_rasta.h

# MIL-STD-B1553 (Core1553BRM)
EXTRA_DIST += shared/1553/b1553brm.c
EXTRA_DIST += shared/1553/b1553brm_pci.c
EXTRA_DIST += shared/1553/b1553brm_rasta.c
EXTRA_DIST += shared/include/b1553brm.h
EXTRA_DIST += shared/include/b1553brm_pci.h
EXTRA_DIST += shared/include/b1553brm_rasta.h

# I2C-master (I2CMST)
EXTRA_DIST += shared/i2c/i2cmst.c
EXTRA_DIST += shared/include/i2cmst.h

# Driver Manager
EXTRA_DIST += shared/drvmgr/ambapp_bus.c
EXTRA_DIST += shared/drvmgr/ambapp_bus_grlib.c
EXTRA_DIST += shared/drvmgr/ambapp_bus_leon2.c
EXTRA_DIST += shared/drvmgr/leon2_amba_bus.c

EXTRA_DIST += shared/include/drvmgr/ambapp_bus_grlib.h
EXTRA_DIST += shared/include/drvmgr/ambapp_bus.h
EXTRA_DIST += shared/include/drvmgr/leon2_amba_bus.h

include $(top_srcdir)/../../../automake/subdirs.am
include $(top_srcdir)/../../../automake/local.am
