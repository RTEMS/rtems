/*
 *  GRLIB L2CACHE Driver
 *
 *  COPYRIGHT (c) 2017
 *  Cobham Gaisler AB
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <string.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/ambapp.h>
#include <rtems.h>
#include <rtems/bspIo.h>
#include <grlib/grlib.h>
#include <bsp.h>
#include <grlib/l2c.h>

/*#define STATIC*/
#define STATIC static

/*#define INLINE*/
#define INLINE inline

#define UNUSED __attribute__((unused))

/*#define DEBUG 1*/

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...) 
#endif

/*
 * L2CACHE CTRL register fields 
 */
#define L2C_CTRL_EN (0x1 << L2C_CTRL_EN_BIT)
#define L2C_CTRL_EDAC (0x1 << L2C_CTRL_EDAC_BIT)
#define L2C_CTRL_REPL (0x3 << L2C_CTRL_REPL_BIT)
#define L2C_CTRL_IWAY (0xf << L2C_CTRL_IWAY_BIT)
#define L2C_CTRL_LOCK (0xf << L2C_CTRL_LOCK_BIT)
#define L2C_CTRL_HPRHB (0x1 << L2C_CTRL_HPRHB_BIT)
#define L2C_CTRL_HPB (0x1 << L2C_CTRL_HPB_BIT)
#define L2C_CTRL_UC (0x1 << L2C_CTRL_UC_BIT)
#define L2C_CTRL_HC (0x1 << L2C_CTRL_HC_BIT)
#define L2C_CTRL_WP (0x1 << L2C_CTRL_WP_BIT)
#define L2C_CTRL_HP (0x1 << L2C_CTRL_HP_BIT)

#define L2C_CTRL_EN_BIT 31
#define L2C_CTRL_EDAC_BIT 30
#define L2C_CTRL_REPL_BIT 28
#define L2C_CTRL_IWAY_BIT 12
#define L2C_CTRL_LOCK_BIT 8
#define L2C_CTRL_HPRHB_BIT 5
#define L2C_CTRL_HPB_BIT 4
#define L2C_CTRL_UC_BIT 3
#define L2C_CTRL_HC_BIT 2
#define L2C_CTRL_WP_BIT 1
#define L2C_CTRL_HP_BIT 0

/*
 * L2CACHE STATUS register fields 
 */
#define L2C_STAT_LS (0x1 << L2C_STAT_LS_BIT)
#define L2C_STAT_AT (0x1 << L2C_STAT_AT_BIT)
#define L2C_STAT_MP (0x1 << L2C_STAT_MP_BIT)
#define L2C_STAT_MTRR (0x3f << L2C_STAT_MTRR_BIT)
#define L2C_STAT_BBUSW (0x7 << L2C_STAT_BBUSW_BIT)
#define L2C_STAT_WAYSIZE (0x7ff << L2C_STAT_WAYSIZE_BIT)
#define L2C_STAT_WAY (0x3 << L2C_STAT_WAY_BIT)

#define L2C_STAT_LS_BIT 24
#define L2C_STAT_AT_BIT 23
#define L2C_STAT_MP_BIT 22
#define L2C_STAT_MTRR_BIT 16
#define L2C_STAT_BBUSW_BIT 13
#define L2C_STAT_WAYSIZE_BIT 2
#define L2C_STAT_WAY_BIT 0

/*
 * L2CACHE MTRR register fields 
 */
#define L2C_MTRR_ADDR (0x3fff << L2C_MTRR_ADDR_BIT)
#define L2C_MTRR_ACC (0x3 << L2C_MTRR_ACC_BIT)
#define L2C_MTRR_MASK (0x3fff << L2C_MTRR_MASK_BIT)
#define L2C_MTRR_WP (0x1 << L2C_MTRR_WP_BIT)
#define L2C_MTRR_AC (0x1 << L2C_MTRR_AC_BIT)

#define L2C_MTRR_ADDR_BIT 18
#define L2C_MTRR_ACC_BIT 16
#define L2C_MTRR_MASK_BIT 2
#define L2C_MTRR_WP_BIT 1
#define L2C_MTRR_AC_BIT 0

#define L2C_MTRR_UNCACHED 0
#define L2C_MTRR_WRITETHROUGH (0x1 << L2C_MTRR_ACC_BIT)
#define L2C_MTRR_WRITEPROT_ENABLE L2C_MTRR_WP
#define L2C_MTRR_WRITEPROT_DISABLE 0
#define L2C_MTRR_ACCESSCONTROL_ENABLE L2C_MTRR_AC
#define L2C_MTRR_ACCESSCONTROL_DISABLE 0

#define REG_WRITE(addr, val) (*(volatile unsigned int *)(addr) = (unsigned int)(val))
#define REG_READ(addr) (*(volatile unsigned int *)(addr))

/*
 * L2CACHE FLUSHMEM register fields 
 */
#define L2C_FLUSH_ADDR (0x7ffffff << L2C_FLUSH_ADDR_BIT)
#define L2C_FLUSH_DI (0x1 << L2C_FLUSH_DI_BIT)
#define L2C_FLUSH_FMODE (0x7 << L2C_FLUSH_FMODE_BIT)

#define L2C_FLUSH_ADDR_BIT 5
#define L2C_FLUSH_DI_BIT 3
#define L2C_FLUSH_FMODE_BIT 0

#define L2C_FLUSH_FMODE_INV_ONE (0x1 << L2C_FLUSH_FMODE_BIT)
#define L2C_FLUSH_FMODE_WB_ONE (0x2 << L2C_FLUSH_FMODE_BIT)
#define L2C_FLUSH_FMODE_INV_WB_ONE (0x3 << L2C_FLUSH_FMODE_BIT)
#define L2C_FLUSH_FMODE_INV_ALL (0x5 << L2C_FLUSH_FMODE_BIT)
#define L2C_FLUSH_FMODE_WB_ALL (0x6 << L2C_FLUSH_FMODE_BIT)
#define L2C_FLUSH_FMODE_INV_WB_ALL (0x7 << L2C_FLUSH_FMODE_BIT)

/*
 * L2CACHE FLUSSETINDEX register fields 
 */
#define L2C_FLUSHSI_INDEX (0xffff << L2C_FLUSHSI_INDEX_BIT)
#define L2C_FLUSHSI_TAG (0x3fffff << L2C_FLUSHSI_TAG_BIT)
#define L2C_FLUSHSI_FL (0x1 << L2C_FLUSHSI_FL_BIT)
#define L2C_FLUSHSI_VB (0x1 << L2C_FLUSHSI_VB_BIT)
#define L2C_FLUSHSI_DB (0x1 << L2C_FLUSHSI_DB_BIT)
#define L2C_FLUSHSI_WAY (0x3 << L2C_FLUSHSI_WAY_BIT)
#define L2C_FLUSHSI_DI (0x1 << L2C_FLUSHSI_DI_BIT)
#define L2C_FLUSHSI_WF (0x1 << L2C_FLUSHSI_WF_BIT)
#define L2C_FLUSHSI_FMODE (0x3 << L2C_FLUSHSI_FMODE_BIT)

#define L2C_FLUSHSI_INDEX_BIT 16
#define L2C_FLUSHSI_TAG_BIT 10
#define L2C_FLUSHSI_FL_BIT 9
#define L2C_FLUSHSI_VB_BIT 8
#define L2C_FLUSHSI_DB_BIT 7
#define L2C_FLUSHSI_WAY_BIT 4
#define L2C_FLUSHSI_DI_BIT 3
#define L2C_FLUSHSI_WF_BIT 2
#define L2C_FLUSHSI_FMODE_BIT 0

#define L2C_FLUSHSI_FMODE_SET_INV_ONE (0x1 << L2C_FLUSHSI_FMODE_BIT)
#define L2C_FLUSHSI_FMODE_SET_WB_ONE (0x2 << L2C_FLUSHSI_FMODE_BIT)
#define L2C_FLUSHSI_FMODE_SET_INV_WB_ONE (0x3 << L2C_FLUSHSI_FMODE_BIT)
#define L2C_FLUSHSI_FMODE_WAY_UPDATE (0x1 << L2C_FLUSHSI_FMODE_BIT)
#define L2C_FLUSHSI_FMODE_WAY_WB (0x2 << L2C_FLUSHSI_FMODE_BIT)
#define L2C_FLUSHSI_FMODE_WAY_UPDATE_WB_ALL (0x3 << L2C_FLUSHSI_FMODE_BIT)

/*
 * L2CACHE ERROR register fields 
 */
