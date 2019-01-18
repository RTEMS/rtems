/*  LEON3 GRLIB AMBA Plug & Play bus driver.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <libcpu/access.h>

#include <grlib/ambapp_bus.h>
#include <grlib/ambapp_bus_grlib.h>
#include <grlib/genirq.h>

#include <bsp.h>
#include <bsp/irq.h>

#include <grlib/grlib_impl.h>

#define DBG(args...)
/*#define DBG(args...) printk(args)*/

static int ambapp_grlib_int_register(
	struct drvmgr_dev *dev,
	int irq,
	const char *info,
	drvmgr_isr isr,
	void *arg);
static int ambapp_grlib_int_unregister(
	struct drvmgr_dev *dev,
	int irq,
	drvmgr_isr isr,
	void *arg);
static int ambapp_grlib_int_clear(
	struct drvmgr_dev *dev,
	int irq);
static int ambapp_grlib_int_mask(
	struct drvmgr_dev *dev,
	int irq);
static int ambapp_grlib_int_unmask(
	struct drvmgr_dev *dev,
	int irq);
#ifdef RTEMS_SMP
static int ambapp_grlib_int_set_affinity(
	struct drvmgr_dev *dev,
	int irq,
	const Processor_mask *cpus);
#endif
static int ambapp_grlib_get_params(
	struct drvmgr_dev *dev,
	struct drvmgr_bus_params *params);

static int ambapp_grlib_init1(struct drvmgr_dev *dev);
static int ambapp_grlib_init2(struct drvmgr_dev *dev);
static int ambapp_grlib_remove(struct drvmgr_dev *dev);

/* READ/WRITE access to SpaceWire target over RMAP */
static void *ambapp_grlib_rw_arg(struct drvmgr_dev *dev);

static struct ambapp_ops ambapp_grlib_ops = {
	.int_register = ambapp_grlib_int_register,
	.int_unregister = ambapp_grlib_int_unregister,
	.int_clear = ambapp_grlib_int_clear,
	.int_mask = ambapp_grlib_int_mask,
	.int_unmask = ambapp_grlib_int_unmask,
#ifdef RTEMS_SMP
	.int_set_affinity = ambapp_grlib_int_set_affinity,
#endif
	.get_params = ambapp_grlib_get_params
};

static void *ambapp_grlib_rw_arg(struct drvmgr_dev *dev)
{
	return dev; /* No argument really needed, but for debug? */
}

static struct drvmgr_func ambapp_grlib_funcs[] =
{
	DRVMGR_FUNC(AMBAPP_RW_ARG, ambapp_grlib_rw_arg),

	DRVMGR_FUNC(AMBAPP_R8,  _ld8),
	DRVMGR_FUNC(AMBAPP_R16, _ld16),
	DRVMGR_FUNC(AMBAPP_R32, _ld32),
	DRVMGR_FUNC(AMBAPP_R64, _ld64),

	DRVMGR_FUNC(AMBAPP_W8,  _st8),
	DRVMGR_FUNC(AMBAPP_W16, _st16),
	DRVMGR_FUNC(AMBAPP_W32, _st32),
	DRVMGR_FUNC(AMBAPP_W64, _st64),

	DRVMGR_FUNC(AMBAPP_RMEM, memcpy),
	DRVMGR_FUNC(AMBAPP_WMEM, memcpy),

	DRVMGR_FUNC_END,
};

static struct drvmgr_drv_ops ambapp_grlib_drv_ops = 
{
	.init = {ambapp_grlib_init1, ambapp_grlib_init2, NULL, NULL},
	.remove = ambapp_grlib_remove,
	.info = NULL,
};

static struct drvmgr_drv ambapp_bus_drv_grlib = 
{
	DRVMGR_OBJ_DRV,			/* Driver */
	NULL,				/* Next driver */
	NULL,				/* Device list */
	DRIVER_GRLIB_AMBAPP_ID,		/* Driver ID */
	"AMBAPP_GRLIB_DRV",		/* Driver Name */
	DRVMGR_BUS_TYPE_ROOT,		/* Bus Type */
	&ambapp_grlib_drv_ops,
	NULL,				/* Funcs */
	0,
	0,
};

static struct grlib_config *drv_mgr_grlib_config = NULL;

void ambapp_grlib_register(void)
{
	drvmgr_drv_register(&ambapp_bus_drv_grlib);
}

int ambapp_grlib_root_register(struct grlib_config *config)
{

	/* Save the configuration for later */
	drv_mgr_grlib_config = config;

	/* Register root device driver */
	drvmgr_root_drv_register(&ambapp_bus_drv_grlib);

	return 0;
}

/* Function called from Driver Manager Initialization Stage 1 */
static int ambapp_grlib_init1(struct drvmgr_dev *dev)
{
	struct ambapp_config *config;

	dev->priv = NULL;
	dev->name = "GRLIB AMBA PnP";

	DBG("AMBAPP GRLIB: intializing\n");

	config = grlib_malloc(sizeof(*config));
	if ( !config )
		return RTEMS_NO_MEMORY;

	config->ops = &ambapp_grlib_ops;
	config->maps_up = DRVMGR_TRANSLATE_ONE2ONE;
	config->maps_down = DRVMGR_TRANSLATE_ONE2ONE;
	config->abus = drv_mgr_grlib_config->abus;
	config->resources = drv_mgr_grlib_config->resources;
	config->funcs = ambapp_grlib_funcs;
	config->bus_type = DRVMGR_BUS_TYPE_AMBAPP;

	/* Initialize the generic part of the AMBA Bus */
	return ambapp_bus_register(dev, config);
}

static int ambapp_grlib_init2(struct drvmgr_dev *dev)
{
	return 0;
}

static int ambapp_grlib_remove(struct drvmgr_dev *dev)
{
	return 0;
}

static int ambapp_grlib_int_register
	(
	struct drvmgr_dev *dev,
	int irq,
	const char *info,
	drvmgr_isr isr,
	void *arg
	)
{
	return rtems_interrupt_handler_install(irq, info,
			RTEMS_INTERRUPT_SHARED, isr, arg);
}

static int ambapp_grlib_int_unregister
	(
	struct drvmgr_dev *dev,
	int irq,
	drvmgr_isr isr,
	void *arg
	)
{
	return rtems_interrupt_handler_remove(irq, isr, arg);
}

static int ambapp_grlib_int_clear
	(
	struct drvmgr_dev *dev,
	int irq)
{
	BSP_shared_interrupt_clear(irq);
	return DRVMGR_OK;
}

static int ambapp_grlib_int_mask
	(
	struct drvmgr_dev *dev,
	int irq
	)
{
	BSP_shared_interrupt_mask(irq);
	return DRVMGR_OK;
}

static int ambapp_grlib_int_unmask
	(
	struct drvmgr_dev *dev,
	int irq
	)
{
	BSP_shared_interrupt_unmask(irq);
	return DRVMGR_OK;
}

#ifdef RTEMS_SMP
static int ambapp_grlib_int_set_affinity
	(
	struct drvmgr_dev *dev,
	int irq,
	const Processor_mask *cpus
	)
{
	bsp_interrupt_set_affinity(irq, cpus);
	return DRVMGR_OK;
}
#endif

static int ambapp_grlib_get_params(struct drvmgr_dev *dev, struct drvmgr_bus_params *params)
{
	/* Leave params->freq_hz untouched for default */
	params->dev_prefix = "";
	return 0;
}
