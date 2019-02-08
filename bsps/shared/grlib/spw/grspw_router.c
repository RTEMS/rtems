/* GRSPW ROUTER APB-Register Driver.
 *
 * COPYRIGHT (c) 2010-2017.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/bspIo.h>
#include <stdio.h>
#include <bsp.h>

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/grspw_router.h>

#include <grlib/grlib_impl.h>

//#define STATIC
#define STATIC static

#define UNUSED __attribute__((unused))

//#define DEBUG 1

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...) 
#endif

#define THREAD_SAFE 1

#define REG_WRITE(addr, val) (*(volatile unsigned int *)(addr) = (unsigned int)(val))
#define REG_READ(addr) (*(volatile unsigned int *)(addr))


/*
 * ROUTER RTPMAP register fields
 */
#define RTPMAP_PE (0x7fffffff << RTPMAP_PE_BIT)
#define RTPMAP_PD (0x1 << RTPMAP_PD_BIT)

#define RTPMAP_PE_BIT 1
#define RTPMAP_PD_BIT 0

/*
 * ROUTER RTACTRL register fields
 * DEFINED IN HEADER
 */

/*
 * ROUTER PCTRL register fields
 * DEFINED IN HEADER
 */

/*
 * ROUTER PSTSCFG register fields
 * DEFINED IN HEADER
 */

/*
 * ROUTER PSTS register fields
 * DEFINED IN HEADER
 */

/*
 * ROUTER PTIMER register fields
 */
#define PTIMER_RL (0xffff << PTIMER_RL_BIT)

#define PTIMER_RL_BIT 0

/*
 * ROUTER PCTRL2 register fields
 * DEFINED IN HEADER
 */

/*
 * ROUTER RTRCFG register fields
 */
#define RTRCFG_SP (0x1f << RTRCFG_SP_BIT)
#define RTRCFG_AP (0x1f << RTRCFG_AP_BIT)
#define RTRCFG_FP (0x1f << RTRCFG_FP_BIT)
#define RTRCFG_SR (0x1 << RTRCFG_SR_BIT)
#define RTRCFG_PE (0x1 << RTRCFG_PE_BIT)
#define RTRCFG_IC (0x1 << RTRCFG_IC_BIT)
#define RTRCFG_IS (0x1 << RTRCFG_IS_BIT)
#define RTRCFG_IP (0x1 << RTRCFG_IP_BIT)
#define RTRCFG_AI (0x1 << RTRCFG_AI_BIT)
#define RTRCFG_AT (0x1 << RTRCFG_AT_BIT)
#define RTRCFG_IE (0x1 << RTRCFG_IE_BIT)
#define RTRCFG_RE (0x1 << RTRCFG_RE_BIT)
#define RTRCFG_EE (0x1 << RTRCFG_EE_BIT)
#define RTRCFG_LS (0x1 << RTRCFG_LS_BIT)
#define RTRCFG_SA (0x1 << RTRCFG_SA_BIT)
#define RTRCFG_TF (0x1 << RTRCFG_TF_BIT)
#define RTRCFG_ME (0x1 << RTRCFG_ME_BIT)
#define RTRCFG_TA (0x1 << RTRCFG_TA_BIT)
#define RTRCFG_PP (0x1 << RTRCFG_PP_BIT)
#define RTRCFG_WCLEAR (RTRCFG_ME)

#define RTRCFG_SP_BIT 27
#define RTRCFG_AP_BIT 22
#define RTRCFG_FP_BIT 17
#define RTRCFG_SR_BIT 15
#define RTRCFG_PE_BIT 14
#define RTRCFG_IC_BIT 13
#define RTRCFG_IS_BIT 12
#define RTRCFG_IP_BIT 11
#define RTRCFG_AI_BIT 10
#define RTRCFG_AT_BIT 9
#define RTRCFG_IE_BIT 8
#define RTRCFG_RE_BIT 7
#define RTRCFG_EE_BIT 6
#define RTRCFG_LS_BIT 5
#define RTRCFG_SA_BIT 4
#define RTRCFG_TF_BIT 3
#define RTRCFG_ME_BIT 2
#define RTRCFG_TA_BIT 1
#define RTRCFG_PP_BIT 0

/*
 * ROUTER TC register fields
 */
#define TC_RE (0x3f << TC_RE_BIT)
#define TC_EN (0x3f << TC_EN_BIT)
#define TC_CF (0x3f << TC_CF_BIT)
#define TC_TC (0x3f << TC_TC_BIT)

#define TC_RE_BIT 9
#define TC_EN_BIT 8
#define TC_CF_BIT 6
#define TC_TC_BIT 0

/*
 * ROUTER VER register fields
 */
#define VER_MA (0xff << VER_MA_BIT)
#define VER_MI (0xff << VER_MI_BIT)
#define VER_PA (0xff << VER_PA_BIT)
#define VER_ID (0xff << VER_ID_BIT)

#define VER_MA_BIT 24
#define VER_MI_BIT 16
#define VER_PA_BIT 8
#define VER_ID_BIT 0

/*
 * ROUTER IDIV register fields
 */
#define IDIV_ID (0xff << IDIV_ID_BIT)

#define IDIV_ID_BIT 0

/*
 * ROUTER CFGWE register fields
 */
#define CFGWE_WE (0x1 << CFGWE_WE_BIT)

#define CFGWE_WE_BIT 0

/*
 * ROUTER PRESCALER register fields
 */
#define PRESCALER_RL (0xffff << PRESCALER_RL_BIT)

#define PRESCALER_RL_BIT 0

/*
 * ROUTER IMASK register fields
 * DEFINED IN HEADER
 */

/*
 * ROUTER ICODEGEN register fields
 * DEFINED IN HEADER
 */

/*
 * ROUTER ISRTIMER register fields
 */
#define ISRTIMER_RL (0xffff << ISRTIMER_RL_BIT)

#define ISRTIMER_RL_BIT 0

/*
 * ROUTER AITIMER register fields
 */
#define AITIMER_RL (0xffff << AITIMER_RL_BIT)

#define AITIMER_RL_BIT 0

/*
 * ROUTER ISRCTIMER register fields
 */
#define ISRCTIMER_RL (0x1f << ISRCTIMER_RL_BIT)

#define ISRCTIMER_RL_BIT 0

/*
 * ROUTER CAP register fields
 */
#define CAP_AF (0x3 << CAP_AF_BIT)
#define CAP_PF (0x7 << CAP_PF_BIT)
#define CAP_RM (0x7 << CAP_RM_BIT)
#define CAP_AS (0x1 << CAP_AS_BIT)
#define CAP_AX (0x1 << CAP_AX_BIT)
#define CAP_DP (0x1 << CAP_DP_BIT)
#define CAP_ID (0x1 << CAP_ID_BIT)
#define CAP_SD (0x1 << CAP_SD_BIT)
#define CAP_PC (0x1f << CAP_PC_BIT)
#define CAP_CC (0x1f << CAP_CC_BIT)

