/*  LEON2 GRLIB AMBA Plug & Play bus driver.
 *
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  This is driver is a wrapper for the general AMBA Plug & Play bus
 *  driver. This is a bus driver for LEON2-GRLIB systems providing a
 *  AMBA Plug & Play bus, the parent bus must be a LEON2 hardcoded
 *  Bus. All IRQs must be routed to this bus driver in order for IRQs
 *  to work. The PnP information is used to extract IRQs and base
 *  register addresses.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>

#ifdef LEON2
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libcpu/access.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <drvmgr/leon2_amba_bus.h>

#define DBG(args...)

int ambapp_leon2_int_register(
	struct drvmgr_dev *dev,
	int index,
	const char *info,
	drvmgr_isr isr,
	void *arg);
int ambapp_leon2_int_unregister(
	struct drvmgr_dev *dev,
	int index,
	drvmgr_isr isr,
	void *arg);
int ambapp_leon2_int_clear(
	struct drvmgr_dev *dev,
	int index);
int ambapp_leon2_int_mask(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_leon2_int_unmask(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_leon2_get_params(
	struct drvmgr_dev *dev,
	struct drvmgr_bus_params *params);

int ambapp_leon2_init1(struct drvmgr_dev *dev);
int ambapp_leon2_init2(struct drvmgr_dev *dev);
int ambapp_leon2_remove(struct drvmgr_dev *dev);
void ambapp_leon2_register(void);

/* READ/WRITE access to SpaceWire target over RMAP */
void *ambapp_leon2_rw_arg(struct drvmgr_dev *dev);

struct ambappl2_priv {
	struct ambapp_bus abus;
	struct ambapp_config config;
};

struct ambapp_ops ambapp_leon2_ops = {
	.int_register = ambapp_leon2_int_register,
	.int_unregister = ambapp_leon2_int_unregister,
	.int_clear = ambapp_leon2_int_clear,
	.int_mask = ambapp_leon2_int_mask,
	.int_unmask = ambapp_leon2_int_unmask,
	.get_params = ambapp_leon2_get_params
};

struct drvmgr_func ambapp_leon2_funcs[] = {
	DRVMGR_FUNC(AMBAPP_RW_ARG, ambapp_leon2_rw_arg),

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

	DRVMGR_FUNC_END
};

struct drvmgr_drv_ops ambapp_ops = {
	.init = {ambapp_leon2_init1, ambapp_leon2_init2, NULL, NULL},
	.remove = ambapp_leon2_remove,
	.info = NULL,
};

struct leon2_amba_dev_id ambapp_leon2_ids[] = {
	{LEON2_AMBA_AMBAPP_ID},
	{0}
};

struct leon2_amba_drv_info ambapp_bus_drv_leon2 = {
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_LEON2_AMBA_AMBAPP,	/* Driver ID */
		"AMBAPP_LEON2_DRV",		/* Driver Name */
		DRVMGR_BUS_TYPE_LEON2_AMBA,	/* Bus Type */
		&ambapp_ops,
		NULL,				/* Funcs */
		0,
		sizeof(struct ambappl2_priv),	/* Let DrvMgr allocate priv */
	},
	&ambapp_leon2_ids[0]
};

void ambapp_leon2_register(void)
{
	drvmgr_drv_register(&ambapp_bus_drv_leon2.general);
}