#define L2C_ERROR_AHBM (0xf << L2C_ERROR_AHBM_BIT)
#define L2C_ERROR_SCRUB (0x1 << L2C_ERROR_SCRUB_BIT)
#define L2C_ERROR_TYPE (0x7 << L2C_ERROR_TYPE_BIT)
#define L2C_ERROR_TAG (0x1 << L2C_ERROR_TAG_BIT)
#define L2C_ERROR_COR (0x1 << L2C_ERROR_COR_BIT)
#define L2C_ERROR_MULTI (0x1 << L2C_ERROR_MULTI_BIT)
#define L2C_ERROR_VALID (0x1 << L2C_ERROR_VALID_BIT)
#define L2C_ERROR_DISERESP (0x1 << L2C_ERROR_DISERESP_BIT)
#define L2C_ERROR_CEC (0x7 << L2C_ERROR_CEC_BIT)
#define L2C_ERROR_IRQP (0xf << L2C_ERROR_IRQP_BIT)
#define L2C_ERROR_IRQM (0xf << L2C_ERROR_IRQM_BIT)
#define L2C_ERROR_IRQM_BCKEND (0x1 << L2C_ERROR_IRQM_BCKEND_BIT)
#define L2C_ERROR_IRQM_WPROT (0x1 << L2C_ERROR_IRQM_WPROT_BIT)
#define L2C_ERROR_IRQM_UNCORR (0x1 << L2C_ERROR_IRQM_UNCORR_BIT)
#define L2C_ERROR_IRQM_CORR (0x1 << L2C_ERROR_IRQM_CORR_BIT)
#define L2C_ERROR_SCB (0x3 << L2C_ERROR_SCB_BIT)
#define L2C_ERROR_STCB (0x3 << L2C_ERROR_STCB_BIT)
#define L2C_ERROR_XCB (0x1 << L2C_ERROR_XCB_BIT)
#define L2C_ERROR_RCB (0x1 << L2C_ERROR_RCB_BIT)
#define L2C_ERROR_COMP (0x1 << L2C_ERROR_COMP_BIT)
#define L2C_ERROR_RST (0x1 << L2C_ERROR_RST_BIT)

#define L2C_ERROR_AHBM_BIT 28
#define L2C_ERROR_SCRUB_BIT 27
#define L2C_ERROR_TYPE_BIT 24
#define L2C_ERROR_TAG_BIT 23
#define L2C_ERROR_COR_BIT 22
#define L2C_ERROR_MULTI_BIT 21
#define L2C_ERROR_VALID_BIT 20
#define L2C_ERROR_DISERESP_BIT 19
#define L2C_ERROR_CEC_BIT 16
#define L2C_ERROR_IRQP_BIT 12
#define L2C_ERROR_IRQM_BCKEND_BIT 11
#define L2C_ERROR_IRQM_WPROT_BIT 10
#define L2C_ERROR_IRQM_UNCORR_BIT 9
#define L2C_ERROR_IRQM_CORR_BIT 8
#define L2C_ERROR_IRQM_BIT 8
#define L2C_ERROR_SCB_BIT 6
#define L2C_ERROR_STCB_BIT 4
#define L2C_ERROR_XCB_BIT 3
#define L2C_ERROR_RCB_BIT 2
#define L2C_ERROR_COMP_BIT 1
#define L2C_ERROR_RST_BIT 0

/*
 * L2CACHE DATA CHECK BITS register fields 
 */
#define L2C_DCB_CB (0xfffffff << L2C_DCB_CB_BIT)

#define L2C_DCB_CB_BIT 0 

/*
 * L2CACHE SCRUB register fields 
 */
#define L2C_SCRUB_INDEX (0xffff << L2C_SCRUB_INDEX_BIT)
#define L2C_SCRUB_WAY (0x3 << L2C_SCRUB_WAY_BIT)
#define L2C_SCRUB_PEN (0x1 << L2C_SCRUB_PEN_BIT)
#define L2C_SCRUB_EN (0x1 << L2C_SCRUB_EN_BIT)

#define L2C_SCRUB_INDEX_BIT 16 
#define L2C_SCRUB_WAY_BIT 2
#define L2C_SCRUB_PEN_BIT 1
#define L2C_SCRUB_EN_BIT 0

/*
 * L2CACHE SCRUBDELAY register fields 
 */
#define L2C_SCRUB_DEL (0xffff << L2C_SCRUB_DEL_BIT)

#define L2C_SCRUB_DEL_BIT 0 

/*
 * L2CACHE ERROR INJECT register fields 
 */
#define L2C_ERRINJ_ADDR (0x3fffffff << L2C_ERRINJ_ADDR_BIT)
#define L2C_ERRINJ_EN (0x1 << L2C_ERRINJ_EN_BIT)

#define L2C_ERRINJ_ADDR_BIT 2
#define L2C_ERRINJ_EN_BIT 0

/*
 * L2CACHE ACCESS CONTROL register fields 
 */
#define L2C_ACCCTRL_DSC (0x1 << L2C_ACCCTRL_DSC_BIT)
#define L2C_ACCCTRL_SH (0x1 << L2C_ACCCTRL_SH_BIT)
#define L2C_ACCCTRL_SPLITQ (0x1 << L2C_ACCCTRL_SPLITQ_BIT)
#define L2C_ACCCTRL_NHM (0x1 << L2C_ACCCTRL_NHM_BIT)
#define L2C_ACCCTRL_BERR (0x1 << L2C_ACCCTRL_BERR_BIT)
#define L2C_ACCCTRL_OAPM (0x1 << L2C_ACCCTRL_OAPM_BIT)
#define L2C_ACCCTRL_FLINE (0x1 << L2C_ACCCTRL_FLINE_BIT)
#define L2C_ACCCTRL_DBPF (0x1 << L2C_ACCCTRL_DBPF_BIT)
#define L2C_ACCCTRL_128WF (0x1 << L2C_ACCCTRL_128WF_BIT)
#define L2C_ACCCTRL_DBPWS (0x1 << L2C_ACCCTRL_DBPWS_BIT)
#define L2C_ACCCTRL_SPLIT (0x1 << L2C_ACCCTRL_SPLIT_BIT)

#define L2C_ACCCTRL_DSC_BIT 14
#define L2C_ACCCTRL_SH_BIT 13
#define L2C_ACCCTRL_SPLITQ_BIT 10
#define L2C_ACCCTRL_NHM_BIT 9
#define L2C_ACCCTRL_BERR_BIT 8
#define L2C_ACCCTRL_OAPM_BIT 7
#define L2C_ACCCTRL_FLINE_BIT 6
#define L2C_ACCCTRL_DBPF_BIT 5
#define L2C_ACCCTRL_128WF_BIT 4
#define L2C_ACCCTRL_DBPWS_BIT 2
#define L2C_ACCCTRL_SPLIT_BIT 1

#ifdef TEST_L2CACHE
/*
 * L2CACHE TAG fields 
 */
#define L2C_TAG_TAG (0xfffffc << L2C_TAG_TAG_BIT)
#define L2C_TAG_VALID (0x3 << L2C_TAG_VALID_BIT)
#define L2C_TAG_DIRTY (0x3 << L2C_TAG_DIRTY_BIT)
#define L2C_TAG_LRU (0x3 << L2C_TAG_LRU_BIT)

#define L2C_TAG_TAG_BIT 10
#define L2C_TAG_VALID_BIT 8
#define L2C_TAG_DIRTY_BIT 6
#define L2C_TAG_LRU_BIT 0

#endif /* TEST_L2CACHE */

#define DEVNAME_LEN 9
/*
 * L2CACHE Driver private data struture
 */
struct l2cache_priv {
	struct drvmgr_dev	*dev;
	char devname[DEVNAME_LEN];

	/* L2CACHE control registers */
	struct l2c_regs	*regs;

	/* L2CACHE status */
	int ways;
	int waysize;
	int linesize;
	int index;
	int mtrr;
	int ft_support;
	int split_support;

	/* User defined ISR */
	l2cache_isr_t isr;
	void *isr_arg;
};

/*
 * L2CACHE internal prototypes 
 */
/* -Register access functions */
STATIC INLINE int l2cache_reg_ctrl_enable(void);
STATIC INLINE int l2cache_reg_ctrl_disable(void);
STATIC INLINE int l2cache_reg_ctrl_locked_set(int locked);
STATIC INLINE int l2cache_reg_ctrl_edac_set(int edac);
STATIC INLINE int l2cache_reg_ctrl_repl(int policy);
STATIC INLINE int l2cache_reg_ctrl_iway(int way);
STATIC INLINE int l2cache_reg_ctrl_writep(int policy);
STATIC INLINE unsigned int l2cache_reg_ctrl(void);
STATIC INLINE unsigned int l2cache_reg_status(void);
STATIC INLINE int l2cache_reg_mtrr_set(int index, unsigned int addr, 
		unsigned int mask, int options);