#define CAP_AF_BIT 24
#define CAP_PF_BIT 29
#define CAP_RM_BIT 16
#define CAP_AS_BIT 14
#define CAP_AX_BIT 13
#define CAP_DP_BIT 12
#define CAP_ID_BIT 11
#define CAP_SD_BIT 10
#define CAP_PC_BIT 4
#define CAP_CC_BIT 0

/*
 * ROUTER PNPVEND register fields
 */
#define PNPVEND_VI (0xffff << PNPVEND_VI_BIT)
#define PNPVEND_PI (0xffff << PNPVEND_PI_BIT)

#define PNPVEND_VI_BIT 16
#define PNPVEND_PI_BIT 0

/*
 * ROUTER PNPUVEND register fields
 */
#define PNPUVEND_VI (0xffff << PNPUVEND_VI_BIT)
#define PNPUVEND_PI (0xffff << PNPUVEND_PI_BIT)

#define PNPUVEND_VI_BIT 16
#define PNPUVEND_PI_BIT 0

/*
 * ROUTER MAXPLEN register fields
 */
#define MAXPLEN_ML (0xffffff << MAXPLEN_ML_BIT)

#define MAXPLEN_ML_BIT 0

/*
 * ROUTER CHARO register fields
 */
#define CHARO_OR (0x1 << CHARO_OR_BIT)
#define CHARO_CC (0x7fffffff << CHARO_CC_BIT)

#define CHARO_OR_BIT 31
#define CHARO_CC_BIT 0

/*
 * ROUTER CHARI register fields
 */
#define CHARI_OR (0x1 << CHARI_OR_BIT)
#define CHARI_CC (0x7fffffff << CHARI_CC_BIT)

#define CHARI_OR_BIT 31
#define CHARI_CC_BIT 0

/*
 * ROUTER PKTO register fields
 */
#define PKTO_OR (0x1 << PKTO_OR_BIT)
#define PKTO_CC (0x7fffffff << PKTO_CC_BIT)

#define PKTO_OR_BIT 31
#define PKTO_CC_BIT 0

/*
 * ROUTER PKTI register fields
 */
#define PKTI_OR (0x1 << PKTI_OR_BIT)
#define PKTI_CC (0x7fffffff << PKTI_CC_BIT)

#define PKTI_OR_BIT 31
#define PKTI_CC_BIT 0

/*
 * ROUTER CRED register fields
 */
#define CRED_OC (0x3f << CRED_OC_BIT)
#define CRED_IC (0x3f << CRED_IC_BIT)

#define CRED_OC_BIT 6
#define CRED_IC_BIT 0

/*
 * ROUTER RTRCOMB register fields
 */
#define RTRCOMB_SR (0x1 << RTRCOMB_SR_BIT)
#define RTRCOMB_EN (0x1 << RTRCOMB_EN_BIT)
#define RTRCOMB_PR (0x1 << RTRCOMB_PR_BIT)
#define RTRCOMB_HD (0x1 << RTRCOMB_HD_BIT)
#define RTRCOMB_PE (0x7ffff << RTRCOMB_PE_BIT)
#define RTRCOMB_PD (0x1 << RTRCOMB_PD_BIT)

#define RTRCOMB_SR_BIT 31
#define RTRCOMB_EN_BIT 30
#define RTRCOMB_PR_BIT 29
#define RTRCOMB_HD_BIT 28
#define RTRCOMB_PE_BIT 1
#define RTRCOMB_PD_BIT 0

struct router_regs {
	unsigned int resv1;		/* 0x000 */
	unsigned int psetup[255];	/* 0x004 */
	unsigned int resv2;		/* 0x400 */
	unsigned int routes[255];	/* 0x404 */
	unsigned int pctrl[32];		/* 0x800 */
	unsigned int psts[32];		/* 0x880 */
	unsigned int treload[32];	/* 0x900 */
	unsigned int pctrl2[32];	/* 0x980 */
	unsigned int cfgsts;		/* 0xA00 */
	unsigned int timecode;		/* 0xA04 */
	unsigned int ver;		/* 0xA08 */
	unsigned int idiv;		/* 0xA0C */
	unsigned int cfgwe;		/* 0xA10 */
	unsigned int tprescaler;	/* 0xA14 */
	unsigned int imask;		/* 0xA18 */
	unsigned int ipmask;		/* 0xA1C */
	unsigned int pip;		/* 0xA20 */
	unsigned int icodegen;		/* 0xA24 */
	unsigned int isr0;		/* 0xA28 */
	unsigned int isr1;		/* 0xA2C */
	unsigned int isrtimer;		/* 0xA30 */
	unsigned int aitimer;		/* 0xA34 */
	unsigned int isrctimer;		/* 0xA38 */
	unsigned int resv4;		/* 0xA3C */
	unsigned int lrunsts;		/* 0xA40 */
	unsigned int cap;		/* 0xA44 */
	unsigned int resv5[111];	/* 0xA48 */
	unsigned int charo[31];		/* 0xC04 */ /* TODO check GR718 */
	unsigned int resv6;		/* 0xC80 */
	unsigned int chari[31];		/* 0xC84 */
	unsigned int resv7;		/* 0xD00 */
	unsigned int pkto[31];		/* 0xD04 */
	unsigned int resv8;		/* 0xD80 */
	unsigned int pkti[31];		/* 0xD84 */
	unsigned int maxplen[32];		/* 0xE00 */
	unsigned int resv9;		/* 0xE80 */
	unsigned int credcnt[31];	/* 0xE84 */
	unsigned int resv10[64];	/* 0xF00 */
	unsigned int resv11;		/* 0x1000 */
	unsigned int rtcomb[255];	/* 0x1004 */
};

struct router_priv {
	struct drvmgr_dev *dev;

	/* ROUTER control registers */
	struct router_regs *regs;

	#ifdef THREAD_SAFE
	/* ROUTER semaphore */
	rtems_id sem;
	#endif

	/* ROUTER driver register */
	char devname[9];
	int index;			/* Index in order it was probed */

	int minor;
	int open;
	struct router_hw_info hwinfo;
	int nports;
	int irq_init;

	SPIN_DECLARE(plock[32])

};

int router_count = 0;
static struct router_priv *priv_tab[ROUTER_MAX];

/* Driver prototypes */

STATIC int router_init(struct router_priv *priv);
STATIC void router_hwinfo(struct router_priv *priv,
			  struct router_hw_info *hwinfo);
