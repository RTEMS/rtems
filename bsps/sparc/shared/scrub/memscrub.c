/*  Memory Scrubber register driver
 *
 *  COPYRIGHT (c) 2017.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rtems/bspIo.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>

#include <grlib/memscrub.h>

/*#define STATIC*/
#define STATIC static

#define UNUSED __attribute__((unused))

/*#define DEBUG 1*/

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...) 
#endif

#define REG_WRITE(addr, val) (*(volatile uint32_t *)(addr) = (uint32_t)(val))
#define REG_READ(addr) (*(volatile uint32_t *)(addr))

/* 
 * MEMORYSCRUBBER AHBS register fields
 * DEFINED IN HEADER FILE
 */

/* 
 * MEMORYSCRUBBER AHBERC register fields
 * DEFINED IN HEADER FILE
 */

/* 
 * MEMORYSCRUBBER STAT register fields
 * DEFINED IN HEADER FILE
 */

/* 
 * MEMORYSCRUBBER CONFIG register fields
 * DEFINED IN HEADER FILE
 */

/* 
 * MEMORYSCRUBBER ETHRES register fields
 * DEFINED IN HEADER FILE
 */

/* MEMORYSCRUBBER Registers layout */
struct memscrub_regs {
	volatile uint32_t ahbstatus; /* 0x00 */
	volatile uint32_t ahbfailing; /* 0x04 */
	volatile uint32_t ahberc; /* 0x08 */
	volatile uint32_t resv1; /* 0x0c */
	volatile uint32_t status; /* 0x10 */
	volatile uint32_t config; /* 0x14 */
	volatile uint32_t rangel; /* 0x18 */
	volatile uint32_t rangeh; /* 0x1c */
	volatile uint32_t pos; /* 0x20 */
	volatile uint32_t ethres; /* 0x24 */
	volatile uint32_t init; /* 0x28 */
	volatile uint32_t rangel2; /* 0x2c */
	volatile uint32_t rangeh2; /* 0x30 */
};

#define DEVNAME_LEN 10
struct memscrub_priv {
	struct drvmgr_dev *dev;
	char devname[DEVNAME_LEN];
	struct memscrub_regs *regs;
	int minor;
	int burstlen;
	int blockmask;
	/* Cached error */
	uint32_t last_status;
	uint32_t last_address;
	/* User defined ISR */
	memscrub_isr_t isr;
	void *isr_arg;
};
static struct memscrub_priv * memscrubpriv = NULL;

STATIC int memscrub_init2(struct drvmgr_dev *dev);
STATIC int memscrub_init(struct memscrub_priv *priv);

void memscrub_isr(void *arg);