UNUSED STATIC INLINE unsigned int l2cache_reg_mtrr_get(int index);
STATIC INLINE int l2cache_reg_flushmem(unsigned int addr, int options);
STATIC INLINE int l2cache_reg_flushline(int way, int index, int options);
STATIC INLINE int l2cache_reg_flushway(unsigned int tag, int way, int options);
STATIC INLINE unsigned int l2cache_reg_error(void);
STATIC INLINE int l2cache_reg_error_reset(void);
STATIC INLINE int l2cache_reg_error_irqmask(int mask);
STATIC INLINE unsigned int l2cache_reg_error_addr(void);
STATIC INLINE unsigned int l2cache_reg_scrub(void);
STATIC INLINE int l2cache_reg_scrub_enable(int delay);
STATIC INLINE int l2cache_reg_scrub_disable(void);
STATIC INLINE unsigned int l2cache_reg_scrub_delay(void);
STATIC INLINE int l2cache_reg_scrub_line(int way, int index);
STATIC INLINE unsigned int l2cache_reg_accctrl(void);
STATIC INLINE int l2cache_reg_accctrl_split_disable(void);
STATIC INLINE int l2cache_reg_accctrl_split_enable(void);
#ifdef TEST_L2CACHE
STATIC INLINE int l2cache_reg_error_dcb(unsigned int cb);
STATIC INLINE int l2cache_reg_error_inject(unsigned int addr);
STATIC INLINE unsigned int l2cache_reg_diagtag(int way, int index);
STATIC INLINE unsigned int l2cache_reg_diagdata(int way, int index, int word);
STATIC unsigned int log2int(unsigned int v);
#endif /* TEST_L2CACHE */

/* -Control functions */
STATIC int l2cache_ctrl_status(void);
STATIC void l2cache_flushwait(void);

/* -Init function */
STATIC int l2cache_init(struct l2cache_priv *priv);

/* -Init function called by drvmgr */
int l2cache_init1(struct drvmgr_dev *dev);

/* -IRQ handler */
void l2cache_isr(void *arg);

/*
 * L2CACHE static members 
 */
static struct l2cache_priv *l2cachepriv = NULL;
#ifdef DEBUG
static char * repl_names[4] = {"LRU","Random","Master-Idx-1","Master-IDx-2"};
#endif

/* L2CACHE DRIVER */

struct drvmgr_drv_ops l2cache_ops = 
{
	.init = {l2cache_init1, NULL, NULL, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id l2cache_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_L2CACHE},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info l2cache_info =
{
	{
		DRVMGR_OBJ_DRV,					/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_L2CACHE_ID,/* Driver ID */
		"L2CACHE_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&l2cache_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct l2cache_priv),	/* Make drvmgr alloc private */
	},
	&l2cache_ids[0]
};

void l2cache_register_drv(void)
{
	DBG("Registering L2CACHE driver\n");
	drvmgr_drv_register(&l2cache_info.general);
}

/* Initializes the L2CACHE core and driver
 *
 * Return values
 *	0			  Successful initalization
 */
STATIC int l2cache_init(struct l2cache_priv *priv)
{
	struct ambapp_ahb_info *ahb;
	struct amba_dev_info *ainfo = priv->dev->businfo;

	/* Find L2CACHE core from Plug&Play information */
	ahb = ainfo->info.ahb_slv;

	/* Found L2CACHE core, init private structure */
	priv->regs = (struct l2c_regs *)ahb->start[1];

	/* Initialize L2CACHE status */
	unsigned int status = l2cache_reg_status();
	priv->ways = (status & L2C_STAT_WAY) + 1;
	priv->waysize = 
		((status & L2C_STAT_WAYSIZE) >> L2C_STAT_WAYSIZE_BIT) * 1024;
	priv->linesize = ((status & L2C_STAT_LS)? 64 : 32);
	priv->index = ((priv->waysize)/(priv->linesize));
	priv->mtrr = (status & L2C_STAT_MTRR) >> L2C_STAT_MTRR_BIT;
	priv->ft_support = (status & L2C_STAT_MP) >> L2C_STAT_MP_BIT;

	/* Probe split support. */
	int split_old = 0;
	int split_new = 0;
	split_old = (l2cache_reg_accctrl() & L2C_ACCCTRL_SPLIT);
	if (split_old){
		l2cache_reg_accctrl_split_disable();
	}else{
		l2cache_reg_accctrl_split_enable();
	}
	split_new = (l2cache_reg_accctrl() & L2C_ACCCTRL_SPLIT);
	if (split_old){
		l2cache_reg_accctrl_split_enable();
	}else{
		l2cache_reg_accctrl_split_disable();
	}
	priv->split_support = 
		((split_new ^ split_old) >> L2C_ACCCTRL_SPLIT_BIT) & 1;

	DBG("L2CACHE driver initialized\n");

	return 0;
}

/* Called when a core is found with the AMBA device and vendor ID 
 * given in l2cache_ids[]. IRQ, Console does not work here
 */
int l2cache_init1(struct drvmgr_dev *dev)
{
	int status;
	struct l2cache_priv *priv;

	DBG("L2CACHE[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	if (l2cachepriv) {
		DBG("Driver only supports one L2CACHE core\n");
		return DRVMGR_FAIL;
	}

	priv = dev->priv;
	if (!priv)
		return DRVMGR_NOMEM;

	priv->dev = dev;
	strncpy(&priv->devname[0], "l2cache0", DEVNAME_LEN);
	l2cachepriv = priv;

	/* Initialize L2CACHE Hardware */
	status = l2cache_init(priv);
	if (status) {
		printk("Failed to initialize l2cache driver %d\n", status);
		return -1;
	}

	return DRVMGR_OK;
}

STATIC INLINE int l2cache_reg_ctrl_enable(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	unsigned int ctrl = REG_READ(&priv->regs->control);
	REG_WRITE(&priv->regs->control, (ctrl | L2C_CTRL_EN));
	return 0;
}

STATIC INLINE int l2cache_reg_ctrl_disable(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	unsigned int ctrl = REG_READ(&priv->regs->control);
	REG_WRITE(&priv->regs->control, (ctrl & ~(L2C_CTRL_EN)));
	return 0;
}

STATIC INLINE int l2cache_reg_ctrl_repl(int policy)
{
	struct l2cache_priv *priv = l2cachepriv;

	unsigned int ctrl = REG_READ(&priv->regs->control);
	REG_WRITE(&priv->regs->control, 
		((ctrl & ~(L2C_CTRL_REPL)) | 
		 ((policy << L2C_CTRL_REPL_BIT) & L2C_CTRL_REPL))
	);
	return 0;
}

STATIC INLINE int l2cache_reg_ctrl_iway(int way)
{
	struct l2cache_priv *priv = l2cachepriv;

	unsigned int ctrl = REG_READ(&priv->regs->control);
	REG_WRITE(&priv->regs->control, 
		((ctrl & ~(L2C_CTRL_IWAY)) | 
		 ((way << L2C_CTRL_IWAY_BIT) & L2C_CTRL_IWAY))
	);
	return 0;
}

STATIC INLINE int l2cache_reg_ctrl_writep(int policy)
{
	struct l2cache_priv *priv = l2cachepriv;

	unsigned int ctrl = REG_READ(&priv->regs->control);
	REG_WRITE(&priv->regs->control, 
		((ctrl & ~(L2C_CTRL_WP)) | ((policy << L2C_CTRL_WP_BIT) & L2C_CTRL_WP))
	);
	return 0;
}

STATIC INLINE int l2cache_reg_ctrl_locked_set(int locked)
{
	struct l2cache_priv *priv = l2cachepriv;

	unsigned int ctrl = REG_READ(&priv->regs->control);
	ctrl = (ctrl & ~(L2C_CTRL_LOCK));
	REG_WRITE(&priv->regs->control, 
			ctrl | 
			((locked << L2C_CTRL_LOCK_BIT) & L2C_CTRL_LOCK));
	return 0;
}

STATIC INLINE int l2cache_reg_ctrl_edac_set(int edac)
{
	struct l2cache_priv *priv = l2cachepriv;

	unsigned int ctrl = REG_READ(&priv->regs->control);
	REG_WRITE(&priv->regs->control, 
			(ctrl & ~(L2C_CTRL_EDAC)) | 
			(edac? L2C_CTRL_EDAC:0));
	return 0;
}

STATIC INLINE unsigned int l2cache_reg_ctrl(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	return REG_READ(&priv->regs->control);
}

STATIC INLINE unsigned int l2cache_reg_status(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	return REG_READ(&priv->regs->status);
}

STATIC INLINE int l2cache_reg_mtrr_set(int index, unsigned int addr, 
		unsigned int mask, int options)
{
	struct l2cache_priv *priv = l2cachepriv;

	/* Set mtrr */
	addr = addr & L2C_MTRR_ADDR;
	mask = (mask >> 16) & L2C_MTRR_MASK; 
	options = ((options & ~(L2C_MTRR_ADDR)) & ~(L2C_MTRR_MASK));
	unsigned int mtrr = 0 | addr | mask | options;
	REG_WRITE(&priv->regs->mtrr[index], mtrr);
	return 0;
}

UNUSED STATIC INLINE unsigned int l2cache_reg_mtrr_get(int index)
{
	struct l2cache_priv *priv = l2cachepriv;

	return REG_READ(&priv->regs->mtrr[index]);
}

STATIC INLINE int l2cache_reg_flushmem(unsigned int addr, int options)
{
	struct l2cache_priv *priv = l2cachepriv;

	options = (options & ~(L2C_FLUSH_ADDR));
	REG_WRITE(&priv->regs->flush_mem_addr, (addr & L2C_FLUSH_ADDR) | options);
	return 0;
}

STATIC INLINE int l2cache_reg_flushline(int way, int index, int options)
{
	struct l2cache_priv *priv = l2cachepriv;

	options = 0 | (options & (L2C_FLUSHSI_FMODE));
	REG_WRITE(&priv->regs->flush_set_index, 
			((index << L2C_FLUSHSI_INDEX_BIT) & L2C_FLUSHSI_INDEX) |
			((way << L2C_FLUSHSI_WAY_BIT) & L2C_FLUSHSI_WAY) | 
			options
	);
	return 0;
}

STATIC INLINE int l2cache_reg_flushway(unsigned int tag, int way, int options)
{
	struct l2cache_priv *priv = l2cachepriv;

	options = (options & ~(L2C_FLUSHSI_TAG | L2C_FLUSHSI_WAY)) 
		| L2C_FLUSHSI_WF;
	REG_WRITE(&priv->regs->flush_set_index, 
			(tag & L2C_FLUSHSI_TAG) | 
			( (way << L2C_FLUSHSI_WAY_BIT) & L2C_FLUSHSI_WAY) | 
			options);
	return 0;
}

STATIC INLINE unsigned int l2cache_reg_error(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	return REG_READ(&priv->regs->error_status_control);
}

STATIC INLINE int l2cache_reg_error_reset(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	unsigned int ctrl = REG_READ(&priv->regs->error_status_control);
	REG_WRITE(&priv->regs->error_status_control, ctrl | L2C_ERROR_RST);
	return 0;
}

STATIC INLINE int l2cache_reg_error_irqmask(int mask)
{
	struct l2cache_priv *priv = l2cachepriv;

	unsigned int ctrl = REG_READ(&priv->regs->error_status_control);
	REG_WRITE(&priv->regs->error_status_control, 
			(ctrl & ~(L2C_ERROR_IRQM)) | (mask & L2C_ERROR_IRQM));
	return 0;
}

STATIC INLINE unsigned int l2cache_reg_error_addr(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	return REG_READ(&priv->regs->error_addr);
}

STATIC INLINE unsigned int l2cache_reg_scrub(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	return REG_READ(&priv->regs->scrub_control_status);
}

STATIC INLINE int l2cache_reg_scrub_enable(int delay)
{
	struct l2cache_priv *priv = l2cachepriv;

	unsigned int accc = REG_READ(&priv->regs->access_control);
	REG_WRITE(&priv->regs->access_control,
			accc | L2C_ACCCTRL_DSC | L2C_ACCCTRL_SH);

	unsigned int ctrl = REG_READ(&priv->regs->scrub_control_status);
	REG_WRITE(&priv->regs->scrub_delay, 
			(delay << L2C_SCRUB_DEL_BIT) & L2C_SCRUB_DEL);
	REG_WRITE(&priv->regs->scrub_control_status,  ctrl | L2C_SCRUB_EN);
	return 0;
}

STATIC INLINE int l2cache_reg_scrub_disable(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	unsigned int ctrl = REG_READ(&priv->regs->scrub_control_status);
	REG_WRITE(&priv->regs->scrub_control_status,  ctrl & ~(L2C_SCRUB_EN));
	return 0;
}

STATIC INLINE int l2cache_reg_scrub_line(int way, int index)
{
	struct l2cache_priv *priv = l2cachepriv;

	REG_WRITE(&priv->regs->scrub_control_status, 
			((index << L2C_SCRUB_INDEX_BIT) & L2C_SCRUB_INDEX) |
			((way << L2C_SCRUB_WAY_BIT) & L2C_SCRUB_WAY) |
			L2C_SCRUB_PEN);
	return 0;
}

STATIC INLINE unsigned int l2cache_reg_scrub_delay(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	return REG_READ(&priv->regs->scrub_delay);
}

STATIC INLINE unsigned int l2cache_reg_accctrl(void){
	struct l2cache_priv *priv = l2cachepriv;

	return REG_READ(&priv->regs->access_control);
}

STATIC INLINE int l2cache_reg_accctrl_split_disable(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	/* Disable split */
	unsigned int ctrl = REG_READ(&priv->regs->access_control);
	REG_WRITE(&priv->regs->access_control, (ctrl & ~(L2C_ACCCTRL_SPLIT)));
	return 0;
}

STATIC INLINE int l2cache_reg_accctrl_split_enable(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	/* Enable split */
	unsigned int ctrl = REG_READ(&priv->regs->access_control);
	REG_WRITE(&priv->regs->access_control, (ctrl | (L2C_ACCCTRL_SPLIT)));
	return 0;
}

STATIC INLINE int l2cache_ctrl_status(void)
{
	return ((l2cache_reg_ctrl() >> L2C_CTRL_EN_BIT) & 0x1);
}

STATIC void l2cache_flushwait(void)
{
	/* Read any L2cache register to wait until flush is done */
	/* The L2 will block any access until the flush is done */
	/* Force read operation */
	//asm volatile ("" : : "r" (l2cache_reg_status()));
	(void) l2cache_reg_status();
	return;
}

#ifdef TEST_L2CACHE
STATIC INLINE int l2cache_reg_error_dcb(unsigned int cb)
{
	struct l2cache_priv *priv = l2cachepriv;

	REG_WRITE(&priv->regs->data_check_bit, (cb & L2C_DCB_CB)); 
	return 0;
}

STATIC INLINE int l2cache_reg_error_inject(unsigned int addr)
{
	struct l2cache_priv *priv = l2cachepriv;

	REG_WRITE(&priv->regs->error_injection, 
			(addr & L2C_ERRINJ_ADDR) | L2C_ERRINJ_EN); 
	return 0;
}

STATIC INLINE unsigned int l2cache_reg_diagtag(int way, int index)
{
	struct l2cache_priv *priv = l2cachepriv;

	int offset = (index*8 + way);
	return REG_READ(&priv->regs->diag_iface_tag[offset]);
}

STATIC INLINE unsigned int l2cache_reg_diagdata(int way, int index, int word)
{
	struct l2cache_priv *priv = l2cachepriv;

	int offset = (index*(priv->linesize/4) + way*0x20000 + word);
	return REG_READ(&priv->regs->diag_iface_data[offset]);
}

STATIC unsigned int log2int(unsigned int v)
{
	unsigned r = 0;
	while (v >>= 1) {
		r++;
	}
	return r;
}

/* Return the index for a given addr */
int l2cache_get_index( uint32_t addr)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	return (addr % priv->waysize)/(priv->linesize);
}