STATIC int router_acontrol_set(struct router_priv *priv,
		struct router_route_acontrol *control);
STATIC int router_acontrol_get(struct router_priv *priv,
		struct router_route_acontrol *control);
STATIC int router_portmap_set(struct router_priv *priv,
		struct router_route_portmap *pmap);
STATIC int router_portmap_get(struct router_priv *priv,
		struct router_route_portmap *pmap);

/* -IRQ handler */
void router_isr(void *arg);

int router_init2(struct drvmgr_dev *dev);

struct drvmgr_drv_ops router_ops =
{
	.init = {NULL,	router_init2, NULL, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id router_ids[] =
{
	{VENDOR_GAISLER, GAISLER_SPW_ROUTER},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info router_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_SPW_ROUTER_ID,/* Driver ID */
		"ROUTER_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&router_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct router_priv),	/* Let DRVMGR allocate for us */
	},
	&router_ids[0],
};

void router_register_drv (void)
{
	DBG("Registering SPW ROUTER driver\n");
	drvmgr_drv_register(&router_drv_info.general);
}

STATIC void router_hwinfo(struct router_priv *priv,
		struct router_hw_info *hwinfo)
{
	unsigned int tmp;

	/* Find router info */
	tmp = REG_READ(&priv->regs->cfgsts);
	hwinfo->nports_spw   = (tmp & RTRCFG_SP) >> RTRCFG_SP_BIT;
	hwinfo->nports_amba  = (tmp & RTRCFG_AP) >> RTRCFG_AP_BIT;
	hwinfo->nports_fifo  = (tmp & RTRCFG_FP) >> RTRCFG_FP_BIT;
	hwinfo->srouting     = (tmp & RTRCFG_SR) >> RTRCFG_SR_BIT;
	hwinfo->pnp_enable   = (tmp & RTRCFG_PE) >> RTRCFG_PE_BIT;
	hwinfo->timers_avail = (tmp & RTRCFG_TA) >> RTRCFG_TA_BIT;
	hwinfo->pnp_avail    = (tmp & RTRCFG_PP) >> RTRCFG_PP_BIT;

	tmp = REG_READ(&priv->regs->ver);
	hwinfo->ver_major = (tmp & VER_MA) >> VER_MA_BIT;
	hwinfo->ver_minor = (tmp & VER_MI) >> VER_MI_BIT;
	hwinfo->ver_patch = (tmp & VER_PA) >> VER_PA_BIT;
	hwinfo->iid       = (tmp & VER_ID) >> VER_ID_BIT;

	/* Find router capabilities */
	tmp = REG_READ(&priv->regs->cap);
	hwinfo->amba_port_fifo_size = 4 << ((tmp & CAP_AF) >> CAP_AF_BIT);
	hwinfo->spw_port_fifo_size = 16 << ((tmp & CAP_PF) >> CAP_PF_BIT);
	hwinfo->rmap_maxdlen = 4 << ((tmp & CAP_RM) >> CAP_RM_BIT);
	hwinfo->aux_async = (tmp & CAP_AS) >> CAP_AS_BIT;
	hwinfo->aux_dist_int_support = (tmp & CAP_AX) >> CAP_AX_BIT;
	hwinfo->dual_port_support = (tmp & CAP_ID) >> CAP_ID_BIT;
	hwinfo->dist_int_support = (tmp & CAP_DP) >> CAP_DP_BIT;
	hwinfo->spwd_support = (tmp & CAP_SD) >> CAP_SD_BIT;
	hwinfo->pktcnt_support = (tmp & CAP_PC) >> CAP_PC_BIT;
	hwinfo->charcnt_support = (tmp & CAP_CC) >> CAP_CC_BIT;
}

STATIC void router_hwinfo_print(struct router_hw_info *hwinfo)
{
	DBG(" -PORTS= SPW: %d, AMBA: %d, FIFO: %d\n", hwinfo->nports_spw,
			hwinfo->nports_amba, hwinfo->nports_fifo);
	DBG(" -Static routing: %s, Timers: %s\n",
			(hwinfo->srouting?"Enabled":"Disabled"),
			(hwinfo->timers_avail?"Available":"N/A"));
	DBG(" -PnP: %s, %s\n",
			(hwinfo->pnp_avail?"Available":"N/A"),
			(hwinfo->pnp_enable?"Enabled":"Disabled"));
	DBG(" -Version= Major: 0x%02x, Minor: 0x%02x, Patch: 0x%02x, ID: 0x%02x\n",
			hwinfo->ver_major, hwinfo->ver_minor,
			hwinfo->ver_patch, hwinfo->iid);
	DBG(" -Aux: %s, AuxDistInt: %s, DistInt: %s, SPWD: %s, PKTCNT: %s, "
		"CHARCNT: %s\n",
			(hwinfo->aux_async?"Async":"Sync"),
			(hwinfo->aux_dist_int_support?"Supported":"N/A"),
			(hwinfo->dist_int_support?"Supported":"N/A"),
			(hwinfo->spwd_support?"Supported":"N/A"),
			(hwinfo->pktcnt_support?"Supported":"N/A"),
			(hwinfo->charcnt_support?"Supported":"N/A"));
}

STATIC int router_acontrol_set(struct router_priv *priv,
		struct router_route_acontrol *control)
{
	int i;
	for (i=0; i<31; i++) {
		REG_WRITE(&priv->regs->routes[i], control->control[i]);
	}
	for (i=0; i<224; i++) {
		REG_WRITE(&priv->regs->routes[i+31], control->control_logical[i]);
	}
	return ROUTER_ERR_OK;
}

STATIC int router_acontrol_get(struct router_priv *priv,
		struct router_route_acontrol *control)
{
	int i;
	for (i=0; i<31; i++) {
		control->control[i] = REG_READ(&priv->regs->routes[i]);
	}
	for (i=0; i<224; i++) {
		control->control_logical[i] = REG_READ(&priv->regs->routes[i+31]);
	}
	return ROUTER_ERR_OK;
}

STATIC int router_portmap_set(struct router_priv *priv,
		struct router_route_portmap *pmap)
{
	int i;
	for (i=0; i<31; i++) {
		REG_WRITE(&priv->regs->psetup[i], pmap->pmap[i]);
	}
	for (i=0; i<224; i++) {
		REG_WRITE(&priv->regs->psetup[i+31], pmap->pmap_logical[i]);
	}
	return ROUTER_ERR_OK;
}

STATIC int router_portmap_get(struct router_priv *priv,
		struct router_route_portmap *pmap)
{
	int i;
	for (i=0; i<31; i++) {
		pmap->pmap[i] = REG_READ(&priv->regs->psetup[i]);
	}
	for (i=0; i<224; i++) {
		pmap->pmap_logical[i] = REG_READ(&priv->regs->psetup[i+31]);
	}
	return ROUTER_ERR_OK;
}

STATIC int router_init(struct router_priv *priv)
{
	#ifdef THREAD_SAFE
	int i;

	/* Device Semaphore created with count = 1 */
	if (rtems_semaphore_create(
			rtems_build_name('S', 'R', 'O', '0' + priv->index), 1,
			RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | \
			RTEMS_NO_INHERIT_PRIORITY | RTEMS_LOCAL | \
			RTEMS_NO_PRIORITY_CEILING, 0, &priv->sem) != RTEMS_SUCCESSFUL) {
		return DRVMGR_FAIL;
	}
	#endif

	/* Find router info */
	router_hwinfo(priv, &priv->hwinfo);

	priv->open = 0;
	/* Number of ports has to consider the configuration port (1 + SPW + AMBA + FIFO) */
	priv->nports = 1 + priv->hwinfo.nports_spw + priv->hwinfo.nports_amba +
		priv->hwinfo.nports_fifo;
	if ((priv->nports < 2) || (priv->nports > 32)) {
		return DRVMGR_EIO;
	}

	#ifdef THREAD_SAFE
	/* Init port spin-lock memory structures */
	for (i=0; i<priv->nports; i++) {
		SPIN_INIT(&priv->plock[i],"portlock");
	}
	#endif

	/* DEBUG print */
	DBG("SPW ROUTER[%d] with following capabilities:\n", priv->index);
	router_hwinfo_print(&priv->hwinfo);

	return DRVMGR_OK;
}

int router_init2(struct drvmgr_dev *dev)
{
	struct router_priv *priv = dev->priv;
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	unsigned int tmp;
	int i;
	int status;

	DBG("SPW ROUTER[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	if (router_count >= ROUTER_MAX) {
		return DRVMGR_ENORES;
	}

	if (priv == NULL) {
		return DRVMGR_NOMEM;
	}
	priv->dev = dev;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)priv->dev->businfo;
	if (ambadev == NULL) {
		return DRVMGR_FAIL;
	}
	pnpinfo = &ambadev->info;
	priv->regs = (struct router_regs *)pnpinfo->ahb_slv->start[0];
	priv->minor = dev->minor_drv;