struct drvmgr_drv_ops memscrub_ops =
{
	.init = {NULL, memscrub_init2, NULL, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id memscrub_ids[] =
{
	{VENDOR_GAISLER, GAISLER_MEMSCRUB},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info memscrub_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_MEMSCRUB_ID,/* Driver ID */
		"MEMSCRUB_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&memscrub_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct memscrub_priv),
	},
	&memscrub_ids[0]
};

void memscrub_register_drv (void)
{
	drvmgr_drv_register(&memscrub_drv_info.general);
}

STATIC int memscrub_init(struct memscrub_priv *priv)
{
	struct ambapp_ahb_info *ahb;
	struct amba_dev_info *ainfo = priv->dev->businfo;
	unsigned int tmp;
	int i,j;

	/* Get device information from AMBA PnP information */
	if (ainfo == NULL){
		return MEMSCRUB_ERR_ERROR;
	}

	/* Find MEMSCRUB core from Plug&Play information */
	ahb = ainfo->info.ahb_slv;
	priv->regs = (struct memscrub_regs *)ahb->start[0];

	DBG("MEMSCRUB regs 0x%08x\n", (unsigned int) priv->regs);

	/* Find MEMSCRUB capabilities */
	tmp = REG_READ(&priv->regs->status);
	i = (tmp & STAT_BURSTLEN) >> STAT_BURSTLEN_BIT;
	for (j=1; i>0; i--) j <<= 1;
	priv->burstlen = j;


	/* If scrubber is active, we cannot stop it to read blockmask value */
	if (tmp & STAT_ACTIVE){
		priv->blockmask = 0;
	}else{
		/* Detect block size in bytes and burst length */
		tmp = REG_READ(&priv->regs->rangeh);
		REG_WRITE(&priv->regs->rangeh, 0);
		priv->blockmask = REG_READ(&priv->regs->rangeh);
		REG_WRITE(&priv->regs->rangeh, tmp);
	}

	/* DEBUG print */
	DBG("MEMSCRUB with following capabilities:\n");
	DBG(" -Burstlength: %d\n", priv->burstlen); 

	return MEMSCRUB_ERR_OK;
}

STATIC int memscrub_init2(struct drvmgr_dev *dev)
{
	struct memscrub_priv *priv = dev->priv;

	DBG("MEMSCRUB[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	if (memscrubpriv) {
		DBG("Driver only supports one MEMSCRUB core\n");
		return DRVMGR_FAIL;
	}

	if (priv==NULL){
		return DRVMGR_NOMEM;
	}

	/* Assign priv */
	priv->dev = dev;
	strncpy(&priv->devname[0], "memscrub0", DEVNAME_LEN);
	memscrubpriv=priv;

	/* Initilize driver struct */
	if (memscrub_init(priv) != MEMSCRUB_ERR_OK){
		return DRVMGR_FAIL;
	}

	/* Startup Action:
	 *	- Clear status
	 *	- Register ISR
	 */

	/* Initialize hardware by clearing its status */
	REG_WRITE(&priv->regs->ahbstatus, 0);
	REG_WRITE(&priv->regs->status, 0);

	return DRVMGR_OK;
}


int memscrub_init_start(uint32_t value, uint8_t delay, int options)
{
	struct memscrub_priv *priv = memscrubpriv;
	uint32_t sts, tmp;
	int i;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	/* Check if scrubber is active */
	sts = REG_READ(&priv->regs->status);
	if (sts & STAT_ACTIVE){
		DBG("MEMSCRUB running.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	/* Check if we need to probe blockmask */
	if (priv->blockmask == 0){
		/* Detect block size in bytes and burst length */
		tmp = REG_READ(&priv->regs->rangeh);
		REG_WRITE(&priv->regs->rangeh, 0);
		priv->blockmask = REG_READ(&priv->regs->rangeh);
		REG_WRITE(&priv->regs->rangeh, tmp);
	}

	/* Set data value */
	for (i=0; i<priv->blockmask; i+=4){
		REG_WRITE(&priv->regs->init,value);
	}

	/* Clear unused bits */
	options = options & ~(CONFIG_MODE | CONFIG_DELAY);

	/* Enable scrubber */
	REG_WRITE(&priv->regs->config, options | 
			((delay << CONFIG_DELAY_BIT) & CONFIG_DELAY) | 
			CONFIG_MODE_INIT | CONFIG_SCEN);

	DBG("MEMSCRUB INIT STARTED\n");

	return MEMSCRUB_ERR_OK;
}

int memscrub_scrub_start(uint8_t delay, int options)
{
	struct memscrub_priv *priv = memscrubpriv;
	uint32_t ctrl,sts;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	/* Check if scrubber is active */
	sts = REG_READ(&priv->regs->status);
	if (sts & STAT_ACTIVE){
		/* Check if mode is not init */
		ctrl = REG_READ(&priv->regs->config);
		if ((ctrl & CONFIG_MODE)==CONFIG_MODE_INIT){
			DBG("MEMSCRUB init running.\n");
			return MEMSCRUB_ERR_ERROR;
		}
	}

	/* Clear unused bits */
	options = options & ~(CONFIG_MODE | CONFIG_DELAY);

	/* Enable scrubber */
	REG_WRITE(&priv->regs->config, options | 
			((delay << CONFIG_DELAY_BIT) & CONFIG_DELAY) | 
			CONFIG_MODE_SCRUB | CONFIG_SCEN);

	DBG("MEMSCRUB SCRUB STARTED\n");

	return MEMSCRUB_ERR_OK;
}

int memscrub_regen_start(uint8_t delay, int options)
{
	struct memscrub_priv *priv = memscrubpriv;
	uint32_t ctrl,sts;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	/* Check if scrubber is active */
	sts = REG_READ(&priv->regs->status);
	if (sts & STAT_ACTIVE){
		/* Check if mode is not init */
		ctrl = REG_READ(&priv->regs->config);
		if ((ctrl & CONFIG_MODE)==CONFIG_MODE_INIT){
			DBG("MEMSCRUB init running.\n");
			return MEMSCRUB_ERR_ERROR;
		}
	}

	/* Clear unused bits */
	options = options & ~(CONFIG_MODE | CONFIG_DELAY);

	/* Enable scrubber */
	REG_WRITE(&priv->regs->config, options | 
			((delay << CONFIG_DELAY_BIT) & CONFIG_DELAY) | 
			CONFIG_MODE_REGEN | CONFIG_SCEN);

	DBG("MEMSCRUB REGEN STARTED\n");

	return MEMSCRUB_ERR_OK;
}

int memscrub_stop(void)
{
	struct memscrub_priv *priv = memscrubpriv;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	/* Disable scrubber */
	REG_WRITE(&priv->regs->config, 0);

	/* Wait until finished */
	while(REG_READ(&priv->regs->status) & STAT_ACTIVE){};

	DBG("MEMSCRUB STOPPED\n");

	return MEMSCRUB_ERR_OK;
}

int memscrub_range_set(uint32_t start, uint32_t end)
{
	struct memscrub_priv *priv = memscrubpriv;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	if (end <= start){
		DBG("MEMSCRUB wrong address.\n");
		return MEMSCRUB_ERR_EINVAL;
	}

	/* Check if scrubber is active */
	if (REG_READ(&priv->regs->status) & STAT_ACTIVE){
		DBG("MEMSCRUB running.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	/* Set range */
	REG_WRITE(&priv->regs->rangel, start);
	REG_WRITE(&priv->regs->rangeh, end);

	DBG("MEMSCRUB range: 0x%08x-0x%08x\n",
			(unsigned int) start,
			(unsigned int) end);

	return MEMSCRUB_ERR_OK;
}

int memscrub_secondary_range_set(uint32_t start, uint32_t end)
{
	struct memscrub_priv *priv = memscrubpriv;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	if (end <= start){
		DBG("MEMSCRUB wrong address.\n");
		return MEMSCRUB_ERR_EINVAL;
	}

	/* Check if scrubber is active */
	if (REG_READ(&priv->regs->status) & STAT_ACTIVE){
		DBG("MEMSCRUB running.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	/* Set range */
	REG_WRITE(&priv->regs->rangel2, start);
	REG_WRITE(&priv->regs->rangeh2, end);

	DBG("MEMSCRUB 2nd range: 0x%08x-0x%08x\n",
			(unsigned int) start,
			(unsigned int) end);

	return MEMSCRUB_ERR_OK;
}

int memscrub_range_get(uint32_t * start, uint32_t * end)
{
	struct memscrub_priv *priv = memscrubpriv;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	if ((start==NULL) || (end == NULL)){
		DBG("MEMSCRUB wrong pointer.\n");
		return MEMSCRUB_ERR_EINVAL;
	}

	/* Get range */
	*start = REG_READ(&priv->regs->rangel);
	*end = REG_READ(&priv->regs->rangeh);

	return MEMSCRUB_ERR_OK;
}

int memscrub_secondary_range_get(uint32_t * start, uint32_t * end)
{
	struct memscrub_priv *priv = memscrubpriv;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	if ((start==NULL) || (end == NULL)){
		DBG("MEMSCRUB wrong pointer.\n");
		return MEMSCRUB_ERR_EINVAL;
	}

	/* Get range */
	*start = REG_READ(&priv->regs->rangel2);
	*end = REG_READ(&priv->regs->rangeh2);

	return MEMSCRUB_ERR_OK;
}

int memscrub_ahberror_setup(int uethres, int cethres, int options)
{
	struct memscrub_priv *priv = memscrubpriv;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	/* Set AHBERR */
	REG_WRITE(&priv->regs->ahberc, 
			((cethres << AHBERC_CECNTT_BIT) & AHBERC_CECNTT) |
			((uethres << AHBERC_UECNTT_BIT) & AHBERC_UECNTT) |
			(options & (AHBERC_CECTE | AHBERC_UECTE)));

	DBG("MEMSCRUB ahb err: UE[%d]:%s, CE[%d]:%s\n",
			(unsigned int) uethres,
			(options & AHBERC_UECTE)? "enabled":"disabled",
			(unsigned int) cethres,
			(options & AHBERC_CECTE)? "enabled":"disabled"
			);

	return MEMSCRUB_ERR_OK;
}

int memscrub_scruberror_setup(int blkthres, int runthres, int options)
{
	struct memscrub_priv *priv = memscrubpriv;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	/* Set ETHRES */
	REG_WRITE(&priv->regs->ethres, 
			((blkthres << ETHRES_BECT_BIT) & ETHRES_BECT) |
			((runthres << ETHRES_RECT_BIT) & ETHRES_RECT) |
			(options & (ETHRES_RECTE | ETHRES_BECTE)));

	DBG("MEMSCRUB scrub err: BLK[%d]:%s, RUN[%d]:%s\n",
			(unsigned int) blkthres,
			(options & ETHRES_BECTE)? "enabled":"disabled",
			(unsigned int) runthres,
			(options & ETHRES_RECTE)? "enabled":"disabled"
			);

	return MEMSCRUB_ERR_OK;
}

int memscrub_scrub_position(uint32_t * position)
{
	struct memscrub_priv *priv = memscrubpriv;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	if (position==NULL){
		DBG("MEMSCRUB wrong pointer.\n");
		return MEMSCRUB_ERR_EINVAL;
	}

	*position = REG_READ(&priv->regs->pos);

	return MEMSCRUB_ERR_OK;
}

int memscrub_isr_register(memscrub_isr_t isr, void * data)
{
	struct memscrub_priv *priv = memscrubpriv;
	unsigned int ethres, ahberc, config;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	if (isr==NULL){
		DBG("MEMSCRUB wrong pointer.\n");
		return MEMSCRUB_ERR_EINVAL;
	}

	/* Mask interrupts */
	ethres = REG_READ(&priv->regs->ethres);
	REG_WRITE(&priv->regs->ethres, ethres & ~(ETHRES_RECTE | ETHRES_BECTE));

	ahberc = REG_READ(&priv->regs->ahberc);
	REG_WRITE(&priv->regs->ahberc, ahberc & ~(AHBERC_CECTE | AHBERC_UECTE));

	config = REG_READ(&priv->regs->config);
	REG_WRITE(&priv->regs->config, config & ~(CONFIG_IRQD));

	/* Install IRQ handler if needed */
	if (priv->isr == NULL){
		drvmgr_interrupt_register(priv->dev, 0, priv->devname, memscrub_isr,
				priv);
	}

	/* Install user ISR */
	priv->isr=isr;
	priv->isr_arg=data;

	/* Unmask interrupts */
	REG_WRITE(&priv->regs->ethres, ethres);

	REG_WRITE(&priv->regs->ahberc, ahberc);

	REG_WRITE(&priv->regs->config, config);

	return MEMSCRUB_ERR_OK;
}

int memscrub_isr_unregister(void)
{
	struct memscrub_priv *priv = memscrubpriv;
	unsigned int ethres, ahberc, config;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	if (priv->isr==NULL){
		DBG("MEMSCRUB wrong pointer.\n");
		return MEMSCRUB_ERR_EINVAL;
	}

	/* Mask interrupts */
	ethres = REG_READ(&priv->regs->ethres);
	REG_WRITE(&priv->regs->ethres, ethres & ~(ETHRES_RECTE | ETHRES_BECTE));

	ahberc = REG_READ(&priv->regs->ahberc);
	REG_WRITE(&priv->regs->ahberc, ahberc & ~(AHBERC_CECTE | AHBERC_UECTE));

	config = REG_READ(&priv->regs->config);
	REG_WRITE(&priv->regs->config, config & ~(CONFIG_IRQD));

	/* Uninstall IRQ handler if needed */
	drvmgr_interrupt_unregister(priv->dev, 0, memscrub_isr, priv);

	/* Uninstall user ISR */
	priv->isr=NULL;
	priv->isr_arg=NULL;

	return MEMSCRUB_ERR_OK;
}

int memscrub_error_status(uint32_t *ahbaccess, uint32_t *ahbstatus, 
		uint32_t *scrubstatus)
{
	struct memscrub_priv *priv = memscrubpriv;
	uint32_t mask, ahbstatus_val;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	if ((ahbaccess==NULL) || (ahbstatus==NULL) || (scrubstatus == NULL)){
		DBG("MEMSCRUB wrong pointer.\n");
		return MEMSCRUB_ERR_EINVAL;
	}

	/* Get hardware status */
	*ahbaccess = REG_READ(&priv->regs->ahbfailing);
	*ahbstatus = ahbstatus_val = REG_READ(&priv->regs->ahbstatus);
	*scrubstatus = REG_READ(&priv->regs->status);

	/* Clear error status */
	mask = 0;
	/* Clear CECNT only if we crossed the CE threshold*/
	if ((ahbstatus_val & AHBS_CE) == 0){
		/* Don't clear the CECNT */
		mask |= AHBS_CECNT;
	}
	/* Clear UECNT only if we crossed the UE threshold*/
	if ((ahbstatus_val & (AHBS_NE|AHBS_CE|AHBS_SBC|AHBS_SEC)) != AHBS_NE){
		/* Don't clear the UECNT */
		mask |= AHBS_UECNT;
	}
	REG_WRITE(&priv->regs->ahbstatus, ahbstatus_val & mask);
	REG_WRITE(&priv->regs->status,0);

	return MEMSCRUB_ERR_OK;
}

int memscrub_active(void)
{
	struct memscrub_priv *priv = memscrubpriv;

	if (priv==NULL){
		DBG("MEMSCRUB not init.\n");
		return MEMSCRUB_ERR_ERROR;
	}

	return REG_READ(&priv->regs->status) & STAT_ACTIVE;
}

void memscrub_isr(void *arg)
{
	struct memscrub_priv *priv = arg;
	uint32_t fadr, ahbstatus, status, mask;

	/* Get hardware status */
	ahbstatus = REG_READ(&priv->regs->ahbstatus);
	if ((ahbstatus & (AHBS_NE|AHBS_DONE)) == 0){
		return;
	}

	/* IRQ generated by MEMSCRUB core... handle it here */

	/* Get Failing address */
	fadr = REG_READ(&priv->regs->ahbfailing);

	/* Get Status */
	status = REG_READ(&priv->regs->status);

	/* Clear error status */
	mask = 0;
	/* Clear CECNT only if we crossed the CE threshold*/
	if ((ahbstatus & AHBS_CE) == 0){
		/* Don't clear the CECNT */
		mask |= AHBS_CECNT;
	}
	/* Clear UECNT only if we crossed the UE threshold*/
	if ((ahbstatus & (AHBS_NE|AHBS_CE|AHBS_SBC|AHBS_SEC)) != AHBS_NE){
		/* Don't clear the UECNT */
		mask |= AHBS_UECNT;
	}
	REG_WRITE(&priv->regs->ahbstatus, ahbstatus & mask);
	REG_WRITE(&priv->regs->status,0);

	/* Let user handle error */
	(priv->isr)(priv->isr_arg, fadr, ahbstatus, status);

	return;
}
