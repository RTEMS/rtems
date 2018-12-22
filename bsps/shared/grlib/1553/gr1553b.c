/*  GR1553B driver, used by BC, RT and/or BM driver
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <grlib/ambapp_bus.h>

#include <grlib/gr1553b.h>

#include <grlib/grlib_impl.h>

/* Driver Manager interface for BC, RT, BM, BRM, BC-BM and RT-BM */

#define GR1553B_WRITE_REG(adr, val) *(volatile uint32_t *)(adr) = (val)
#define GR1553B_READ_REG(adr) (*(volatile uint32_t *)(adr))

#define FEAT_BC 0x1
#define FEAT_RT 0x2
#define FEAT_BM 0x4

#define ALLOC_BC 0x1
#define ALLOC_RT 0x2
#define ALLOC_BM 0x4

struct gr1553_device {
	struct drvmgr_dev *dev;
	int features;
	int alloc;
};

struct gr1553_device_feature {
	struct gr1553_device_feature *next;
	struct gr1553_device *dev;
	int minor;
};

/* Device lists */
static struct gr1553_device_feature *gr1553_bm_root = NULL;
static struct gr1553_device_feature *gr1553_rt_root = NULL;
static struct gr1553_device_feature *gr1553_bc_root = NULL;

/* Driver registered */
static int gr1553_driver_registerd = 0;

/* Add 'feat' to linked list pointed to by 'root'. A minor is also assigned. */
static void gr1553_list_add
	(
	struct gr1553_device_feature **root,
	struct gr1553_device_feature *feat
	)
{
	int minor;
	struct gr1553_device_feature *curr;

	if ( *root == NULL ) {
		*root = feat;
		feat->next = NULL;
		feat->minor = 0;
		return;
	}

	minor = 0;
retry_new_minor:
	curr = *root;
	while ( curr->next ) {
		if ( curr->minor == minor ) {
			minor++;
			goto retry_new_minor;
		}
		curr = curr->next;
	}

	feat->next = NULL;
	feat->minor = minor;
	curr->next = feat;
}

static struct gr1553_device_feature *gr1553_list_find
	(
	struct gr1553_device_feature *root,
	int minor
	)
{
	struct gr1553_device_feature *curr = root;
	while ( curr ) {
		if ( curr->minor == minor ) {
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}

struct drvmgr_dev **gr1553_bc_open(int minor)
{
	struct gr1553_device_feature *feat;
	
	feat = gr1553_list_find(gr1553_bc_root, minor);
	if ( feat == NULL )
		return NULL;

	/* Only possible to allocate is RT and BC is free,
	 * this is beacuse it is not possible to use the
	 * RT and the BC at the same time.
	 */
	if ( feat->dev->alloc & (ALLOC_BC|ALLOC_RT) )
		return NULL;

	/* Alloc BC device */
	feat->dev->alloc |= ALLOC_BC;

	return &feat->dev->dev;
}

void gr1553_bc_close(struct drvmgr_dev **dev)
{
	struct gr1553_device *d = (struct gr1553_device *)dev;

	d->alloc &= ~ALLOC_BC;
}

struct drvmgr_dev **gr1553_rt_open(int minor)
{
	struct gr1553_device_feature *feat;

	feat = gr1553_list_find(gr1553_rt_root, minor);
	if ( feat == NULL )
		return NULL;

	/* Only possible to allocate is RT and BC is free,
	 * this is beacuse it is not possible to use the
	 * RT and the BC at the same time.
	 */
	if ( feat->dev->alloc & (ALLOC_BC|ALLOC_RT) )
		return NULL;

	/* Alloc RT device */
	feat->dev->alloc |= ALLOC_RT;

	return &feat->dev->dev;
}

void gr1553_rt_close(struct drvmgr_dev **dev)
{
	struct gr1553_device *d = (struct gr1553_device *)dev;

	d->alloc &= ~ALLOC_RT;
}

struct drvmgr_dev **gr1553_bm_open(int minor)
{
	struct gr1553_device_feature *feat;
	
	feat = gr1553_list_find(gr1553_bm_root, minor);
	if ( feat == NULL )
		return NULL;

	/* Only possible to allocate is RT and BC is free,
	 * this is beacuse it is not possible to use the
	 * RT and the BC at the same time.
	 */
	if ( feat->dev->alloc & ALLOC_BM )
		return NULL;

	/* Alloc BM device */
	feat->dev->alloc |= ALLOC_BM;

	return &feat->dev->dev;
}

void gr1553_bm_close(struct drvmgr_dev **dev)
{
	struct gr1553_device *d = (struct gr1553_device *)dev;

	d->alloc &= ~ALLOC_BM;
}

static int gr1553_init2(struct drvmgr_dev *dev)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	struct gr1553b_regs *regs;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)dev->businfo;
	if ( ambadev == NULL ) {
		return DRVMGR_FAIL;
	}
	pnpinfo = &ambadev->info;
	if ( pnpinfo->apb_slv == NULL )
		return DRVMGR_EIO;
	regs = (struct gr1553b_regs *)pnpinfo->apb_slv->start;

	/* Stop IRQ */
	GR1553B_WRITE_REG(&regs->imask, 0);
	GR1553B_WRITE_REG(&regs->irq, 0xffffffff);
	/* Stop BC if not already stopped (just in case) */
	GR1553B_WRITE_REG(&regs->bc_ctrl, 0x15520204);
	/* Stop RT rx (just in case) */
	GR1553B_WRITE_REG(&regs->rt_cfg, 0x15530000);
	/* Stop BM logging (just in case) */
	GR1553B_WRITE_REG(&regs->bm_ctrl, 0);

	return DRVMGR_OK;
}