	/* Initilize driver struct */
	status = router_init(priv);
	if (status != DRVMGR_OK) {
		return status;
	}

	/* Startup Action:
	 *  - Clear interrupts
	 *  - Mask interrupts
	 */

	/* Mask interrupts in ROTUER */
	REG_WRITE(&priv->regs->imask,0);
	REG_WRITE(&priv->regs->ipmask,0);

	/* Clear interrupts in ROTUER */
	REG_WRITE(&priv->regs->pip,0xffffffff);

	/* Clear errors in router and ports */
	tmp = REG_READ(&priv->regs->cfgsts);
	REG_WRITE(&priv->regs->cfgsts, tmp | RTRCFG_WCLEAR);
	tmp = REG_READ(&priv->regs->psts[0]);
	REG_WRITE(&priv->regs->psts[0], (tmp & PSTSCFG_WCLEAR) | PSTSCFG_WCLEAR2);
	for (i=1; i<priv->nports; i++) {
		tmp = REG_READ(&priv->regs->psts[i]);
		REG_WRITE(&priv->regs->psts[i], tmp & PSTS_WCLEAR);
	}

	/* Register driver internally */
	priv->index = router_count;
	priv_tab[priv->index] = priv;
	router_count++;

	/* Device name */
	sprintf(priv->devname, "router%d", priv->index);

	return DRVMGR_OK;
}

void *router_open(unsigned int dev_no)
{
	struct router_priv *priv, *ret;

	if (dev_no >= router_count) {
		DBG("ROUTER Wrong index %u\n", dev_no);
		return NULL;
	}

	priv = priv_tab[dev_no];

	if (priv == NULL) {
		DBG("ROUTER Device not initialized\n");
		return NULL;
	}

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return NULL;
	}
	#endif

	if (priv->open) {
		DBG("ROUTER Device already opened\n");
		ret = NULL;
	} else {
		/* Take the device */
		priv->open = 1;
		ret = priv;
	}
	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	return ret;
}

int router_close(void *d)
{
	struct router_priv *priv = d;
	int ret;

	if (priv == NULL) {
		DBG("ROUTER Device not initialized\n");
		return ROUTER_ERR_EINVAL;
	}

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return ROUTER_ERR_ERROR;
	}
	#endif

	if (priv->open == 0) {
		DBG("ROUTER Device already closed\n");
		ret = ROUTER_ERR_ERROR;
	} else {
		/* Mark not open */
		priv->open = 0;
		ret = ROUTER_ERR_OK;
	}

	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	return ret;
}

STATIC int router_check_open(void *d)
{
	struct router_priv *priv = d;

	if (priv == NULL) {
		DBG("ROUTER Device not initialized\n");
		return ROUTER_ERR_EINVAL;
	}

	if (priv->open == 0) {
		DBG("ROUTER Device closed\n");
		return ROUTER_ERR_ERROR;
	}

	return 0;
}

STATIC int router_check_port(void *d, int port)
{
	int ret = router_check_open(d);

	if (ret == 0) {
		struct router_priv *priv = d;
		if((port < 0) || (port >= priv->nports)) {
			DBG("ROUTER wrong port\n");
			ret = ROUTER_ERR_EINVAL;
		}
	}

	return ret;
}

STATIC int router_check_distint_support(void *d)
{
	int ret = router_check_open(d);

	if (ret == 0) {
		struct router_priv *priv = d;
		if (priv->hwinfo.dist_int_support == 0) {
			DBG("ROUTER Dist interrupts not supported\n");
			ret = ROUTER_ERR_IMPLEMENTED;
		}
	}

	return ret;
}

int router_hwinfo_get(void *d, struct router_hw_info *hwinfo)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	if (hwinfo == NULL) {
		DBG("ROUTER Wrong pointer\n");
		return ROUTER_ERR_EINVAL;
	}

	/* Get hwinfo */
	router_hwinfo(priv, hwinfo);

	return ROUTER_ERR_OK;
}

int router_print(void *d)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	/* DEBUG print */
	DBG("Number of routers: %d\n", router_count);
	DBG("SPW ROUTER[%d] with following capabilities:\n", priv->index);
	router_hwinfo_print(&priv->hwinfo);

	return ROUTER_ERR_OK;
}

/* Configure Router. Leave field NULL in order to skip configuration
 */
