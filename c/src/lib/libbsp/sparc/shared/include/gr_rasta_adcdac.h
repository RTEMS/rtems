/*  GR-RASTA-ADCDAC PCI Target driver.
 * 
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  Configures the GR-RASTA-ADCDAC interface PCI board.
 *  This driver provides a AMBA PnP bus by using the general part
 *  of the AMBA PnP bus driver (ambapp_bus.c).
 *
 *  Driver resources for the AMBA PnP bus provided can be set using
 *  gr_rasta_adcdac_set_resources().
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __GR_RASTA_ADCDAC_H__
#define __GR_RASTA_ADCDAC_H__

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* An array of pointers to GR-RASTA-ADCDAC resources. The resources will be
 * used by the drivers controlling the cores on the GR-RASTA-ADCDAC target AMBA bus.
 *
 * The gr_rasta_io_resources is declared weak so that the user can override the
 * default configuration.
 */
extern struct drvmgr_bus_res *gr_rasta_adcdac_resources[];

/* Options to gr_rasta_io_print function */
#define RASTA_ADCDAC_OPTIONS_AMBA   0x01 /* Print AMBA bus devices */
#define RASTA_ADCDAC_OPTIONS_IRQ    0x02 /* Print current IRQ setup */

/* Print information about GR-RASTA-IO PCI board */
void gr_rasta_adcdac_print(int options);

/* Register GR-RASTA-IO driver */
void gr_rasta_adcdac_register_drv(void);

#ifdef __cplusplus
}
#endif

#endif
