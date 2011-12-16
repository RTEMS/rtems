/*  General part of a AMBA Plug & Play bus driver.
 *
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB
 *
 *  This is the general part of the different AMBA Plug & Play
 *  drivers. The drivers are wrappers around this driver, making
 *  the code size smaller for systems with multiple AMBA Plug & 
 *  Play buses.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __AMBAPP_BUS_H__
#define __AMBAPP_BUS_H__

#include <drvmgr/drvmgr.h>
#include <ambapp.h>

#ifdef __cplusplus
extern "C" {
#endif

/* GRLIB AMBA Plug&Play Driver ID generation */
#define DRIVER_AMBAPP_ID(vendor, device) \
	DRIVER_ID(DRVMGR_BUS_TYPE_AMBAPP, ((((vendor) & 0xff) << 16) | ((device) & 0xfff)))

/*** Gaisler Hardware Device Driver IDs ***/
#define DRIVER_AMBAPP_GAISLER_GPTIMER_ID       DRIVER_AMBAPP_ID(VENDOR_GAISLER, GAISLER_GPTIMER)

struct amba_dev_id {
	unsigned short		vendor;
	unsigned short		device;
	/* Version ? */
};

struct amba_drv_info {
	struct drvmgr_drv	general;	/* General bus info */
	/* AMBA specific bus information */
	struct amba_dev_id		*ids;		/* Supported hardware */
};

struct amba_dev_info {
	struct amba_dev_id	id;
	struct ambapp_core	info;
};

struct ambapp_ops {
	int	(*int_register)
		(struct drvmgr_dev *dev, int index, const char *info, drvmgr_isr isr, void *arg);
	int	(*int_unregister)
		(struct drvmgr_dev *dev, int index, drvmgr_isr isr, void *arg);
	int	(*int_clear)(struct drvmgr_dev *dev, int index);
	int	(*int_mask)(struct drvmgr_dev *dev, int index);
	int	(*int_unmask)(struct drvmgr_dev *dev, int index);
	int	(*get_params)
		(struct drvmgr_dev *, struct drvmgr_bus_params *);
};

struct ambapp_config {
	struct ambapp_bus		*abus;		/* Prescanned AMBA PnP bus */
	struct ambapp_ops		*ops;		/* AMBA bus operations */
	struct drvmgr_map_entry		*maps_up;	/* Bus memory map up-stream towards CPU */
	struct drvmgr_map_entry		*maps_down;	/* Bus memory map down-stream towards HW */
	struct drvmgr_bus_res		*resources;	/* Driver Resources */
	int				bus_type;	/* Set DRVMGR_BUS_TYPE_AMBAPP_DIST if distributed AMBA Bus */
	struct drvmgr_func		*funcs;		/* Custom functions */
};

/*** Bus operations with READ/WRITE access operations ***
 *
 * The functions are implemented using the standard drvmgr RW interface
 */
#define AMBAPP_R8        DRVMGR_RWFUNC(RW_SIZE_1|RW_READ|RW_REG)
#define AMBAPP_R16       DRVMGR_RWFUNC(RW_SIZE_2|RW_READ|RW_REG)
#define AMBAPP_R32       DRVMGR_RWFUNC(RW_SIZE_4|RW_READ|RW_REG)
#define AMBAPP_R64       DRVMGR_RWFUNC(RW_SIZE_8|RW_READ|RW_REG)
#define AMBAPP_W8        DRVMGR_RWFUNC(RW_SIZE_1|RW_WRITE|RW_REG)
#define AMBAPP_W16       DRVMGR_RWFUNC(RW_SIZE_2|RW_WRITE|RW_REG)
#define AMBAPP_W32       DRVMGR_RWFUNC(RW_SIZE_4|RW_WRITE|RW_REG)
#define AMBAPP_W64       DRVMGR_RWFUNC(RW_SIZE_8|RW_WRITE|RW_REG)
#define AMBAPP_RMEM      DRVMGR_RWFUNC(RW_SIZE_ANY|RW_READ|RW_MEM)
#define AMBAPP_WMEM      DRVMGR_RWFUNC(RW_SIZE_ANY|RW_WRITE|RW_MEM)
#define AMBAPP_MEMSET    DRVMGR_RWFUNC(RW_SIZE_ANY|RW_SET|RW_MEM)
#define AMBAPP_RW_ARG    DRVMGR_RWFUNC(RW_ARG)

/* Register an ambapp bus on-top of a device */
extern int ambapp_bus_register(
	struct drvmgr_dev *dev,
	struct ambapp_config *config
	);

extern void ambapp_bus_freq_register(
	struct drvmgr_dev *dev,
	int amba_interface,
	unsigned int freq_hz);

#ifdef __cplusplus
}
#endif

#endif