/* Function called from a hard configuration */
int ambapp_leon2_init1(struct drvmgr_dev *dev)
{
	union drvmgr_key_value *value;
	struct ambappl2_priv *priv = dev->priv;
	struct leon2_amba_dev_info *devinfo;
	struct ambapp_config *config;
	unsigned int ioarea;
	unsigned int freq_hz;
	LEON_Register_Map *regs;

	dev->name = "LEON2 AMBA PnP";

	if (!priv)
		return DRVMGR_NOMEM;

	config = &priv->config;
	config->abus = &priv->abus;
	config->ops = &ambapp_leon2_ops;
	config->maps_up = DRVMGR_TRANSLATE_ONE2ONE;
	config->maps_down = DRVMGR_TRANSLATE_ONE2ONE;
	config->funcs = ambapp_leon2_funcs;
	config->bus_type = DRVMGR_BUS_TYPE_LEON2_AMBA;

	/* Get AMBA PnP Area from REG0 */
	devinfo = (struct leon2_amba_dev_info *)dev->businfo;
	ioarea = devinfo->reg_base;

	/* Scan AMBA PnP Bus. ABUS has already been cleared with memset() */
	ambapp_scan(&priv->abus, ioarea, NULL, NULL);

	/* Try to get Configuration from resource configuration */

	value = drvmgr_dev_key_get(dev, "busFreq", DRVMGR_KT_INT);
	if (value) {
		/* Set frequency of AMBA bus if specified by user. The frequency
		 * must be for AHB bus which IOAREA matches (AHB bus 0).
		 */
		freq_hz = value->i;
	} else {
		/* Get Bus/LEON2 Frequency from timer prescaler,
		 * the hardcoded address is used to get to timer
		 */
		regs = (LEON_Register_Map *) 0x80000000;
		freq_hz = (regs->Scaler_Reload + 1) * 1000 * 1000;
	}
	/* Note that this can be overrided by a driver on the AMBA PnP bus.*/
	ambapp_freq_init(&priv->abus, NULL, freq_hz);

	value = drvmgr_dev_key_get(dev, "drvRes", DRVMGR_KT_POINTER);
	if (!value) {
		DBG("ambapp_leon2_init1: Failed getting resource drvRes\n");
		config->resources = NULL;
	} else {
		DBG("ambapp_leon2_init1: drvRes: 0x%08x\n", (unsigned int)value->ptr);
		config->resources = (struct drvmgr_bus_res *)value->ptr;
	}

	/* Initialize the AMBA Bus */
	return ambapp_bus_register(dev, config);
}

int ambapp_leon2_init2(struct drvmgr_dev *dev)
{
	return 0;
}

int ambapp_leon2_remove(struct drvmgr_dev *dev)
{
	return 0;
}

void *ambapp_leon2_rw_arg(struct drvmgr_dev *dev)
{
	return dev; /* No argument really needed, by for debug */
}

int ambapp_leon2_int_register
	(
	struct drvmgr_dev *dev,
	int index,
	const char *info,
	drvmgr_isr isr,
	void *arg
	)
{
	/* Let LEON2 bus handle interrupt requests */
	return drvmgr_interrupt_register(dev->parent->dev, index, info, isr, arg);
}

int ambapp_leon2_int_unregister
	(
	struct drvmgr_dev *dev,
	int index,
	drvmgr_isr isr,
	void *arg
	)
{
	/* Let LEON2 bus handle interrupt requests */
	return drvmgr_interrupt_unregister(dev->parent->dev, index, isr, arg);
}

int ambapp_leon2_int_clear
	(
	struct drvmgr_dev *dev,
	int index
	)
{
	/* Let LEON2 bus handle interrupt requests */
	return drvmgr_interrupt_clear(dev->parent->dev, index);
}

int ambapp_leon2_int_mask
	(
	struct drvmgr_dev *dev,
	int index
	)
{
	/* Let LEON2 bus handle interrupt requests */
	return drvmgr_interrupt_mask(dev->parent->dev, index);
}

int ambapp_leon2_int_unmask
	(
	struct drvmgr_dev *dev,
	int index
	)
{
	/* Let LEON2 bus handle interrupt requests */
	return drvmgr_interrupt_unmask(dev->parent->dev, index);
}

int ambapp_leon2_get_params(struct drvmgr_dev *dev, struct drvmgr_bus_params *params)
{
	params->dev_prefix = "";
	return 0;
}

#endif