/* Return the tag for a given addr */
uint32_t l2cache_get_tag( uint32_t addr)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	uint32_t tmp;
	int i = log2int(priv->waysize);
	tmp = (addr >> i);
	tmp = (tmp << i);
	return tmp;
}

int l2cache_lookup(uint32_t addr, int * way)
{
	struct l2cache_priv * priv = l2cachepriv;
	int i;
	struct l2cache_tag gottag;
	int ret;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	uint32_t exptag = l2cache_get_tag(addr);
	int index = l2cache_get_index(addr);

	/* Check all tags in the set */ 
	for(i=0; i< priv->ways; i++){
		ret = l2cache_diag_tag(i, index, &gottag);
		if (ret != L2CACHE_ERR_OK){
			return ret;
		}
		/*DBG("L2CACHE gottag: way=%d, valid=%d, tag=0x%08x.\n",
		 *		i, gottag.valid, gottag.tag);*/
		/* Check if valid */
		if (gottag.valid){
			/* Check if they are the same */
			if (gottag.tag == exptag){
				/* HIT! */
				if (way){
					*way = i;
				}
				DBG("L2CACHE lookup: index=%d, tag=0x%08x HIT way=%d.\n", 
						index, (unsigned int) exptag, i);
				return L2CACHE_HIT;
			}
		}
	}
	DBG("L2CACHE lookup: index=%d, tag=0x%08x MISS.\n", 
			index, (unsigned int) exptag);
	/* MISS! */
	return L2CACHE_MISS;
}

