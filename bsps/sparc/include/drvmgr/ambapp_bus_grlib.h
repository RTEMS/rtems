/*  LEON3 GRLIB AMBA Plug & Play bus driver interface.
 *
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  This is driver is a wrapper for the general AMBA Plug & Play bus
 *  driver. This is the root bus driver for GRLIB systems.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __AMBAPP_BUS_GRLIB_H__
#define __AMBAPP_BUS_GRLIB_H__

#ifdef __cplusplus
extern "C" {
#endif

struct grlib_config {
	struct ambapp_bus	*abus;
	struct drvmgr_bus_res	*resources;
};

/* Register GRLIB AMBA PnP Bus as root bus at driver manager */
extern int ambapp_grlib_root_register(struct grlib_config *config);

/* Register bus driver to Driver Manager */
void ambapp_grlib_register(void);

#ifdef __cplusplus
}
#endif

#endif