int router_config_set(void *d, struct router_config *cfg)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	if (cfg == NULL) {
		DBG("ROUTER CFG wrong\n");
		return ROUTER_ERR_EINVAL;
	}

	if ((cfg->flags & (ROUTER_FLG_TPRES|ROUTER_FLG_TRLD)) &&
			!priv->hwinfo.timers_avail) {
		return ROUTER_ERR_IMPLEMENTED;
	}

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return ROUTER_ERR_ERROR;
	}
	#endif

	/* Write only configuration bits in Config register */
	if (cfg->flags & ROUTER_FLG_CFG) {
		REG_WRITE(&priv->regs->cfgsts, cfg->config & ~(RTRCFG_WCLEAR));
	}

	/* Write Instance ID to Version Register */
	if (cfg->flags & ROUTER_FLG_IID) {
		REG_WRITE(&priv->regs->ver, (cfg->iid << VER_ID_BIT) & VER_ID);
	}

	/* Write startup-clock-divisor Register */
	if (cfg->flags & ROUTER_FLG_IDIV) {
		REG_WRITE(&priv->regs->idiv, (cfg->idiv << IDIV_ID_BIT) & IDIV_ID);
	}

	/* Write Timer Prescaler Register */
	if (cfg->flags & ROUTER_FLG_TPRES) {
		REG_WRITE(&priv->regs->tprescaler,
				(cfg->timer_prescaler << PRESCALER_RL_BIT) & PRESCALER_RL);
	}

	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	return ROUTER_ERR_OK;
}

int router_config_get(void *d, struct router_config *cfg)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	if (cfg == NULL) {
		DBG("ROUTER CFG wrong\n");
		return ROUTER_ERR_EINVAL;
	}

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return ROUTER_ERR_ERROR;
	}
	#endif

	cfg->config = REG_READ(&priv->regs->cfgsts) &
		~(RTRCFG_SP|RTRCFG_AP|RTRCFG_FP|RTRCFG_SR|RTRCFG_PE|RTRCFG_ME|
		  RTRCFG_TA|RTRCFG_PP);
	cfg->iid = (REG_READ(&priv->regs->ver) & VER_ID) >> VER_ID_BIT;
	cfg->idiv = (REG_READ(&priv->regs->idiv) & IDIV_ID) >> IDIV_ID_BIT;
	cfg->timer_prescaler =
		(REG_READ(&priv->regs->tprescaler) & PRESCALER_RL) >> PRESCALER_RL_BIT;

	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	return ROUTER_ERR_OK;
}

/* Configure Router routing table.
 * Leave field NULL in order to skip configuration
 */
int router_routing_table_set(void *d, struct router_routing_table *cfg)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	if (cfg == NULL) {
		DBG("ROUTER CFG wrong\n");
		return ROUTER_ERR_EINVAL;
	}

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return ROUTER_ERR_ERROR;
	}
	#endif

	/* Write Address control */
	if (cfg->flags & ROUTER_ROUTE_FLG_CTRL) {
		router_acontrol_set(priv,&cfg->acontrol);
	}

	/* Write Port map */
	if (cfg->flags & ROUTER_ROUTE_FLG_MAP) {
		router_portmap_set(priv,&cfg->portmap);
	}

	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	return ROUTER_ERR_OK;
}

int router_routing_table_get(void *d, struct router_routing_table *cfg)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	if (cfg == NULL) {
		DBG("ROUTER CFG wrong\n");
		return ROUTER_ERR_EINVAL;
	}

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return ROUTER_ERR_ERROR;
	}
	#endif

	/* Read Address control */
	router_acontrol_get(priv,&cfg->acontrol);

	/* Read Port map */
	router_portmap_get(priv,&cfg->portmap);

	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	return ROUTER_ERR_OK;
}

int router_route_set(void *d, struct router_route *route)
{
	struct router_priv *priv = d;
	int i;
	unsigned int mask;
	int error = router_check_open(d);

	if (error)
		return error;

	if (route == NULL) {
		DBG("ROUTER route wrong\n");
		return ROUTER_ERR_EINVAL;
	}

	if (route->from_address < 32) {
		/* Physical address */
		if ((route->from_address == 0) ||
				(route->from_address >= priv->nports)) {
			DBG("ROUTER wrong physical address\n");
			return ROUTER_ERR_TOOMANY;
		}
	}

	/* Compute port map */
	mask=0;
	for (i=0; i < route->count; i++) {
		if ((route->to_port[i] == 0) || (route->to_port[i] >= priv->nports)) {
			DBG("ROUTER route wrong destiny port\n");
			return ROUTER_ERR_EINVAL;
		}
		mask |= (0x1 << route->to_port[i]);
	}
	if (route->options & ROUTER_ROUTE_PACKETDISTRIBUTION_ENABLE) {
		mask |= RTPMAP_PD;
	}

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return ROUTER_ERR_ERROR;
	}
	#endif

	/* Write port map */
	REG_WRITE(&priv->regs->psetup[route->from_address-1], mask);

	/* Write Address control */
	REG_WRITE(&priv->regs->routes[route->from_address-1],
			route->options & (0xf));

	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	return ROUTER_ERR_OK;
}

int router_route_get(void *d, struct router_route *route)
{
	struct router_priv *priv = d;
	int i,count;
	unsigned int mask;
	int error = router_check_open(d);

	if (error)
		return error;

	if (route == NULL) {
		DBG("ROUTER route wrong\n");
		return ROUTER_ERR_EINVAL;
	}

	if (route->from_address < 32) {
		/* Physical address */
		if ((route->from_address == 0) ||
				(route->from_address >= priv->nports)) {
			DBG("ROUTER wrong physical address\n");
			return ROUTER_ERR_TOOMANY;
		}
	}

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return ROUTER_ERR_ERROR;
	}
	#endif

	/* Get Address control */
	route->options =
		REG_READ(&priv->regs->routes[route->from_address-1]) & (0xf);

	/* Read port map */
	mask=REG_READ(&priv->regs->psetup[route->from_address-1]);

	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	if (mask & RTPMAP_PD) {
		route->options |= ROUTER_ROUTE_PACKETDISTRIBUTION_ENABLE;
	}

	/*DBG("ROUTE from address 0x%02x read, PMAP: 0x%08x, CTRL: 0x%08x\n",
	 *		(unsigned int) route->from_address, mask,
	 *		(unsigned int) route->options);*/

	i=0;
	count=0;
	mask &= (RTPMAP_PE);
	while (mask != 0) {
		if (mask & 0x1) {
			route->to_port[count] = i;
			count++;
		}
		mask >>= 1;
		i++;
	}
	route->count=count;

	return ROUTER_ERR_OK;
}

int router_write_enable(void *d)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	REG_WRITE(&priv->regs->cfgwe, 0x1);

	return ROUTER_ERR_OK;
}

int router_write_disable(void *d)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	REG_WRITE(&priv->regs->cfgwe, 0x0);

	return ROUTER_ERR_OK;
}