/* Diagnostic Accesses */
#define l2cache_tag_valid(val) ((val & L2C_TAG_VALID) >> L2C_TAG_VALID_BIT)
#define l2cache_tag_dirty(val) ((val & L2C_TAG_DIRTY) >> L2C_TAG_DIRTY_BIT)
#define l2cache_tag_lru(val) ((val & L2C_TAG_LRU) >> L2C_TAG_LRU_BIT)
int l2cache_diag_tag( int way, int index, struct l2cache_tag * tag)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (way >= priv->ways){
		DBG("L2CACHE has only %d ways.\n", priv->ways);
		return L2CACHE_ERR_EINVAL;
	}

	if (index >= priv->index){
		DBG("L2CACHE has only %d lines.\n", priv->index);
		return L2CACHE_ERR_EINVAL;
	}

	unsigned int val = l2cache_reg_diagtag(way,index);

	if (tag){
		tag->tag   = l2cache_get_tag(val);
		tag->valid = l2cache_tag_valid(val);
		tag->dirty = l2cache_tag_dirty(val);
		tag->lru   = l2cache_tag_lru(val);
	}else{
		return L2CACHE_ERR_EINVAL;
	}
	return L2CACHE_ERR_OK;
}

int l2cache_diag_line( int way, int index, struct l2cache_dataline * dataline)
{
	struct l2cache_priv * priv = l2cachepriv;
	int i;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (way >= priv->ways){
		DBG("L2CACHE has only %d ways.\n", priv->ways);
		return L2CACHE_ERR_EINVAL;
	}

	if (index >= priv->index){
		DBG("L2CACHE has only %d lines.\n", priv->index);
		return L2CACHE_ERR_EINVAL;
	}

	if (dataline){
		dataline->words = (priv->linesize/4);
		for (i=0; i< (priv->linesize/4); i++){
			dataline->data[i] = l2cache_reg_diagdata(way,index,i);
		}
	}else{
		return L2CACHE_ERR_EINVAL;
	}
	return L2CACHE_ERR_OK;
}

/* Inject an error on a given addr */
int l2cache_error_inject_address( uint32_t addr, uint32_t mask)
{
	struct l2cache_priv * priv = l2cachepriv;
	int word;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (!priv->ft_support){
		DBG("L2CACHE does not have EDAC support.\n");
		return L2CACHE_ERR_ERROR;
	}

	if (addr & 0x3){
		DBG("Address not aligned to 32-bit.\n");
		return L2CACHE_ERR_EINVAL;
	}

	/* Get word index */
	word = (addr % priv->linesize)/4;

	/* Shift mask to proper word */
	mask = (mask << (7*(priv->ways - word - 1)));

	/* Write DCB mask to XOR */
	l2cache_reg_error_dcb(mask);

	/* Inject error */
	l2cache_reg_error_inject(addr);

	DBG("L2CACHE error injected in 0x%08x (0x%08x).\n", 
			(unsigned int) addr, (unsigned int) mask);

	return L2CACHE_ERR_OK;
}

#endif /* TEST_L2CACHE */

/* L2CACHE Interrupt handler, called when there may be a L2CACHE interrupt.
 */
void l2cache_isr(void *arg)
{
	struct l2cache_priv *priv = arg;
	unsigned int sts = l2cache_reg_error();
	unsigned int addr = l2cache_reg_error_addr();

	/* Make sure that the interrupt is pending and unmasked,
	 * otherwise it migth have been other core
	 * sharing the same interrupt line */
	if ( ((sts & L2C_ERROR_IRQP) >> L2C_ERROR_IRQP_BIT) &
			((sts & L2C_ERROR_IRQM) >> L2C_ERROR_IRQM_BIT)){
		/* Reset error status */
		l2cache_reg_error_reset();
		/* Execute user IRQ (ther will always be one ISR */
		/* Give cacheline address */
		(priv->isr)(priv->isr_arg, (addr & ~(0x1f)), sts);
	}
}

/* Enable L2CACHE:
 */
int l2cache_enable(int flush)
{
	int ret;

	/* Flush checks flus parameter and INIT state */
	ret = l2cache_flush(flush);
	if (ret < 0){
		return ret;
	}

	l2cache_reg_ctrl_enable();

	DBG("L2CACHE enabled\n");
	return L2CACHE_ERR_OK;
}

/* Disable L2CACHE:
 */
int l2cache_disable(int flush)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if ((flush < 0) || 
			(flush > 
			 (L2CACHE_OPTIONS_FLUSH_INVALIDATE | L2CACHE_OPTIONS_FLUSH_WAIT))
			){
		DBG("L2CACHE wrong flush option.\n");
		return L2CACHE_ERR_EINVAL;
	}

	/* Flush & invalidate all cache. Also disable L2C */
	switch(flush & 0x3){
		case L2CACHE_OPTIONS_FLUSH_NONE:
			l2cache_reg_ctrl_disable();
			break;
		case L2CACHE_OPTIONS_FLUSH_INV_WBACK:
			l2cache_reg_flushmem(0, L2C_FLUSH_FMODE_INV_WB_ALL | L2C_FLUSH_DI);
			break;
		case L2CACHE_OPTIONS_FLUSH_WRITEBACK:
			l2cache_reg_flushmem(0, L2C_FLUSH_FMODE_WB_ALL | L2C_FLUSH_DI);
			break;
		case L2CACHE_OPTIONS_FLUSH_INVALIDATE:
		default:
			l2cache_reg_flushmem(0, L2C_FLUSH_FMODE_INV_ALL | L2C_FLUSH_DI);
			break;
	}

	if (flush & L2CACHE_OPTIONS_FLUSH_WAIT){
		l2cache_flushwait();
	}

	DBG("L2CACHE disabled\n");
	return L2CACHE_ERR_OK;
}

/* Status L2CACHE:
 */
int l2cache_status(void)
{
	struct l2cache_priv * priv = l2cachepriv;
	int status;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	unsigned int ctrl = l2cache_reg_ctrl();
	int locked = (ctrl & L2C_CTRL_LOCK) >> L2C_CTRL_LOCK_BIT;
	int enabled = ((ctrl & L2C_CTRL_EN) >> L2C_CTRL_EN_BIT) & 0x1;
	int edac = (ctrl & L2C_CTRL_EDAC) >> L2C_CTRL_EDAC_BIT;
	int repl = (ctrl & L2C_CTRL_REPL) >> L2C_CTRL_REPL_BIT;
	int writep = (ctrl & L2C_CTRL_WP) >> L2C_CTRL_WP_BIT;

	unsigned int acc = l2cache_reg_accctrl();
	int split = (acc & L2C_ACCCTRL_SPLIT) >> L2C_ACCCTRL_SPLIT_BIT;

	unsigned int err = l2cache_reg_error();
	int interrupts = (err & L2C_ERROR_IRQM) >> L2C_ERROR_IRQM_BIT;

	unsigned int scr = l2cache_reg_scrub();
	int scrub = (scr & L2C_SCRUB_EN) >> L2C_SCRUB_EN_BIT;

	unsigned int dly = l2cache_reg_scrub_delay();
	int delay = (dly & L2C_SCRUB_DEL) >> L2C_SCRUB_DEL_BIT;

	status = 0|
		(enabled? L2CACHE_STATUS_ENABLED: 0) |
		(split? L2CACHE_STATUS_SPLIT_ENABLED: 0) |
		(edac? L2CACHE_STATUS_EDAC_ENABLED: 0) |
		((repl & 0x3) << L2CACHE_STATUS_REPL_BIT) |
		(writep? L2CACHE_STATUS_WRITETHROUGH: 0) |
		((locked & 0xf) << L2CACHE_STATUS_LOCK_BIT) |
		((interrupts & 0xf) << L2CACHE_STATUS_INT_BIT) |
		(scrub? L2CACHE_STATUS_SCRUB_ENABLED: 0) |
		((delay & 0xffff) << L2CACHE_STATUS_SCRUB_DELAY_BIT);

	return status;
}

/* Flush L2CACHE:
 */
int l2cache_flush(int flush)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if ((flush < 0) || 
			(flush > 
			 (L2CACHE_OPTIONS_FLUSH_INVALIDATE | L2CACHE_OPTIONS_FLUSH_WAIT))
			){
		DBG("L2CACHE wrong flush option.\n");
		return L2CACHE_ERR_EINVAL;
	}

	switch(flush & 0x3){
		case L2CACHE_OPTIONS_FLUSH_NONE:
			break;
		case L2CACHE_OPTIONS_FLUSH_INV_WBACK:
			l2cache_reg_flushmem(0, L2C_FLUSH_FMODE_INV_WB_ALL);
			break;
		case L2CACHE_OPTIONS_FLUSH_WRITEBACK:
			l2cache_reg_flushmem(0, L2C_FLUSH_FMODE_WB_ALL);
			break;
		case L2CACHE_OPTIONS_FLUSH_INVALIDATE:
		default:
			l2cache_reg_flushmem(0, L2C_FLUSH_FMODE_INV_ALL);
			break;
	}

	if (flush & L2CACHE_OPTIONS_FLUSH_WAIT){
		l2cache_flushwait();
	}

	DBG("L2CACHE flushed\n");
	return L2CACHE_ERR_OK;
}

/* Flush L2CACHE address:
 */
