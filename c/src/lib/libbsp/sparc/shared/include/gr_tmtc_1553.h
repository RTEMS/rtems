/*  GR-TMTC-1553 PCI Target driver.
 * 
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  Configures the GR-TMTC-1553 interface PCI board.
 *  This driver provides a AMBA PnP bus by using the general part
 *  of the AMBA PnP bus driver (ambapp_bus.c).
 *
 *  Driver resources for the AMBA PnP bus provided can be set using
 *  gr_tmtc_1553_set_resources().
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __GR_TMTC_1553_H__
#define __GR_TMTC_1553_H__

#include <drvmgr/drvmgr.h>

#ifdef __cplusplus
extern "C" {
#endif

/* An array of pointers to GR-TMTC-1553 resources. The resources will be
 * used by the drivers controlling the cores on the GR-TMTC-1553 target AMBA bus.
 *
 * The gr_rasta_io_resources is declared weak so that the user can override the
 * default configuration. The array must be terminated with a NULL resource.
 */
extern struct drvmgr_bus_res *gr_tmtc_1553_resources[];

/* Options to gr_rasta_io_print function */
#define TMTC_1553_OPTIONS_AMBA   0x01 /* Print AMBA bus devices */
#define TMTC_1553_OPTIONS_IRQ    0x02 /* Print current IRQ setup */

/* Print information about GR-RASTA-IO PCI board */
void gr_tmtc_1553_print(int options);

/* Register GR-RASTA-IO driver */
void gr_tmtc_1553_register_drv(void);

/* Print information about all GR-TMTC-1553 devices registered to this driver */
void gr_tmtc_1553_print(int options);

/* Print information about one GR-TMTC-1553 device */
void gr_tmtc_1553_print_dev(struct drvmgr_dev *dev, int options);

#ifdef __cplusplus
}
#endif

#endif