int router_port_ioc(void *d, int port, struct router_port *cfg)
{
	struct router_priv *priv = d;
	unsigned int ctrl, ctrl2, sts, timer, pktl;
	SPIN_IRQFLAGS(irqflags);
	int error = router_check_port(d, port);

	if (error)
		return error;

	if (cfg == NULL) {
		DBG("ROUTER Wrong cfg\n");
		return ROUTER_ERR_EINVAL;
	}

	SPIN_LOCK_IRQ(&priv->plock[port], irqflags);

	ctrl = cfg->ctrl;
	if (cfg->flag & ROUTER_PORT_FLG_GET_CTRL) {
		ctrl = REG_READ(&priv->regs->pctrl[port]);
	}
	ctrl2 = cfg->ctrl;
	if (cfg->flag & ROUTER_PORT_FLG_GET_CTRL2) {
		ctrl2 = REG_READ(&priv->regs->pctrl2[port]);
	}
	sts = cfg->sts;
	if (cfg->flag & ROUTER_PORT_FLG_GET_STS) {
		sts = REG_READ(&priv->regs->psts[port]);
	}
	timer = cfg->timer_reload;
	if (cfg->flag & ROUTER_PORT_FLG_GET_TIMER) {
		REG_READ(&priv->regs->treload[port]);
	}
	pktl = cfg->packet_length;
	if (cfg->flag & ROUTER_PORT_FLG_GET_PKTLEN) {
		REG_READ(&priv->regs->maxplen[port]);
	}

	if (cfg->flag & ROUTER_PORT_FLG_SET_CTRL) {
		REG_WRITE(&priv->regs->pctrl[port], cfg->ctrl);
	}
	if (cfg->flag & ROUTER_PORT_FLG_SET_CTRL2) {
		REG_WRITE(&priv->regs->pctrl2[port], cfg->ctrl2);
	}
	if (cfg->flag & ROUTER_PORT_FLG_SET_STS) {
		REG_WRITE(&priv->regs->psts[port], cfg->sts);
	}
	if (cfg->flag & ROUTER_PORT_FLG_SET_TIMER) {
		REG_WRITE(&priv->regs->treload[port], cfg->timer_reload & PTIMER_RL);
	}
	if (cfg->flag & ROUTER_PORT_FLG_SET_PKTLEN) {
		REG_WRITE(&priv->regs->maxplen[port], cfg->packet_length & MAXPLEN_ML);
	}

	SPIN_UNLOCK_IRQ(&priv->plock[port], irqflags);

	cfg->ctrl = ctrl;
	cfg->ctrl2 = ctrl2;
	cfg->sts = sts;
	cfg->timer_reload = timer;
	cfg->packet_length = pktl;

	return ROUTER_ERR_OK;
}

int router_port_ctrl_rmw(void *d, int port, uint32_t *oldvalue, uint32_t bitmask, uint32_t value)
{
	struct router_priv *priv = d;
	int error = router_check_port(d, port);
	unsigned int oldctrl, ctrl;
	SPIN_IRQFLAGS(irqflags);

	if (error)
		return error;

	SPIN_LOCK_IRQ(&priv->plock[port], irqflags);

	oldctrl = REG_READ(&priv->regs->pctrl[port]);
	ctrl = ((oldctrl & ~(bitmask)) | (value & bitmask));
	REG_WRITE(&priv->regs->pctrl[port], ctrl);

	SPIN_UNLOCK_IRQ(&priv->plock[port], irqflags);

	if (oldvalue != NULL) {
		*oldvalue = oldctrl;
	}

	return ROUTER_ERR_OK;
}

int router_port_ctrl2_rmw(void *d, int port, uint32_t *oldvalue, uint32_t bitmask, uint32_t value)
{
	struct router_priv *priv = d;
	int error = router_check_port(d, port);
	unsigned int oldctrl, ctrl;
	SPIN_IRQFLAGS(irqflags);

	if (error)
		return error;

	SPIN_LOCK_IRQ(&priv->plock[port], irqflags);

	oldctrl = REG_READ(&priv->regs->pctrl2[port]);
	ctrl = ((oldctrl & ~(bitmask)) | (value & bitmask));
	REG_WRITE(&priv->regs->pctrl2[port], ctrl);

	SPIN_UNLOCK_IRQ(&priv->plock[port], irqflags);

	if (oldvalue != NULL) {
		*oldvalue = oldctrl;
	}

	return ROUTER_ERR_OK;
}

/* Read Port Control register */
int router_port_ctrl_get(void *d, int port, uint32_t *ctrl)
{
	struct router_priv *priv = d;
	int error = router_check_port(d, port);

	if (error)
		return error;

	if (ctrl == NULL) {
		DBG("ROUTER Wrong ctrl\n");
		return ROUTER_ERR_EINVAL;
	}

	*ctrl = REG_READ(&priv->regs->pctrl[port]);
	return ROUTER_ERR_OK;
}

/* Read Port Status register and clear errors if there are */
int router_port_status(void *d, int port, uint32_t *sts, uint32_t clrmsk)
{
	struct router_priv *priv = d;
	int error = router_check_port(d, port);
	SPIN_IRQFLAGS(irqflags);

	if (error)
		return error;

	if (sts == NULL) {
		DBG("ROUTER Wrong sts\n");
		return ROUTER_ERR_EINVAL;
	}

	SPIN_LOCK_IRQ(&priv->plock[port], irqflags);
	*sts = REG_READ(&priv->regs->psts[port]);
	if (port == 0) {
		REG_WRITE(&priv->regs->psts[port], ((*sts) & (PSTSCFG_WCLEAR & clrmsk)) | (PSTSCFG_WCLEAR2 & clrmsk));
	}else{
		REG_WRITE(&priv->regs->psts[port], (*sts) & (PSTS_WCLEAR & clrmsk));
	}
	SPIN_UNLOCK_IRQ(&priv->plock[port], irqflags);
	return ROUTER_ERR_OK;
}

/* Read Port Control2 register */
int router_port_ctrl2_get(void *d, int port, uint32_t *ctrl2)
{
	struct router_priv *priv = d;
	int error = router_check_port(d, port);

	if (error)
		return error;

	if (ctrl2 == NULL) {
		DBG("ROUTER Wrong ctrl2\n");
		return ROUTER_ERR_EINVAL;
	}

	*ctrl2 = REG_READ(&priv->regs->pctrl2[port]);
	return ROUTER_ERR_OK;
}

/* Write Port Control Register */
int router_port_ctrl_set(void *d, int port, uint32_t mask, uint32_t ctrl)
{
	return router_port_ctrl_rmw(d, port, NULL, mask, ctrl);
}

