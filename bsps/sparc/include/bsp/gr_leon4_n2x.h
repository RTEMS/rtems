/*  GR-CPCI-LEON4-N2X (NGFP) PCI Peripheral driver
 *
 *  COPYRIGHT (c) 2013.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Configures the GR-CPIC-LEON4-N2X interface PCI board in peripheral
 *  mode. This driver provides a AMBA PnP bus by using the general part
 *  of the AMBA PnP bus driver (ambapp_bus.c).
 *
 *
 *  Driver resource options:
 *   NAME          DEFAULT VALUE
 *   ahbmst2pci    _RAM_START            AMBA->PCI translation PCI base address
 *   ambaFreq      200000000 (200MHz)    AMBA system frequency of LEON4-N2X
 *   cgEnMask      0x1f (all)            Clock gating enable mask
 *
 * TODO/UNTESTED
 *   Interrupt testing
 *   bar0 RESOURCE 0x00000000            L2-Cache SDRAM memory
 *   bar1 RESOURCE 0xf0000000            L2-Cache registers
 */

#ifndef __GR_CPCI_LEON4_N2X_H__
#define __GR_CPCI_LEON4_N2X_H__

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* An array of pointers to GR-CPCI-LEON4-N2X resources. The resources will be
 * used by the drivers controlling the cores on the GR-CPCI-LEON4-N2X target
 * AMBA bus.
 *
 * The gr_leon4_n2x_resources is declared weak so that the user can override the
 * default configuration. The array must be terminated with a NULL resource.
 */
extern struct drvmgr_bus_res *gr_leon4_n2x_resources[];

/* Options to gr_cpci_leon4_n2x_print function */
#define GR_LEON4_N2X_OPTIONS_AMBA   0x01 /* Print AMBA bus devices */

/* Print information about all GR-CPCI-LEON4-N2X PCI boards */
void gr_leon4_n2x_print(int options);

/* Print information about one GR-CPCI-LEON4-N2X PCI board */
void gr_cpci_leon4_n2x_print_dev(struct drvmgr_dev *dev, int options);

/* Register GR-CPCI-LEON4-N2X driver */
void gr_cpci_leon4_n2x_register_drv(void);

#ifdef __cplusplus
}
#endif

#endif
