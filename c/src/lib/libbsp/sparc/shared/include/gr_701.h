/*  GR-701 PCI Target driver.
 * 
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  Configures the GR-701 interface PCI board.
 *  This driver provides a AMBA PnP bus by using the general part
 *  of the AMBA PnP bus driver (ambapp_bus.c).
 *
 *  Driver resources for the AMBA PnP bus provided can be set using
 *  gr701_set_resources().
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __GR_701_H__
#define __GR_701_H__

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* An array of pointers to GR-701 resources. The resources will be
 * used by the drivers controlling the cores on the GR-701 target AMBA bus.
 *
 * The gr701_resources is declared weak so that the user can override the
 * default configuration. The array must be terminated with a NULL resource.
 */
extern struct drvmgr_bus_res *gr701_resources[];

#define GR701_OPTIONS_AMBA   0x01
#define GR701_OPTIONS_IRQ    0x02

/* Print information about all GR-701 PCI boards */
void gr701_print(int options);

/* Print information about one GR-701 PCI board */
void gr701_print_dev(struct drvmgr_dev *dev, int options);

/* Register GR-701 driver */
void gr701_register_drv(void);

#ifdef __cplusplus
}
#endif

#endif