/* Write Port Control2 Register */
int router_port_ctrl2_set(void *d, int port, uint32_t mask, uint32_t ctrl2)
{
	return router_port_ctrl_rmw(d, port, NULL, mask, ctrl2);
}

int router_port_treload_set(void *d, int port, uint32_t reload)
{
	struct router_priv *priv = d;
	int error = router_check_port(d, port);

	if (error)
		return error;

	REG_WRITE(&priv->regs->treload[port], reload & PTIMER_RL);

	return ROUTER_ERR_OK;
}

int router_port_treload_get(void *d, int port, uint32_t *reload)
{
	struct router_priv *priv = d;
	int error = router_check_port(d, port);

	if (error)
		return error;

	if (reload == NULL) {
		DBG("ROUTER Wrong reload pointer\n");
		return ROUTER_ERR_EINVAL;
	}

	*reload = REG_READ(&priv->regs->treload[port]) & PTIMER_RL;

	return ROUTER_ERR_OK;
}

int router_port_maxplen_set(void *d, int port, uint32_t length)
{
	struct router_priv *priv = d;
	int error = router_check_port(d, port);

	if (error)
		return error;

	REG_WRITE(&priv->regs->maxplen[port], length & MAXPLEN_ML);

	return ROUTER_ERR_OK;
}

int router_port_maxplen_get(void *d, int port, uint32_t *length)
{
	struct router_priv *priv = d;
	int error = router_check_port(d, port);

	if (error)
		return error;

	if (length == NULL) {
		DBG("ROUTER Wrong length pointer\n");
		return ROUTER_ERR_EINVAL;
	}

	*length = REG_READ(&priv->regs->maxplen[port]);

	return ROUTER_ERR_OK;
}

/* Get Port Link Status */
int router_port_link_status(void *d, int port)
{
	struct router_priv *priv = d;
	int error = router_check_port(d, port);

	if (error)
		return error;

	return ((REG_READ(&priv->regs->psts[port]) & PSTS_LS) >> PSTS_LS_BIT);
}

int router_port_disable(void *d, int port)
{
	return router_port_ctrl_rmw(d, port, NULL, PCTRL_DI, PCTRL_DI);
}

int router_port_enable(void *d, int port)
{
	return router_port_ctrl_rmw(d, port, NULL, PCTRL_DI, 0);
}

int router_port_link_stop(void *d, int port)
{
	return router_port_ctrl_rmw(d, port, NULL, PCTRL_LD | PCTRL_LS, PCTRL_LD);
}

int router_port_link_start(void *d, int port)
{
	return router_port_ctrl_rmw(d, port, NULL, PCTRL_LD | PCTRL_LS, PCTRL_LS);
}

int router_port_link_div(void *d, int port, int rundiv)
{
	return router_port_ctrl_rmw(d, port, NULL, PCTRL_RD, (rundiv << PCTRL_RD_BIT) & PCTRL_RD);
}

int router_port_link_receive_spill(void *d, int port)
{
	struct router_priv *priv = d;
	unsigned int ctrl;
	SPIN_IRQFLAGS(irqflags);
	int error = router_check_port(d, port);

	if (error)
		return error;

	SPIN_LOCK_IRQ(&priv->plock[port], irqflags);

	ctrl = REG_READ(&priv->regs->pctrl[port]);
	REG_WRITE(&priv->regs->pctrl[port], (ctrl| (PCTRL_RS)));

	/* Wait until the spill is done */
	while(REG_READ(&priv->regs->pctrl[port]) & PCTRL_RS) {};

	SPIN_UNLOCK_IRQ(&priv->plock[port], irqflags);

	return ROUTER_ERR_OK;
}

int router_port_link_transmit_reset(void *d, int port)
{
	struct router_priv *priv = d;
	unsigned int ctrl;
	SPIN_IRQFLAGS(irqflags);
	int error = router_check_port(d, port);

	if (error)
		return error;

	SPIN_LOCK_IRQ(&priv->plock[port], irqflags);

	ctrl = REG_READ(&priv->regs->pctrl[port]);
	REG_WRITE(&priv->regs->pctrl[port], (ctrl| (PCTRL_TF)));

	/* Wait until the spill is done */
	while(REG_READ(&priv->regs->pctrl[port]) & PCTRL_TF) {};

	SPIN_UNLOCK_IRQ(&priv->plock[port], irqflags);

	return ROUTER_ERR_OK;
}

int router_port_cred_get(void *d, int port, uint32_t *cred)
{
	struct router_priv *priv = d;
	int error = router_check_port(d, port);

	if (error)
		return error;

	if (cred == NULL) {
		DBG("ROUTER Wrong cred pointer\n");
		return ROUTER_ERR_EINVAL;
	}

	*cred = REG_READ(&priv->regs->credcnt[port]);
	return ROUTER_ERR_OK;
}

int router_instance_set(void *d, uint8_t instance)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	REG_WRITE(&priv->regs->ver, (instance << VER_ID_BIT) & VER_ID);
	return ROUTER_ERR_OK;
}

int router_idiv_set(void *d, uint8_t idiv)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	REG_WRITE(&priv->regs->idiv, (idiv << IDIV_ID_BIT) & IDIV_ID);
	return ROUTER_ERR_OK;
}

int router_tpresc_set(void *d, uint32_t prescaler)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	REG_WRITE(&priv->regs->tprescaler,
			(prescaler << PRESCALER_RL_BIT) & PRESCALER_RL);
	return ROUTER_ERR_OK;
}

int router_instance_get(void *d, uint8_t *instance)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	if (instance == NULL) {
		DBG("ROUTER Wrong instance pointer\n");
		return ROUTER_ERR_EINVAL;
	}

	*instance = REG_READ(&priv->regs->ver);

	return ROUTER_ERR_OK;
}

int router_idiv_get(void *d, uint8_t *idiv)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	if (idiv == NULL) {
		DBG("ROUTER Wrong idiv pointer\n");
		return ROUTER_ERR_EINVAL;
	}

	*idiv = REG_READ(&priv->regs->idiv);

	return ROUTER_ERR_OK;
}

int router_tpresc_get(void *d, uint32_t *prescaler)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	if (prescaler == NULL) {
		DBG("ROUTER Wrong prescaler pointer\n");
		return ROUTER_ERR_EINVAL;
	}

	*prescaler = REG_READ(&priv->regs->tprescaler);

	return ROUTER_ERR_OK;
}

int router_cfgsts_set(void *d, uint32_t cfgsts)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	REG_WRITE(&priv->regs->cfgsts, cfgsts);

	return ROUTER_ERR_OK;
}

int router_cfgsts_get(void *d, uint32_t *cfgsts)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	if (cfgsts == NULL) {
		DBG("ROUTER Wrong cfgsts pointer\n");
		return ROUTER_ERR_EINVAL;
	}

	*cfgsts = REG_READ(&priv->regs->cfgsts);

	return ROUTER_ERR_OK;
}