int l2cache_flush_address(uint32_t addr, int size, int flush)
{
	struct l2cache_priv * priv = l2cachepriv;
	uint32_t endaddr;
	int options;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if ((flush < 0) || 
			(flush > 
			 (L2CACHE_OPTIONS_FLUSH_INVALIDATE | L2CACHE_OPTIONS_FLUSH_WAIT))
			){
		DBG("L2CACHE wrong flush option.\n");
		return L2CACHE_ERR_EINVAL;
	}

	if (size <= 0){
		DBG("L2CACHE wrong size.\n");
		return L2CACHE_ERR_EINVAL;
	}

	switch(flush & 0x3){
		case L2CACHE_OPTIONS_FLUSH_NONE:
			break;
		case L2CACHE_OPTIONS_FLUSH_INV_WBACK:
			options=L2C_FLUSH_FMODE_INV_WB_ONE;
			break;
		case L2CACHE_OPTIONS_FLUSH_WRITEBACK:
			options=L2C_FLUSH_FMODE_WB_ONE;
			break;
		case L2CACHE_OPTIONS_FLUSH_INVALIDATE:
		default:
			options=L2C_FLUSH_FMODE_INV_ONE;
			break;
	}

	if ( (flush & 0x3) == L2CACHE_OPTIONS_FLUSH_NONE){
		return L2CACHE_ERR_OK;
	}

	/* Get the end address */
	endaddr = (addr + size);

	/* Start on first cacheline address */
	addr = addr - (addr % priv->linesize);
	while( addr < endaddr){
		/* Flush address */
		l2cache_reg_flushmem(addr, options);
		/* Update next line */
		addr += priv->linesize;
	}

	if (flush & L2CACHE_OPTIONS_FLUSH_WAIT){
		l2cache_flushwait();
	}
	
	DBG("L2CACHE address range flushed\n");
	return L2CACHE_ERR_OK;
}

/* Flush L2CACHE line:
 */
int l2cache_flush_line(int way, int index, int flush)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if ((flush < 0) || 
			(flush > 
			 (L2CACHE_OPTIONS_FLUSH_INVALIDATE | L2CACHE_OPTIONS_FLUSH_WAIT))
			){
		DBG("L2CACHE wrong flush option.\n");
		return L2CACHE_ERR_EINVAL;
	}

	if ((index < 0) || (index >= priv->index)){
		DBG("L2CACHE only has %d lines.\n", priv->index);
		return L2CACHE_ERR_EINVAL;
	}

	if ((way < 0 ) || (way >= priv->ways)){
		DBG("L2CACHE only has %d ways.\n", priv->ways);
		return L2CACHE_ERR_EINVAL;
	}

	switch(flush & 0x3){
		case L2CACHE_OPTIONS_FLUSH_NONE:
			break;
		case L2CACHE_OPTIONS_FLUSH_INV_WBACK:
			l2cache_reg_flushline(way, index, 
					L2C_FLUSHSI_FMODE_SET_INV_WB_ONE);
			break;
		case L2CACHE_OPTIONS_FLUSH_WRITEBACK:
			l2cache_reg_flushline(way, index, L2C_FLUSHSI_FMODE_SET_WB_ONE);
			break;
		case L2CACHE_OPTIONS_FLUSH_INVALIDATE:
		default:
			l2cache_reg_flushline(way, index, L2C_FLUSHSI_FMODE_SET_INV_ONE);
			break;
	}

	if (flush & L2CACHE_OPTIONS_FLUSH_WAIT){
		l2cache_flushwait();
	}

	DBG("L2CACHE line [%d,%d] flushed\n", way, index);
	return L2CACHE_ERR_OK;
}

/* Flush L2CACHE way:
 */
int l2cache_flush_way(int way, int flush)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if ((flush < 0) || 
			(flush > 
			 (L2CACHE_OPTIONS_FLUSH_INVALIDATE | L2CACHE_OPTIONS_FLUSH_WAIT))
			){
		DBG("L2CACHE wrong flush option.\n");
		return L2CACHE_ERR_EINVAL;
	}

	if ((way < 0 ) || (way >= priv->ways)){
		DBG("L2CACHE only has %d ways.\n", priv->ways);
		return L2CACHE_ERR_EINVAL;
	}

	switch(flush & 0x3){
		case L2CACHE_OPTIONS_FLUSH_NONE:
			break;
		case L2CACHE_OPTIONS_FLUSH_INVALIDATE:
			l2cache_reg_flushway(0, way, L2C_FLUSHSI_FMODE_WAY_UPDATE);
			break;
		case L2CACHE_OPTIONS_FLUSH_WRITEBACK:
			l2cache_reg_flushway(0, way, L2C_FLUSHSI_FMODE_WAY_WB);
			break;
		case L2CACHE_OPTIONS_FLUSH_INV_WBACK:
		default:
			l2cache_reg_flushway(0, way, L2C_FLUSHSI_FMODE_WAY_UPDATE_WB_ALL);
			break;
	}

	if (flush & L2CACHE_OPTIONS_FLUSH_WAIT){
		l2cache_flushwait();
	}

	DBG("L2CACHE way [%d] flushed\n",way);
	return L2CACHE_ERR_OK;
}

/* Fill L2CACHE way:
 */
int l2cache_fill_way(int way, uint32_t tag, int options, int flush)
{
	struct l2cache_priv * priv = l2cachepriv;
	int flags;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if ((way < 0 ) || (way >= priv->ways)){
		DBG("L2CACHE only has %d ways.\n", priv->ways);
		return L2CACHE_ERR_EINVAL;
	}

	/* Check input parameters */
	if (tag & 0x000003ff){
		DBG("Only using bits 31:10 of Addr/Mask\n");
		return L2CACHE_ERR_EINVAL;
	}

	/* Perform the Way-flush */
	flags = ((options & L2CACHE_OPTIONS_FETCH)? L2C_FLUSHSI_FL:0) |
			 ((options & L2CACHE_OPTIONS_VALID)? L2C_FLUSHSI_VB:0) | 
			 ((options & L2CACHE_OPTIONS_DIRTY)? L2C_FLUSHSI_DB:0);

	/*DBG("L2CACHE lock way: Locked=%d, way=%d, option=0x%04x\n", 
	 *		locked, way, flags);*/

	switch(flush & 0x3){
		case L2CACHE_OPTIONS_FLUSH_NONE:
			break;
		case L2CACHE_OPTIONS_FLUSH_INVALIDATE:
			l2cache_reg_flushway(tag, way, 
					flags | L2C_FLUSHSI_FMODE_WAY_UPDATE);
			break;
		case L2CACHE_OPTIONS_FLUSH_WRITEBACK:
			l2cache_reg_flushway(tag, way, flags | L2C_FLUSHSI_FMODE_WAY_WB);
			break;
		case L2CACHE_OPTIONS_FLUSH_INV_WBACK:
		default:
			l2cache_reg_flushway(tag, way, 
					flags | L2C_FLUSHSI_FMODE_WAY_UPDATE_WB_ALL);
			break;
	}

	if (flush & L2CACHE_OPTIONS_FLUSH_WAIT){
		l2cache_flushwait();
	}

	DBG("Way[%d] filled with Tag 0x%08x\n", way, (unsigned int) tag);

	return L2CACHE_ERR_OK;
}

/* Lock L2CACHE way:
 */
int l2cache_lock_way(uint32_t tag, int options, int flush, int enable)
{
	struct l2cache_priv * priv = l2cachepriv;
	int enabled;
	int way;
	int locked;
	int flags;
	int ret;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	locked = L2CACHE_LOCKED_WAYS(l2cache_status());
	if (locked >= priv->ways){
		DBG("L2CACHE only has %d ways.\n", priv->ways);
		return L2CACHE_ERR_TOOMANY;
	}

	/* Check input parameters */
	if (tag & 0x000003ff){
		DBG("Only using bits 31:10 of Addr/Mask\n");
		return L2CACHE_ERR_EINVAL;
	}

	/* Check L2C status */
	enabled = l2cache_ctrl_status();
	
	/* Disable L2C */
	ret = l2cache_disable(flush);
	if (ret < 0){
		return ret;
	}

	/* Increase number of locked ways */
	locked++;
	way = priv->ways - locked;
	l2cache_reg_ctrl_locked_set(locked);

	/* Perform the Way-flush */
	flags = ((options & L2CACHE_OPTIONS_FETCH)? L2C_FLUSHSI_FL:0) |
			 ((options & L2CACHE_OPTIONS_VALID)? L2C_FLUSHSI_VB:0) | 
			 ((options & L2CACHE_OPTIONS_DIRTY)? L2C_FLUSHSI_DB:0);

	/*DBG("L2CACHE lock way: Locked=%d, way=%d, option=0x%04x\n",
	 *		locked, way, flags);*/

	switch(flush & 0x3){
		case L2CACHE_OPTIONS_FLUSH_NONE:
			break;
		case L2CACHE_OPTIONS_FLUSH_INVALIDATE:
			l2cache_reg_flushway(tag, way, 
					flags | L2C_FLUSHSI_FMODE_WAY_UPDATE);
			break;
		case L2CACHE_OPTIONS_FLUSH_WRITEBACK:
			l2cache_reg_flushway(tag, way, flags | L2C_FLUSHSI_FMODE_WAY_WB);
			break;
		case L2CACHE_OPTIONS_FLUSH_INV_WBACK:
		default:
			l2cache_reg_flushway(tag, way, 
					flags | L2C_FLUSHSI_FMODE_WAY_UPDATE_WB_ALL);
			break;
	}

	/* Reenable L2C if required */
	switch(enable){
		case L2CACHE_OPTIONS_ENABLE:
			l2cache_reg_ctrl_enable();
			break;
		case L2CACHE_OPTIONS_DISABLE:
			break;
		case L2CACHE_OPTIONS_NONE:
		default:
			if (enabled) {
				l2cache_reg_ctrl_enable();
			}
			break;
	}

	if (flush & L2CACHE_OPTIONS_FLUSH_WAIT){
		l2cache_flushwait();
	}

	DBG("Way[%d] locked with Tag 0x%08x\n", way, (unsigned int) tag);

	return L2CACHE_ERR_OK;
}