/* Register the different functionalities that the
 * core supports.
 */
static int gr1553_init3(struct drvmgr_dev *dev)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	struct gr1553_device *priv;
	struct gr1553_device_feature *feat;
	struct gr1553b_regs *regs;

	priv = grlib_malloc(sizeof(*priv));
	if ( priv == NULL )
		return DRVMGR_NOMEM;
	priv->dev = dev;
	priv->alloc = 0;
	priv->features = 0;
	dev->priv = NULL; /* Let higher level driver handle this */

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)dev->businfo;
	pnpinfo = &ambadev->info;
	regs = (struct gr1553b_regs *)pnpinfo->apb_slv->start;

	if ( GR1553B_READ_REG(&regs->bm_stat) & GR1553B_BM_STAT_BMSUP ) {
		priv->features |= FEAT_BM;
		feat = grlib_malloc(sizeof(*feat));
		feat->dev = priv;
		/* Init Minor and Next */
		gr1553_list_add(&gr1553_bm_root, feat);
	}

	if ( GR1553B_READ_REG(&regs->bc_stat) & GR1553B_BC_STAT_BCSUP ) {
		priv->features |= FEAT_BC;
		feat = grlib_malloc(sizeof(*feat));
		feat->dev = priv;
		/* Init Minor and Next */
		gr1553_list_add(&gr1553_bc_root, feat);
	}

	if ( GR1553B_READ_REG(&regs->rt_stat) & GR1553B_RT_STAT_RTSUP ) {
		priv->features |= FEAT_RT;
		feat = grlib_malloc(sizeof(*feat));
		feat->dev = priv;
		/* Init Minor and Next */
		gr1553_list_add(&gr1553_rt_root, feat);
	}

	if ( priv->features == 0 ) {
		/* no features in HW should never happen.. an I/O error? */
		free(priv);
		return DRVMGR_EIO;
	}

	return DRVMGR_OK;
}

struct drvmgr_drv_ops gr1553_ops =
{
	{NULL, gr1553_init2, gr1553_init3, NULL},
	NULL,
	NULL
};

struct amba_dev_id gr1553_ids[] =
{
	{VENDOR_GAISLER, GAISLER_GR1553B},
	{0, 0}	/* Mark end of table */
};

struct amba_drv_info gr1553_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_GR1553B_ID,/* Driver ID */
		"GR1553_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&gr1553_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		0,
	},
	&gr1553_ids[0]
};

/* Multiple drivers may call this function. The drivers that depends on
 * this driver:
 *  - BM driver
 *  - BC driver
 *  - RT driver
 */
void gr1553_register(void)
{
	if ( gr1553_driver_registerd == 0 ) {
		gr1553_driver_registerd = 1;
	        drvmgr_drv_register(&gr1553_drv_info.general);
	}
}
