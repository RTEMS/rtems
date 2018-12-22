/*  Memory Controller driver (FTMTRL, MCTRL)
 * 
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  This file contains the driver for the MCTRL memory controller.
 *  The driver sets the memory configuration registers (MCFG1, MCFG2, MCFG3)
 *  during driver initialization
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/******************* Driver manager interface ***********************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>

#include <grlib/mctrl.h>

#include <grlib/grlib_impl.h>

#define MEMSET(priv, start, c, length) memset((void *)start, c, length)

#define DBG(args...)
/*#define DBG(args...) printk(args)*/

struct mctrl_regs {
	unsigned int	mcfg[8];
};

struct mctrl_priv;

struct mctrl_ops {
	void (*mcfg_set)(struct mctrl_priv *priv, int index, void *regs, unsigned int regval);
};

struct mctrl_priv {
	struct drvmgr_dev	*dev;
	void			*regs;
	unsigned int		mcfg[8];	/* The wanted memory configuration */
	unsigned int		configured;	/* Determines what mcfgs was configured by user */
	struct mctrl_ops	*ops;		/* Operation may depend on hardware */
};

static int mctrl_init1(struct drvmgr_dev *dev);
static int mctrl_remove(struct drvmgr_dev *dev);

/* Standard MCFG registers */
static void mctrl_set_std(struct mctrl_priv *priv, int index, void *regs, unsigned int regval);

struct mctrl_ops std_mctrl_ops = 
{
	mctrl_set_std
};

struct drvmgr_drv_ops mctrl_ops = 
{
	.init = {mctrl_init1, NULL, NULL, NULL},
	.remove = mctrl_remove,
	.info = NULL
};

struct amba_dev_id mctrl_ids[] = 
{
	{VENDOR_ESA, ESA_MCTRL},
	{VENDOR_GAISLER, GAISLER_FTMCTRL},
	{VENDOR_GAISLER, GAISLER_FTSRCTRL},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info mctrl_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_MCTRL_ID,		/* Driver ID */
		"MCTRL_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&mctrl_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		0,
	},
	&mctrl_ids[0]
};

void mctrl_register_drv (void)
{
	DBG("Registering MCTRL driver\n");
	drvmgr_drv_register(&mctrl_drv_info.general);
}

static int mctrl_init1(struct drvmgr_dev *dev)
{
	struct mctrl_priv *priv;
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	int i;
	char res_name[16];
	union drvmgr_key_value *value;
	unsigned int start, length;

	DBG("MCTRL[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);
	priv = dev->priv = grlib_calloc(1, sizeof(*priv));
	if ( !priv )
		return DRVMGR_NOMEM;
	priv->dev = dev;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)priv->dev->businfo;
	if ( ambadev == NULL ) {
		return DRVMGR_FAIL;
	}
	pnpinfo = &ambadev->info;
	if ( pnpinfo->apb_slv == NULL ) {
		/* LEON2 PnP systems are missing the APB interface */
		priv->regs = (void *)0x80000000;
	} else {
		priv->regs = (void *)pnpinfo->apb_slv->start;
	}

	/* Depending on Hardware selection write/read routines */
	switch ( pnpinfo->vendor ) {
		case VENDOR_ESA:
		switch ( pnpinfo->device ) {
			case ESA_MCTRL:
			default:
				priv->ops = &std_mctrl_ops;
		}
		break;

		case VENDOR_GAISLER:
		switch ( pnpinfo->device ) {
			case GAISLER_FTMCTRL:
			case GAISLER_FTSRCTRL:
			default:
				priv->ops = &std_mctrl_ops;
		}
		break;

		default:
		priv->ops = &std_mctrl_ops;
		break;
	}

	/* Find user configuration from bus resources */
	priv->configured = 0;
	strcpy(res_name, "mcfgX");
	for(i=0; i<8; i++) {
		res_name[4] = '1' + i;
		value = drvmgr_dev_key_get(priv->dev, res_name, DRVMGR_KT_INT);
		if ( value ) {
			priv->mcfg[i] = value->i;
			priv->configured |= (1<<i);
		}
	}

	/* Init hardware registers right away, other devices may depend on it in init2(), also
	 * the washing depend on it.
	 */
	for ( i=0; i<8; i++) {
		if ( priv->configured & (1<<i) ) {
			DBG("Setting MCFG%d to 0x%08x\n", i+1, priv->mcfg[i]);
			priv->ops->mcfg_set(priv, i, priv->regs, priv->mcfg[i]);
		}
	}

	/* Wash memory partitions if user wants */
	for (i=0; i<9; i++) {
		strcpy(res_name, "washXStart");
		res_name[4] = '0' + i;
		value = drvmgr_dev_key_get(priv->dev, res_name, DRVMGR_KT_INT);
		if ( value ) {
			start = value->i;
			strcpy(res_name, "washXLength");
			res_name[4] = '0' + i;
			value = drvmgr_dev_key_get(priv->dev, res_name, DRVMGR_KT_INT);
			if ( value ) {
				length = value->i;

				if ( length > 0 ) {
					DBG("MCTRL: Washing 0x%08x-0x%08x\n", start, start+length-1);

					MEMSET(priv, (void *)start, 0, length);
				}
			}
		}
	}

	return DRVMGR_OK;
}

static int mctrl_remove(struct drvmgr_dev *dev)
{
	/* Nothing to be done */
	DBG("Removing %s\n", dev->name);
	return DRVMGR_OK;
}

/* Standard Operations */
static void mctrl_set_std(struct mctrl_priv *priv, int index, void *regs, unsigned int regval)
{
	struct mctrl_regs *pregs = regs;

	/* Store new value */
	pregs->mcfg[index] = regval;
}