/* Unlock L2CACHE waw:
 */
int l2cache_unlock()
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	/* Set number of locked ways to 0*/
	l2cache_reg_ctrl_locked_set(0);

	DBG("L2CACHE ways unlocked\n");

	return L2CACHE_ERR_OK;
}

/* Setup L2CACHE:
 * Parameters:
 * -options: Can be:  
 */
int l2cache_mtrr_enable(int index, uint32_t addr, uint32_t mask, int options, 
		int flush)
{
	struct l2cache_priv * priv = l2cachepriv;
	int enabled;
	int flags;
	int ret;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (index < 0){
		DBG("Wrong index\n");
		return L2CACHE_ERR_EINVAL;
	}

	if (index >= priv->mtrr){
		DBG("Not enough MTRR registers\n");
		return L2CACHE_ERR_TOOMANY;
	}

	/* Check input parameters */
	if ((addr & 0x0003ffff) || (mask & 0x0003ffff)){
		DBG("Only using bits 31:18 of Addr/Mask\n");
		return L2CACHE_ERR_EINVAL;
	}

	/* Check L2C status */
	enabled = l2cache_ctrl_status();

	/* Disable L2C */
	ret = l2cache_disable(flush);
	if (ret < 0){
		return ret;
	}

	/* Decode options */
	flags = 0 |
		(options & L2CACHE_OPTIONS_MTRR_ACCESS_WRITETHROUGH? 
			L2C_MTRR_WRITETHROUGH	: 
			L2C_MTRR_UNCACHED)		|
		(options & L2CACHE_OPTIONS_MTRR_WRITEPROT_ENABLE? 
			L2C_MTRR_WRITEPROT_ENABLE	: 
			L2C_MTRR_WRITEPROT_DISABLE) |
		L2C_MTRR_ACCESSCONTROL_ENABLE;

	/* Configure mtrr */
	l2cache_reg_mtrr_set(index, addr, mask, flags); 

	/* Enable cache again (if needed) */
	if (enabled){
		l2cache_reg_ctrl_enable();
	}

	DBG("MTRR[%d] succesfully configured for 0x%08x (mask 0x%08x), "
			"access=%s, wprot=%s\n", 
		index, (unsigned int) addr, (unsigned int) mask, 
		(options & L2CACHE_OPTIONS_MTRR_ACCESS_WRITETHROUGH? 
				"WRITETHROUGH":"UNCACHED"),
		(options & L2CACHE_OPTIONS_MTRR_WRITEPROT_ENABLE? "ENABLE":"DISABLE")
		);

	return L2CACHE_ERR_OK;
}

/* Setup L2CACHE:
 * Parameters:
 * -options: Can be:  
 */
int l2cache_mtrr_disable(int index)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (index < 0){
		DBG("Wrong index\n");
		return L2CACHE_ERR_EINVAL;
	}

	if (index >= priv->mtrr){
		DBG("Not enough MTRR registers\n");
		return L2CACHE_ERR_TOOMANY;
	}

	/* Configure mtrr */
	l2cache_reg_mtrr_set(index, 0, 0, L2C_MTRR_ACCESSCONTROL_DISABLE); 

	DBG("MTRR[%d] disabled\n", index);

	return L2CACHE_ERR_OK;
}

/* Print L2CACHE status
 * DEBUG function
 */
int l2cache_print(void)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	#ifdef DEBUG
	int status = l2cache_status();
	if (status < 0){
		return status;
	}
	printf("L2cache: Ways:%d. Waysize:%d, Linesize:%d, Lines:%d\n"
		   "		 MTRR:%d, FT:%s, Locked:%d, Split:%s\n"
		   "		 REPL:%s, WP:%s, EDAC:%s, Enabled:%s\n"
		   "		 Scrub:%s, S-Delay:%d\n",
		priv->ways, 
		priv->waysize, 
		priv->linesize, 
		(priv->index * priv->ways), 
		priv->mtrr, 
		(priv->ft_support? "Available":"N/A"),
		L2CACHE_LOCKED_WAYS(status),
		(priv->split_support? (L2CACHE_SPLIT_ENABLED(status)? 
							   "Enabled":"Disabled"):"N/A"),
		repl_names[L2CACHE_REPL(status)],
		(L2CACHE_WRITETHROUGH(status)? "Write-through":"Write-back"),
		(L2CACHE_EDAC_ENABLED(status)? "Enabled":"Disabled"),
		(L2CACHE_ENABLED(status)? "Yes":"No"),
		(L2CACHE_SCRUB_ENABLED(status)? "Enabled":"Disabled"),
		L2CACHE_SCRUB_DELAY(status)
	);
	if (l2cache_ctrl_status()){
		printf("L2cache enabled.\n");
	}else{
		printf("L2cache disabled.\n");
	}
	#endif
	return L2CACHE_ERR_OK;
}

int l2cache_split_enable(void)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (!priv->split_support){
		DBG("L2CACHE does not have split support.\n");
		return L2CACHE_ERR_ERROR;
	}

	l2cache_reg_accctrl_split_enable();
	DBG("L2CACHE split is now enabled\n");

	return L2CACHE_ERR_OK;
}

int l2cache_split_disable(void)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (!priv->split_support){
		DBG("L2CACHE does not have split support.\n");
		return L2CACHE_ERR_ERROR;
	}

	l2cache_reg_accctrl_split_disable();
	DBG("L2CACHE split is now disabled\n");

	return L2CACHE_ERR_OK;
}

int l2cache_edac_enable(int flush)
{
	struct l2cache_priv * priv = l2cachepriv;
	int enabled;
	int ret;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (!priv->ft_support){
		DBG("L2CACHE does not have EDAC support.\n");
		return L2CACHE_ERR_ERROR;
	}

	/* Check that L2C is enabled */
	enabled = l2cache_ctrl_status();

	/* Disable&Flush L2C */
	ret = l2cache_disable(flush);
	if (ret < 0){
		return ret;
	}

	/* Clear error register */
	l2cache_reg_error_reset();

	/* Enable EDAC */
	l2cache_reg_ctrl_edac_set(1);

	/* Enable cache again */
	if (enabled){
		l2cache_reg_ctrl_enable();
	}

	DBG("L2CACHE EDAC is now enabled\n");

	return L2CACHE_ERR_OK;
}

int l2cache_edac_disable(int flush)
{
	struct l2cache_priv * priv = l2cachepriv;
	int enabled;
	int ret;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (!priv->ft_support){
		DBG("L2CACHE does not have EDAC support.\n");
		return L2CACHE_ERR_ERROR;
	}

	/* Check that L2C is enabled */
	enabled = l2cache_ctrl_status();

	/* Disable&Flush L2C */
	ret = l2cache_disable(flush);
	if (ret < 0){
		return ret;
	}

	/* Disable EDAC */
	l2cache_reg_ctrl_edac_set(0);

	/* Clear error register */
	l2cache_reg_error_reset();

	/* Enable cache again */
	if (enabled){
		l2cache_reg_ctrl_enable();
	}

	DBG("L2CACHE EDAC is now disabled\n");

	return L2CACHE_ERR_OK;
}

int l2cache_scrub_enable(int delay)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (!priv->ft_support){
		DBG("L2CACHE does not have EDAC support.\n");
		return L2CACHE_ERR_ERROR;
	}

	/* Enable Scrub */
	l2cache_reg_scrub_enable(delay);

	DBG("L2CACHE Scrub is now enabled\n");

	return L2CACHE_ERR_OK;
}

int l2cache_scrub_disable(void)
{
	struct l2cache_priv * priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (!priv->ft_support){
		DBG("L2CACHE does not have EDAC support.\n");
		return L2CACHE_ERR_ERROR;
	}

	/* Disable Scrub */
	l2cache_reg_scrub_disable();

	DBG("L2CACHE Scrub is now disabled\n");

	return L2CACHE_ERR_OK;
}

