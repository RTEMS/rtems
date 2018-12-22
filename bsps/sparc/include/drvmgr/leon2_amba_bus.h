/*  LEON2 Hardcoded bus driver interface.
 *
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  Bus driver for a hardcoded setup. LEON2 systems have some
 *  cores always present, here called "Standard Cores". In 
 *  addtion to the standard cores there are often extra cores
 *  that can be defined using the "Custom Cores" mechanism.
 *
 *  A Core is described by assigning a base register and 
 *  IRQ0..IRQ15 using the leon2_core structure.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __LEON2_AMBA_BUS_H__
#define __LEON2_AMBA_BUS_H__

/*** Cores location and IRQs hardcoded ***/

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp.h>

#ifdef __cplusplus
extern "C" {
#endif

/* LEON2 AMBA Driver ID generation */
#define DRIVER_LEON2_AMBA(id)	DRIVER_ID(DRVMGR_BUS_TYPE_LEON2_AMBA, id)

/* LEON2 Cores (any unique 48-bit number will do) */
#define LEON2_AMBA_NONE_ID		0
#define LEON2_AMBA_TIMER_ID		1
#define LEON2_AMBA_UART_ID		2
#define LEON2_AMBA_GPIO_ID		3
#define LEON2_AMBA_IRQCTRL_ID		4

#define LEON2_AMBA_AT697PCI_ID		100
#define LEON2_AMBA_AMBAPP_ID		0xfff0

/* LEON2 driver IDs */
#define DRIVER_LEON2_AMBA_TIMER 	DRIVER_LEON2_AMBA(LEON2_AMBA_TIMER_ID)
#define DRIVER_LEON2_AMBA_UART		DRIVER_LEON2_AMBA(LEON2_AMBA_UART_ID)
#define DRIVER_LEON2_AMBA_AT697PCI	DRIVER_LEON2_AMBA(LEON2_AMBA_AT697PCI_ID)
#define DRIVER_LEON2_AMBA_AMBAPP	DRIVER_LEON2_AMBA(LEON2_AMBA_AMBAPP_ID)

struct leon2_amba_dev_id {
	unsigned short		core_id;
};

#define EMPTY_LEON2_CORE {{LEON2_AMBA_NONE_ID}, NULL, NULL}
struct leon2_core {
	struct leon2_amba_dev_id	id;	/* Core ID */
	char				*name;	/* Name of Core */
	struct drvmgr_key		*keys;	/* Core setup (location, IRQs) */
};

struct leon2_bus {
	struct leon2_core		*std_cores;	/* The LEON2 standard cores */
	struct leon2_core		*custom_cores;	/* Custom cores on the same bus */
	struct drvmgr_map_entry		*maps_up;	/* Memory map ip-stream */
	struct drvmgr_map_entry		*maps_down;	/* Memory map down-stream */
};

extern struct leon2_core leon2_std_cores[];

/* Data structure drivers can access */
struct leon2_amba_dev_info {
	unsigned short		core_id;	/* Core ID */
	unsigned int		reg_base;	/* Register base */
	char			irqs[16];	/* 16 irqs */
};

struct leon2_amba_drv_info {
	struct drvmgr_drv	general;	/* General bus info */
	/* AMBA specific bus information */
	struct leon2_amba_dev_id	*ids;		/* Supported hardware */
};

/* Initialize LEON2 bus with a configuration 
 *  bus_config   -   What cores, their location and irqs
 *  resources    -   Driver configuration for the cores specified bus_config
 */
int leon2_root_register(
	struct leon2_bus *bus_config,
	struct drvmgr_bus_res *resources);

#ifdef __cplusplus
}
#endif

#endif