int router_tc_enable(void *d)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	REG_WRITE(&priv->regs->timecode, TC_EN);

	return ROUTER_ERR_OK;
}

int router_tc_disable(void *d)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	REG_WRITE(&priv->regs->timecode, 0);

	return ROUTER_ERR_OK;
}

int router_tc_reset(void *d)
{
	struct router_priv *priv = d;
	unsigned int tc;
	int error = router_check_open(d);

	if (error)
		return error;

	tc = REG_READ(&priv->regs->timecode);
	REG_WRITE(&priv->regs->timecode, tc | TC_RE);

	return ROUTER_ERR_OK;
}

int router_tc_get(void *d)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	return (REG_READ(&priv->regs->timecode) & (TC_CF | TC_TC)) >> TC_TC_BIT;
}

int router_interrupt_unmask(void *d, int options)
{
	struct router_priv *priv = d;
	unsigned int mask;
	int error = router_check_open(d);

	if (error)
		return error;

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return ROUTER_ERR_ERROR;
	}
	#endif

	/* Unmask interrupts in ROTUER */
	/* Get previous mask */
	mask = REG_READ(&priv->regs->imask);

	/* Clear previous interrupts*/
	REG_WRITE(&priv->regs->pip, 0xffffffff);

	/* Set new mask */
	REG_WRITE(&priv->regs->imask, mask | options);

	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	return ROUTER_ERR_OK;
}

int router_interrupt_mask(void *d, int options)
{
	struct router_priv *priv = d;
	unsigned int mask;
	int error = router_check_open(d);

	if (error)
		return error;

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return ROUTER_ERR_ERROR;
	}
	#endif

	/* Mask interrupts in ROTUER */
	/* Get previous mask */
	mask = REG_READ(&priv->regs->imask);

	/* Clear previous interrupts*/
	REG_WRITE(&priv->regs->pip, 0xffffffff);

	/* Set new mask */
	REG_WRITE(&priv->regs->imask, mask & ~(options));

	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	return ROUTER_ERR_OK;
}

int router_port_interrupt_unmask(void *d, int port)
{
	struct router_priv *priv = d;
	unsigned int mask;
	int error = router_check_port(d, port);

	if (error)
		return error;

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return ROUTER_ERR_ERROR;
	}
	#endif

	/* Unmask interrupts in ROTUER */
	/* Get previous mask */
	mask = REG_READ(&priv->regs->ipmask);

	/* Clear previous interrupts*/
	REG_WRITE(&priv->regs->pip, (0x1 << port));

	/* Set new mask */
	REG_WRITE(&priv->regs->ipmask, mask | (0x1 << port));

	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	return ROUTER_ERR_OK;
}

int router_port_interrupt_mask(void *d, int port)
{
	struct router_priv *priv = d;
	unsigned int mask;
	int error = router_check_port(d, port);

	if (error)
		return error;

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return ROUTER_ERR_ERROR;
	}
	#endif

	/* Mask interrupts in ROTUER */
	/* Get previous mask */
	mask = REG_READ(&priv->regs->ipmask);

	/* Clear previous interrupts*/
	REG_WRITE(&priv->regs->pip, (0x1 << port));

	/* Set new mask */
	REG_WRITE(&priv->regs->ipmask, mask & ~(0x1 << port));

	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	return ROUTER_ERR_OK;
}

int router_reset(void *d)
{
	struct router_priv *priv = d;
	int error = router_check_open(d);

	if (error)
		return error;

	/* Reset router */
	REG_WRITE(&priv->regs->cfgsts, RTRCFG_RE);

	return ROUTER_ERR_OK;
}

int router_icodegen_enable(void *d, uint8_t intn, uint32_t aitimer,
		int options)
{
	struct router_priv *priv = d;
	int error = router_check_distint_support(d);

	if (error)
		return error;

	#ifdef THREAD_SAFE
	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
			!= RTEMS_SUCCESSFUL) {
		DBG("ROUTER Sempahore failed\n");
		return ROUTER_ERR_ERROR;
	}
	#endif

	REG_WRITE(&priv->regs->icodegen, (options & ~(ICODEGEN_IN)) |
			ICODEGEN_EN | (intn & ICODEGEN_IN));

	if (options & ICODEGEN_TE) {
		REG_WRITE(&priv->regs->aitimer, (aitimer & AITIMER_RL));
	}

	#ifdef THREAD_SAFE
	/* Unlock dev */
	rtems_semaphore_release(priv->sem);
	#endif

	return ROUTER_ERR_OK;
}

int router_icodegen_disable(void *d)
{
	struct router_priv *priv = d;
	int error = router_check_distint_support(d);

	if (error)
		return error;

	REG_WRITE(&priv->regs->icodegen, ICODEGEN_TE);

	return ROUTER_ERR_OK;
}

int router_isrctimer_set(void *d, uint32_t reloadvalue)
{
	struct router_priv *priv = d;
	int error = router_check_distint_support(d);

	if (error)
		return error;

	/* Set ISRC TIMER */
	REG_WRITE(&priv->regs->isrctimer, (reloadvalue & (ISRCTIMER_RL)));

	return ROUTER_ERR_OK;
}

int router_isrtimer_set(void *d, uint32_t reloadvalue)
{
	struct router_priv *priv = d;
	int error = router_check_distint_support(d);

	if (error)
		return error;

	/* Set ISR TIMER */
	REG_WRITE(&priv->regs->isrtimer, (reloadvalue & (ISRTIMER_RL)));

	return ROUTER_ERR_OK;
}

int router_isrctimer_get(void *d, uint32_t *reloadvalue)
{
	struct router_priv *priv = d;
	int error = router_check_distint_support(d);

	if (error)
		return error;

	if (reloadvalue == NULL) {
		DBG("ROUTER Wrong reloadvalue pointer\n");
		return ROUTER_ERR_EINVAL;
	}

	/* Set ISRC TIMER */
	*reloadvalue = REG_READ(&priv->regs->isrctimer) & (ISRCTIMER_RL);

	return ROUTER_ERR_OK;
}

int router_isrtimer_get(void *d, uint32_t *reloadvalue)
{
	struct router_priv *priv = d;
	int error = router_check_distint_support(d);

	if (error)
		return error;

	if (reloadvalue == NULL) {
		DBG("ROUTER Wrong reloadvalue pointer\n");
		return ROUTER_ERR_EINVAL;
	}

	/* Set ISR TIMER */
	*reloadvalue = REG_READ(&priv->regs->isrtimer) & (ISRTIMER_RL);

	return ROUTER_ERR_OK;
}
