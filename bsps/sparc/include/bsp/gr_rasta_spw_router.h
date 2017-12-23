/*  GR-RASTA-SPW-ROUTER PCI Peripheral driver
 *
 *  COPYRIGHT (c) 2015.
 *  Cobham Gaisler.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#ifndef __GR_RASTA_SPW_ROUTER_H__
#define __GR_RASTA_SPW_ROUTER_H__

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* An array of pointers to GR-RASTA-SPW-ROUTER resources. The resources will be
 * used by the drivers controlling the cores on the GR-RASTA-SPW-ROUTER target
 * AMBA bus.
 *
 * The gr_rasta_spw_router_resources is declared weak so that the user can
 * override the default configuration. The array must be terminated with a
 * NULL resource.
 */
extern struct drvmgr_bus_res *gr_rasta_spw_router_resources[];

/* Options to gr_rasta_spw_router_print function */
#define RASTA_SPW_ROUTER_OPTIONS_AMBA  0x01 /* Print AMBA bus devices */
#define RASTA_SPW_ROUTER_OPTIONS_IRQ   0x02 /* Print current IRQ setup */

/* Print information about all GR-CPCI-LEON4-N2X PCI boards */
void gr_rasta_spw_router_print(int options);

/* Print information about one GR-CPCI-LEON4-N2X PCI board */
void gr_rasta_spw_router_print_dev(struct drvmgr_dev *dev, int options);

/* Register GR-RASTA-SPW-ROUTER driver to Driver Manager */
void gr_rasta_spw_router_register_drv(void);

#ifdef __cplusplus
}
#endif

#endif