int l2cache_scrub_line(int way, int index)
{
	struct l2cache_priv * priv = l2cachepriv;
	unsigned int scrub;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (!priv->ft_support){
		DBG("L2CACHE does not have EDAC support.\n");
		return L2CACHE_ERR_ERROR;
	}

	if ((index < 0) || (index >= priv->index)){
		DBG("L2CACHE only has %d lines.\n", priv->index);
		return L2CACHE_ERR_EINVAL;
	}

	if ((way < 0) || (way >= priv->ways)){
		DBG("L2CACHE only has %d ways.\n", priv->ways);
		return L2CACHE_ERR_EINVAL;
	}

	/* Check pending bit */
	scrub = l2cache_reg_scrub();
	if ( (scrub & L2C_SCRUB_PEN) || (scrub & L2C_SCRUB_EN) ){
		DBG("L2CACHE already scrubbing.\n");
		return L2CACHE_ERR_ERROR;
	}

	/* Scrub line */
	l2cache_reg_scrub_line(way, index);

	DBG("L2CACHE Scrub line [%d,%d]\n",way,index);

	return L2CACHE_ERR_OK;
}

int l2cache_writethrough(int flush)
{
	struct l2cache_priv * priv = l2cachepriv;
	int enabled;
	int ret;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	/* Check that L2C is enabled */
	enabled = l2cache_ctrl_status();

	/* Disable&Flush L2C */
	ret = l2cache_disable(flush);
	if (ret < 0){
		return ret;
	}

	/* Configure writethrough */
	l2cache_reg_ctrl_writep(1);

	/* Enable cache again */
	if (enabled){
		l2cache_reg_ctrl_enable();
	}

	DBG("L2CACHE now is writethrough\n");

	return L2CACHE_ERR_OK;
}

int l2cache_writeback(int flush)
{
	struct l2cache_priv * priv = l2cachepriv;
	int enabled;
	int ret;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	/* Check that L2C is enabled */
	enabled = l2cache_ctrl_status();

	/* Disable&Flush L2C */
	ret = l2cache_disable(flush);
	if (ret < 0){
		return ret;
	}

	/* Configure writeback */
	l2cache_reg_ctrl_writep(0);

	/* Enable cache again */
	if (enabled){
		l2cache_reg_ctrl_enable();
	}

	DBG("L2CACHE now is writeback\n");

	return L2CACHE_ERR_OK;
}

int l2cache_replacement(int options, int flush)
{
	struct l2cache_priv * priv = l2cachepriv;
	int enabled;
	int ret;
	int way;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	/* Check that L2C is enabled */
	enabled = l2cache_ctrl_status();

	/* Disable&Flush L2C */
	ret = l2cache_disable(flush);
	if (ret < 0){
		return ret;
	}

	if ( (options & 0x3) == L2CACHE_OPTIONS_REPL_MASTERIDX_IDX){
		/* Set iway */
		way = (options >> 2) & 0x3;
		l2cache_reg_ctrl_iway(way);
	}

	/* Configure writeback */
	l2cache_reg_ctrl_repl(options & 0x3);

	/* Enable cache again */
	if (enabled){
		l2cache_reg_ctrl_enable();
	}

	DBG("L2CACHE replacement set to %d\n", (options & 0x3));

	return L2CACHE_ERR_OK;

}

int l2cache_isr_register(l2cache_isr_t isr, void * arg, int options)
{
	struct l2cache_priv *priv = l2cachepriv;
	unsigned int mask;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (isr == NULL){
		DBG("L2CACHE wrong isr.\n");
		return L2CACHE_ERR_EINVAL;
	}

	/* Get mask */
	mask = 0 |
		((options & L2CACHE_INTERRUPT_BACKENDERROR)? L2C_ERROR_IRQM_BCKEND:0) |
		((options & L2CACHE_INTERRUPT_WPROTHIT)? L2C_ERROR_IRQM_WPROT:0) |
		((options & L2CACHE_INTERRUPT_CORRERROR)? L2C_ERROR_IRQM_CORR:0) |
		((options & L2CACHE_INTERRUPT_UNCORRERROR)? L2C_ERROR_IRQM_UNCORR:0);

	/* Clear previous interrupts and mask them*/
	l2cache_reg_error_reset();
	l2cache_reg_error_irqmask(0);

	/* First time registering an ISR */
	if (priv->isr == NULL){
		/* Install and Enable L2CACHE interrupt handler */
		drvmgr_interrupt_register(priv->dev, 0, priv->devname, l2cache_isr, 
				priv);
	}

	/* Install user ISR */
	priv->isr=isr;
	priv->isr_arg=arg;

	/* Now it is safe to unmask interrupts */
	l2cache_reg_error_irqmask(mask);

	return L2CACHE_ERR_OK;
}

int l2cache_isr_unregister(void)
{
	struct l2cache_priv *priv = l2cachepriv;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (priv->isr == NULL){
		DBG("L2CACHE wrong isr.\n");
		return L2CACHE_ERR_EINVAL;
	}

	/* Clear previous interrupts and mask them*/
	l2cache_reg_error_reset();
	l2cache_reg_error_irqmask(0);

	/* Uninstall and disable L2CACHE interrupt handler */
	drvmgr_interrupt_unregister(priv->dev, 0, l2cache_isr, priv);

	/* Uninstall user ISR */
	priv->isr=NULL;
	priv->isr_arg=NULL;

	return L2CACHE_ERR_OK;
}

int l2cache_interrupt_unmask(int options)
{
	struct l2cache_priv *priv = l2cachepriv;
	unsigned int mask, irq;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	if (priv->isr == NULL){
		DBG("L2CACHE wrong isr.\n");
		return L2CACHE_ERR_EINVAL;
	}

	/* Unmask interrupts in  L2CACHE */
	mask = 0 |
		((options & L2CACHE_INTERRUPT_BACKENDERROR)? L2C_ERROR_IRQM_BCKEND:0) |
		((options & L2CACHE_INTERRUPT_WPROTHIT)? L2C_ERROR_IRQM_WPROT:0) |
		((options & L2CACHE_INTERRUPT_CORRERROR)? L2C_ERROR_IRQM_CORR:0) |
		((options & L2CACHE_INTERRUPT_UNCORRERROR)? L2C_ERROR_IRQM_UNCORR:0);

	/* Clear previous interrupts*/
	l2cache_reg_error_reset();

	/* Get previous mask */
	irq = ((l2cache_reg_error() & L2C_ERROR_IRQM) >> L2C_ERROR_IRQM_BIT);

	/* Set new mask */
	l2cache_reg_error_irqmask(irq | mask);

	return L2CACHE_ERR_OK;
}

int l2cache_interrupt_mask(int options)
{
	struct l2cache_priv *priv = l2cachepriv;
	unsigned int mask, irq;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	/* Mask interrupts in  L2CACHE */
	mask = 0 |
		((options & L2CACHE_INTERRUPT_BACKENDERROR)? L2C_ERROR_IRQM_BCKEND:0) |
		((options & L2CACHE_INTERRUPT_WPROTHIT)? L2C_ERROR_IRQM_WPROT:0) |
		((options & L2CACHE_INTERRUPT_CORRERROR)? L2C_ERROR_IRQM_CORR:0) |
		((options & L2CACHE_INTERRUPT_UNCORRERROR)? L2C_ERROR_IRQM_UNCORR:0);

	/* Clear previous interrupts */
	l2cache_reg_error_reset();

	/* Get previous mask */
	irq = ((l2cache_reg_error() & L2C_ERROR_IRQM) >> L2C_ERROR_IRQM_BIT);

	/* Set new mask */
	l2cache_reg_error_irqmask(irq & ~(mask));

	return L2CACHE_ERR_OK;
}

int l2cache_error_status(uint32_t * addr, uint32_t * status)
{
	struct l2cache_priv *priv = l2cachepriv;
	unsigned int sts;
	unsigned int erraddr;

	if (priv == NULL){
		DBG("L2CACHE not initialized.\n");
		return L2CACHE_ERR_NOINIT;
	}

	/* Get error register */
	sts = priv->regs->error_status_control;
	erraddr = priv->regs->error_addr;

	/* Check if an error occurred */
	if (sts & L2C_ERROR_VALID){
		/* Reset error register */
		l2cache_reg_error_reset();

		/* Update user variables if needed */
		if (addr != NULL){
			*addr = (erraddr & ~(0x1f));
		}

		if(status != NULL){
			*status = sts;
		}

		/* Return status */
		if (sts & L2C_ERROR_MULTI){
			return L2CACHE_STATUS_MULTIPLEERRORS;
		}else{
			return L2CACHE_STATUS_NEWERROR;
		}
	}else{
		/* Return status */
		return L2CACHE_STATUS_NOERROR;
	}
}
