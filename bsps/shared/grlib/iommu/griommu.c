/*
 *  GRIOMMU Driver Interface
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
#include <bsp.h>
#include <grlib/griommu.h>

#include <grlib/grlib_impl.h>

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
 * GRIOMMU CAP0 register fields 
 */
#define CAP0_A (0x1 << CAP0_A_BIT)
#define CAP0_AC (0x1 << CAP0_AC_BIT)
#define CAP0_CA (0x1 << CAP0_CA_BIT)
#define CAP0_CP (0x1 << CAP0_CP_BIT)
#define CAP0_NARB (0xf << CAP0_NARB_BIT)
#define CAP0_CS (0x1 << CAP0_CS_BIT)
#define CAP0_FT (0x3 << CAP0_FT_BIT)
#define CAP0_ST (0x1 << CAP0_ST_BIT)
#define CAP0_I (0x1 << CAP0_I_BIT)
#define CAP0_IT (0x1 << CAP0_IT_BIT)
#define CAP0_IA (0x1 << CAP0_IA_BIT)
#define CAP0_IP (0x1 << CAP0_IP_BIT)
#define CAP0_MB (0x1 << CAP0_MB_BIT)
#define CAP0_GRPS (0xf << CAP0_GRPS_BIT)
#define CAP0_MSTS (0xf << CAP0_MSTS_BIT)

#define CAP0_A_BIT 31
#define CAP0_AC_BIT 30
#define CAP0_CA_BIT 29
#define CAP0_CP_BIT 28
#define CAP0_NARB_BIT 20
#define CAP0_CS_BIT 19
#define CAP0_FT_BIT 17
#define CAP0_ST_BIT 16
#define CAP0_I_BIT 15
#define CAP0_IT_BIT 14
#define CAP0_IA_BIT 13
#define CAP0_IP_BIT 12
#define CAP0_MB_BIT 8
#define CAP0_GRPS_BIT 4
#define CAP0_MSTS_BIT 0

/*
 * GRIOMMU CAP1 register fields 
 */
#define CAP1_CADDR (0xfff << CAP1_CADDR_BIT)
#define CAP1_CMASK (0xf << CAP1_CMASK_BIT)
#define CAP1_CTAGBITS (0xff << CAP1_CTAGBITS_BIT)
#define CAP1_CISIZE (0x7 << CAP1_CISIZE_BIT)
#define CAP1_CLINES (0x1f << CAP1_CLINES_BIT)

#define CAP1_CADDR_BIT 20
#define CAP1_CMASK_BIT 16
#define CAP1_CTAGBITS_BIT 8
#define CAP1_CISIZE_BIT 5
#define CAP1_CLINES_BIT 0 

/*
 * GRIOMMU CTRL register fields 
 * DEFINED IN HEADER FILE
 */

/*
 * GRIOMMU FLUSH register fields 
 */
#define FLUSH_FGRP (0xf << FLUSH_FGRP_BIT)
#define FLUSH_GF (0x1 << FLUSH_GF_BIT)
#define FLUSH_F (0x1 << FLUSH_F_BIT)

#define FLUSH_FGRP_BIT 4
#define FLUSH_GF_BIT 1
#define FLUSH_F_BIT 0

/*
 * GRIOMMU STATUS register fields 
 */
#define STS_PE (0x1 << STS_PE_BIT)
#define STS_DE (0x1 << STS_DE_BIT)
#define STS_FC (0x1 << STS_FC_BIT)
#define STS_FL (0x1 << STS_FL_BIT)
#define STS_AD (0x1 << STS_AD_BIT)
#define STS_TE (0x1 << STS_TE_BIT)
#define STS_ALL (STS_PE | STS_DE | STS_FC | STS_FL | STS_AD | STS_TE)

#define STS_PE_BIT 5
#define STS_DE_BIT 4
#define STS_FC_BIT 3
#define STS_FL_BIT 2
#define STS_AD_BIT 1
#define STS_TE_BIT 0

/*
 * GRIOMMU IMASK register fields 
 */
#define IMASK_PEI (0x1 << IMASK_PEI_BIT)
#define IMASK_FCI (0x1 << IMASK_FCI_BIT)
#define IMASK_FLI (0x1 << IMASK_FLI_BIT)
#define IMASK_ADI (0x1 << IMASK_ADI_BIT)
#define IMASK_TEI (0x1 << IMASK_TEI_BIT)
#define IMASK_ALL (IMASK_PEI | IMASK_FCI | IMASK_FLI | IMASK_ADI | IMASK_TEI)

#define IMASK_PEI_BIT 5
#define IMASK_FCI_BIT 3
#define IMASK_FLI_BIT 2
#define IMASK_ADI_BIT 1
#define IMASK_TEI_BIT 0

/*
 * GRIOMMU MASTER register fields 
 */
/* DEFINED IN HEADER FILE 
#define MASTER_VENDOR (0xff << MASTER_VENDOR_BIT)
#define MASTER_DEVICE (0xfff << MASTER_DEVICE_BIT)
#define MASTER_BS (0x1 << MASTER_BS_BIT)
#define MASTER_GROUP (0xf << MASTER_GROUP_BIT)

#define MASTER_VENDOR_BIT 24
#define MASTER_DEVICE_BIT 12
#define MASTER_BS_BIT 4
#define MASTER_GROUP_BIT 0
*/

#define MASTER_BS_BUS0 0
#define MASTER_BS_BUS1 MASTER_BS

/*
 * GRIOMMU GROUP register fields 
 */
#define GRP_BASE (0xfffffff << GRP_BASE_BIT)
#define GRP_P (0x1 << GRP_P_BIT)
#define GRP_AG (0x1 << GRP_AG_BIT)

#define GRP_BASE_BIT 4
#define GRP_P_BIT 1
#define GRP_AG_BIT 0


#define REG_WRITE(addr, val) (*(volatile unsigned int *)(addr) = (unsigned int)(val))
#define REG_READ(addr) (*(volatile unsigned int *)(addr))

/*
 * GRIOMMU APB Register MAP
 */
struct griommu_regs {
	volatile unsigned int cap0;			/* 0x00 - Capability 0 */
	volatile unsigned int cap1;			/* 0x04 - Capability 1 */
	volatile unsigned int cap2;			/* 0x08 - Capability 2 */
	volatile unsigned int resv1;		/* 0x0c - Reserved */
	volatile unsigned int ctrl;			/* 0x10 - Control */
	volatile unsigned int flush;		/* 0x14 - TLB/cache flush */
	volatile unsigned int status;		/* 0x18 - Status */
	volatile unsigned int imask;		/* 0x1c - Interrupt mask */
	volatile unsigned int ahbstat;		/* 0x20 - AHB Failing Access */
	volatile unsigned int resv2[7];		/* 0x24-0x3c - Reserved. No access */
	volatile unsigned int master[16];	/* 0x40-0x7c - Master configuration */
	volatile unsigned int grp_ctrl[16]; /* 0x80-0xbc - Group control */
	volatile unsigned int diag_ca;		/* 0xc0 - Diagnostic cache access */
	volatile unsigned int diag_cad[8];	/* 0xc4-0xe0 - Diagnostic cache data */
	volatile unsigned int diag_cat;		/* 0xe4 - Diagnostic cache tag */
	volatile unsigned int ei_data;		/* 0xe8 - Data RAM error injection */
	volatile unsigned int ei_tag;		/* 0xec - Tag RAM error injection */
	volatile unsigned int resv3[4];		/* 0xf0-0xfc - Reserved. No access */
	volatile unsigned int asmpctrl[16]; /* 0x100-0x13c - ASMP access control */
};

#define DEVNAME_LEN 9
/*
 * GRIOMMU Driver private data struture
 */
struct griommu_priv {
	struct drvmgr_dev	*dev;
	char devname[DEVNAME_LEN];
	/* GRIOMMU control registers */
	struct griommu_regs	*regs;

	/* GRIOMMU capabilities */
	int apv;
	int apv_cache;
	int apv_cache_addr;
	int conf_pagesize;

	int groups;
	int masters;

	/* GRIOMMU page size */
	int pagesize;

	/* GRIOMMU APV cache */
	int cache_enabled;
	int group_addressing;

	/* User defined ISR */
	griommu_isr_t isr;
	void *isr_arg;
};

/*
 * GRIOMMU internal prototypes 
 */
/* -Register access functions */
STATIC INLINE unsigned int griommu_reg_cap0(void);
STATIC INLINE unsigned int griommu_reg_cap1(void);
STATIC INLINE unsigned int griommu_reg_ctrl(void);
STATIC INLINE int griommu_reg_ctrl_set(unsigned int val);
STATIC INLINE int griommu_reg_flush_set(unsigned int val);
STATIC INLINE unsigned int griommu_reg_status(void);
STATIC INLINE int griommu_reg_status_clear(unsigned int val);
STATIC INLINE unsigned int griommu_reg_imask(void);
STATIC INLINE int griommu_reg_imask_set(int mask);
STATIC INLINE unsigned int griommu_reg_ahbfas(void);
STATIC INLINE unsigned int griommu_reg_master(int master);
STATIC INLINE int griommu_reg_master_set(int master, unsigned int val);
STATIC INLINE unsigned int griommu_reg_group(int group);
STATIC INLINE int griommu_reg_group_set(int group, unsigned int val);

/* APV helper functions */
STATIC void griommu_apv_set_word(unsigned int * wordptr, int startbitidx, 
		int nbits, unsigned int val);
STATIC int griommu_apv_set(void * apv, int index, int size, unsigned int val);

/* -Init function called by drvmgr */
int griommu_init1(struct drvmgr_dev *dev);
STATIC int griommu_init(struct griommu_priv *priv);


/* -IRQ handler */
void griommu_isr(void *arg);

/*
 * GRIOMMU static members 
 */
static struct griommu_priv *griommupriv = NULL;

/* GRIOMMU DRIVER */

struct drvmgr_drv_ops griommu_ops = 
{
	.init = {griommu_init1, NULL, NULL, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id griommu_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_GRIOMMU},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info griommu_info =
{
	{
		DRVMGR_OBJ_DRV,					/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_GRIOMMU_ID,/* Driver ID */
		"GRIOMMU_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&griommu_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct griommu_priv),	/* Make drvmgr alloc private */
	},
	&griommu_ids[0]
};

void griommu_register_drv(void)
{
	DBG("Registering GRIOMMU driver\n");
	drvmgr_drv_register(&griommu_info.general);
}

/* Initializes the GRIOMMU core and driver
 *
 * Return values
 *	0			  Successful initalization
 */
STATIC int griommu_init(struct griommu_priv *priv)
{
	struct ambapp_ahb_info *ahb;
	struct amba_dev_info *ainfo = priv->dev->businfo;

	/* Find GRIOMMU core from Plug&Play information */
	ahb = ainfo->info.ahb_slv;

	/* Found GRIOMMU core, init private structure */
	priv->regs = (struct griommu_regs *)ahb->start[0];

	/* Mask all interrupts */
	griommu_reg_imask_set(0);

	/* Initialize GRIOMMU capabilities */
	uint32_t cap0 = griommu_reg_cap0();
	priv->apv = (cap0 & CAP0_A) >> CAP0_A_BIT;
	priv->apv_cache = (cap0 & CAP0_AC) >> CAP0_AC_BIT;
	priv->apv_cache_addr = (cap0 & CAP0_CA) >> CAP0_CA_BIT;
	priv->conf_pagesize = (cap0 & CAP0_CS) >> CAP0_CS_BIT;
	priv->groups = ((cap0 & CAP0_GRPS) >> CAP0_GRPS_BIT) + 1;
	priv->masters = ((cap0 & CAP0_MSTS) >> CAP0_MSTS_BIT) + 1;

	/* Get GRIOMMU pagesize */
	uint32_t ctrl = griommu_reg_ctrl();
	if (priv->conf_pagesize){
		priv->pagesize = (4*1024 << ((ctrl & CTRL_PGSZ) >> CTRL_PGSZ_BIT));
	}else{
		priv->pagesize = 4*1024;
	}
	priv->cache_enabled = (ctrl & CTRL_CE);
	priv->group_addressing = (ctrl & CTRL_GS);

	DBG("GRIOMMU Capabilities: APV=%d, APVC=%d, APVCA=%d, CS=%d, "
			"GRPS=%d, MSTS=%d\n",
			priv->apv, priv->apv_cache, priv->apv_cache_addr, 
			priv->conf_pagesize, priv->groups, priv->masters);
	DBG("GRIOMMU driver initialized\n");

	return 0;
}

/* Called when a core is found with the AMBA device and vendor ID 
 * given in griommu_ids[]. IRQ, Console does not work here
 */
int griommu_init1(struct drvmgr_dev *dev)
{
	int status;
	struct griommu_priv *priv;

	DBG("GRIOMMU[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	if (griommupriv) {
		DBG("Driver only supports one GRIOMMU core\n");
		return DRVMGR_FAIL;
	}

	priv = dev->priv;
	if (!priv)
		return DRVMGR_NOMEM;

	priv->dev = dev;
	strncpy(&priv->devname[0], "griommu0", DEVNAME_LEN);
	griommupriv = priv;

	/* Initialize GRIOMMU Hardware */
	status = griommu_init(priv);
	if (status) {
		printk("Failed to initialize griommu driver %d\n", status);
		return -1;
	}

	return DRVMGR_OK;
}

STATIC INLINE unsigned int griommu_reg_cap0(void)
{
	struct griommu_priv *priv = griommupriv;

	return REG_READ(&priv->regs->cap0);
}

STATIC INLINE unsigned int griommu_reg_cap1(void)
{
	struct griommu_priv *priv = griommupriv;

	return REG_READ(&priv->regs->cap1);
}

STATIC INLINE unsigned int griommu_reg_ctrl(void)
{
	struct griommu_priv *priv = griommupriv;

	return REG_READ(&priv->regs->ctrl);
}

STATIC INLINE int griommu_reg_ctrl_set(unsigned int val)
{
	struct griommu_priv *priv = griommupriv;

	REG_WRITE(&priv->regs->ctrl, val);
	return 0;
}

STATIC INLINE int griommu_reg_flush_set(unsigned int val)
{
	struct griommu_priv *priv = griommupriv;

	REG_WRITE(&priv->regs->flush, val);
	return 0;
}

STATIC INLINE unsigned int griommu_reg_status(void)
{
	struct griommu_priv *priv = griommupriv;

   return REG_READ(&priv->regs->status);
}

STATIC INLINE int griommu_reg_status_clear(unsigned int val)
{
	struct griommu_priv *priv = griommupriv;

	/* Clear errors */
	REG_WRITE(&priv->regs->status, (val & STS_ALL));
	return 0;
}

STATIC INLINE unsigned int griommu_reg_imask(void)
{
	struct griommu_priv *priv = griommupriv;

   return REG_READ(&priv->regs->imask);
}

STATIC INLINE int griommu_reg_imask_set(int mask)
{
	struct griommu_priv *priv = griommupriv;

	/* Clear errors */
	REG_WRITE(&priv->regs->imask, (mask & IMASK_ALL));
	return 0;
}

STATIC INLINE unsigned int griommu_reg_ahbfas(void)
{
	struct griommu_priv *priv = griommupriv;

   return REG_READ(&priv->regs->ahbstat);
}

STATIC INLINE int griommu_reg_master_set(int master, unsigned int val)
{
	struct griommu_priv *priv = griommupriv;

	/* Change master conf */
	REG_WRITE(&priv->regs->master[master], val);
	return 0;
}

STATIC INLINE unsigned int griommu_reg_master(int master)
{
	struct griommu_priv *priv = griommupriv;

	return REG_READ(&priv->regs->master[master]);
}

STATIC INLINE unsigned int griommu_reg_group(int group)
{
	struct griommu_priv *priv = griommupriv;

	return REG_READ(&priv->regs->grp_ctrl[group]);
}

STATIC INLINE int griommu_reg_group_set(int group, unsigned int val)
{
	struct griommu_priv *priv = griommupriv;

	REG_WRITE(&priv->regs->grp_ctrl[group], val);
	return 0;
}

STATIC void griommu_apv_set_word(unsigned int * wordptr, int startbitidx, 
		int nbits, unsigned int val)
{
	unsigned int mask;
	unsigned int word = *wordptr;
	int endbitidx = startbitidx + nbits - 1;

	/* Set initial mask */
	mask = 0xffffffff;

	/* Adjust mask for the starting bit */
	mask >>= startbitidx;

	/* Adjust mask for the end bit */
	mask >>= (31 - endbitidx);
	mask <<= (31 - endbitidx);

	DBG("Setting word: startbitdx=%d, endbitidx=%d, mask=0x%02x",
			startbitidx, endbitidx, (unsigned int) mask);

	/* Clear written bits with mask */
	word &= ~(mask);

	/* Set bits in val with mask */
	mask &= val;
	word |= mask;

	DBG(", old word=0x%08x, new word=0x%08x\n",*wordptr, word);

	/* Write word */
	*wordptr=word;
}

/* Set certains bits of the APV to val */
STATIC int griommu_apv_set(void * apv, int index, int size, unsigned int val)
{
	unsigned int * words = (unsigned int *) apv;
	int len = size;
	int wordidx = (index/32);
	int startbit = (index % 32);
	int nbits;
	int nwords;

	/* First incomplete word is a special case */
	if (startbit != 0){
		/* Get how many bits are we changing in this word */
		if (startbit + len  < 32){
			nbits = len;
		}else{
			nbits = 32 - startbit;
		}
		griommu_apv_set_word(&words[wordidx], startbit, nbits, val);
		DBG("First word: wordidx=%d, startbit=%d, bits=%d, val=0x%08x\n", 
				wordidx, startbit, nbits, words[wordidx]);

		/* Update wordidx and len */
		len = len - nbits;
		wordidx++;
	}

	/* Write all complete full words */
	if (len != 0){
		nwords = (len/32);
		memset((void *) &words[wordidx], val, nwords*4);
		DBG("Middle words: wordidx=%d, nwords=%d\n", wordidx, nwords);
		/* Update wordidx and len*/
		wordidx = wordidx + nwords;
		len = len - nwords*32;
	}

	/* Last word is a special case */
	if (len != 0){
		nbits = len;
		griommu_apv_set_word(&words[wordidx], 0, nbits, val);
		DBG("First word: wordidx=%d, startbit=%d, bits=%d, val=0x%08x\n", 
				wordidx, 0, nbits, words[wordidx]);
		/* Update len */
		len = len - (nbits);
	}

	return GRIOMMU_ERR_OK;
}

/* GRIOMMU Interrupt handler, called when there may be a GRIOMMU interrupt.
 */
void griommu_isr(void *arg)
{
	struct griommu_priv *priv = arg;
	unsigned int sts = griommu_reg_status();
	unsigned int mask = griommu_reg_imask();
	unsigned int access = griommu_reg_ahbfas();

	/* Make sure that the interrupt is pending and unmasked,
	 * otherwise it migth have been other core
	 * sharing the same interrupt line */
	if ((sts & STS_ALL) & (mask & IMASK_ALL)){
		/* Reset error status */
		griommu_reg_status_clear(sts);
		/* Execute user IRQ (ther will always be one ISR */
		(priv->isr)(priv->isr_arg, access, sts);
	}
}

/* Setup IOMMU master:
 */
int griommu_master_setup(int master, int group, int options)
{
	struct griommu_priv * priv = griommupriv;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	if ((master < 0) || (master >= priv->masters)){
		DBG("Wrong master id.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	if ((group < 0) || (group >= priv->groups)){
		DBG("Wrong group id.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	griommu_reg_master_set(master,
			((options & GRIOMMU_OPTIONS_BUS1)? MASTER_BS_BUS1: MASTER_BS_BUS0)|
			((group << MASTER_GROUP_BIT) & MASTER_GROUP)
			);

	DBG("IOMMU master setup: master %d, traffic routed %s, group %d\n",
			master,
			(options & GRIOMMU_OPTIONS_BUS1) ? 
					"to Secondary bus":"to Primary bus",
			group);

	return GRIOMMU_ERR_OK;
}


/* Get IOMMU master info:
 */
int griommu_master_info(int master, uint32_t * info)
{
	struct griommu_priv * priv = griommupriv;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	if ((master < 0) || (master >= priv->masters)){
		DBG("Wrong master id.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	if (info == NULL){
		DBG("Wrong pointer.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	/* Get master */
	*info = griommu_reg_master(master);

	return GRIOMMU_ERR_OK;
}

/* Find IOMMU master:
 */
int griommu_master_find(int vendor, int device, int instance)
{
	struct griommu_priv * priv = griommupriv;
	int i, gotvendor, gotdevice;
	unsigned int master;
	int found;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	/* Find which master */
	found=0;
	for (i=0; i< priv->masters; i++){
		master = griommu_reg_master(i);
		gotvendor = (master & MASTER_VENDOR) >> MASTER_VENDOR_BIT;
		gotdevice = (master & MASTER_DEVICE) >> MASTER_DEVICE_BIT;
		if ((gotvendor == vendor) && (gotdevice == device)){
			if(found == instance){
				DBG("Found master %d: VENDOR=%s(0x%02x), DEVICE=%s(0x%03x), "
					"Instance=%d\n",
					i,
					ambapp_vendor_id2str(vendor), vendor,
					ambapp_device_id2str(vendor,device), device, instance
					);
				return i;
			}
			found++;
		}
	}

	DBG("Master not found: VENDOR=%s(0x%02x), DEVICE=%s(0x%03x), "
		"Instance=%d\n",
		ambapp_vendor_id2str(vendor), vendor,
		ambapp_device_id2str(vendor,device), device, instance
		);
	return GRIOMMU_ERR_NOTFOUND;
}

/* Setup IOMMU:
 */
int griommu_setup(int options)
{
	struct griommu_priv * priv = griommupriv;
	unsigned int ctrl;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	/* Check Cache */
	if (options & GRIOMMU_OPTIONS_CACHE_ENABLE) {
		if (priv->apv_cache){
			/* Flush cache */
			griommu_reg_flush_set(FLUSH_F);
			priv->cache_enabled = 1;
		}else{
			DBG("GRIOMMU APV cache not supported.\n");
			return GRIOMMU_ERR_IMPLEMENTED;
		}
	}else{
		priv->cache_enabled = 0;
	}

	/* Check group addressing */
	if (options & GRIOMMU_OPTIONS_GROUPADDRESSING_ENABLE){
		if (priv->apv_cache_addr){
			priv->group_addressing = 1;
		}else{
			DBG("GRIOMMU APV cache group addressing not supported.\n");
			return GRIOMMU_ERR_IMPLEMENTED;
		}
	}else{
		priv->group_addressing = 0;
	}

	/* Check pagesize */
	if ((options & CTRL_PGSZ) != GRIOMMU_OPTIONS_PAGESIZE_4KIB){
		if (priv->conf_pagesize == 0){
			DBG("GRIOMMU Configurable pagesize not supported.\n");
			return GRIOMMU_ERR_IMPLEMENTED;
		}
	}

	/* Get CTRL IOMMU */
	ctrl = griommu_reg_ctrl();

	/* Clear used fields */
	ctrl &= ~(CTRL_CE | CTRL_GS | CTRL_PGSZ | CTRL_LB | 
			CTRL_DP | CTRL_AU | CTRL_WP);

	/* Clear not used fields */
	options &= (CTRL_CE | CTRL_GS | CTRL_PGSZ | CTRL_LB | 
			CTRL_DP | CTRL_AU | CTRL_WP);

	/* Set new values */
	ctrl |= options;

	/* Set CTRL IOMMU */
	griommu_reg_ctrl_set(ctrl);

	DBG("IOMMU setup: prefetching %s, cache %s, groupaddr %s, "
		"lookup bus %s, ahb update %s,\nwprot only %s, pagesize %d KiB\n",
			((options & GRIOMMU_OPTIONS_PREFETCH_DISABLE)? 
					"disabled":"enabled"),
			((options & GRIOMMU_OPTIONS_CACHE_ENABLE)? "enabled":"disabled"),
			((options & GRIOMMU_OPTIONS_GROUPADDRESSING_ENABLE)? 
					"enabled":"disabled"),
			((options & GRIOMMU_OPTIONS_LOOKUPBUS_BUS1)? "bus1":"bus0"),
			((options & GRIOMMU_OPTIONS_AHBUPDATE_ENABLE)? 
					"enabled":"disabled"),
			((options & GRIOMMU_OPTIONS_WPROTONLY_ENABLE)? 
					"enabled":"disabled"),
			(4 << ((options & GRIOMMU_OPTIONS_PAGESIZE_512KIB) >> 18))
		);

	return GRIOMMU_ERR_OK;
}

/* Status IOMMU:
 */
int griommu_status(void)
{
	struct griommu_priv * priv = griommupriv;
	unsigned int ctrl;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	/* Get CTRL IOMMU */
	ctrl = griommu_reg_ctrl();

	DBG("IOMMU status: prefetching %s, cache %s, groupaddr %s, "
		"lookup bus %s, ahb update %s,\nwprot only %s, pagesize %d KiB\n",
			((ctrl & CTRL_DP)? "disabled":"enabled"),
			((ctrl & CTRL_CE)? "enabled":"disabled"),
			((ctrl & CTRL_GS)? "enabled":"disabled"),
			((ctrl & CTRL_LB)? "bus1":"bus0"),
			((ctrl & CTRL_AU)? "enabled":"disabled"),
			((ctrl & CTRL_WP)? "enabled":"disabled"),
			(4 << ((ctrl & CTRL_PGSZ) >> CTRL_PGSZ_BIT))
		);

	return ctrl;
}

int griommu_isr_register(griommu_isr_t isr, void * arg, int options)
{
	struct griommu_priv *priv = griommupriv;
	unsigned int mask;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	if (isr == NULL){
		DBG("GRIOMMU wrong isr.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	/* Get mask */
	mask = 0 |
		((options & GRIOMMU_INTERRUPT_PARITY_ERROR)? IMASK_PEI:0) |
		((options & GRIOMMU_INTERRUPT_FLUSH_COMPLETED)? IMASK_FCI:0) |
		((options & GRIOMMU_INTERRUPT_FLUSH_START)? IMASK_FLI:0) |
		((options & GRIOMMU_INTERRUPT_ACCESS_DENIED)? IMASK_ADI:0) |
		((options & GRIOMMU_INTERRUPT_TRANSLATION_ERROR)? IMASK_TEI:0);

	/* Clear previous interrupts and mask them*/
	griommu_reg_status_clear(STS_ALL);
	griommu_reg_imask_set(0);

	/* First time registering an ISR */
	if (priv->isr == NULL){
		/* Install and Enable GRIOMMU interrupt handler */
		drvmgr_interrupt_register(priv->dev, 0, priv->devname, griommu_isr, 
				priv);
	}

	/* Install user ISR */
	priv->isr=isr;
	priv->isr_arg=arg;

	/* Now it is safe to unmask interrupts */
	griommu_reg_imask_set(mask);

	return GRIOMMU_ERR_OK;
}

int griommu_isr_unregister(void)
{
	struct griommu_priv *priv = griommupriv;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	if (priv->isr == NULL){
		DBG("GRIOMMU wrong isr.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	/* Clear previous interrupts and mask them*/
	griommu_reg_status_clear(STS_ALL);
	griommu_reg_imask_set(0);

	/* Uninstall and disable GRIOMMU interrupt handler */
	drvmgr_interrupt_unregister(priv->dev, 0, griommu_isr, priv);

	/* Uninstall user ISR */
	priv->isr=NULL;
	priv->isr_arg=NULL;

	return GRIOMMU_ERR_OK;
}

int griommu_interrupt_unmask(int options)
{
	struct griommu_priv *priv = griommupriv;
	unsigned int mask, irq;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	if (priv->isr == NULL){
		DBG("GRIOMMU wrong isr.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	/* Unmask interrupts in GRIOMMU */
	mask = 0 |
		((options & GRIOMMU_INTERRUPT_PARITY_ERROR)? IMASK_PEI:0) |
		((options & GRIOMMU_INTERRUPT_FLUSH_COMPLETED)? IMASK_FCI:0) |
		((options & GRIOMMU_INTERRUPT_FLUSH_START)? IMASK_FLI:0) |
		((options & GRIOMMU_INTERRUPT_ACCESS_DENIED)? IMASK_ADI:0) |
		((options & GRIOMMU_INTERRUPT_TRANSLATION_ERROR)? IMASK_TEI:0);

	/* Clear previous interrupts*/
	griommu_reg_status_clear(STS_ALL);

	/* Get previous mask */
	irq = griommu_reg_imask() & IMASK_ALL;

	/* Set new mask */
	griommu_reg_imask_set(irq | mask);

	return GRIOMMU_ERR_OK;
}

int griommu_interrupt_mask(int options)
{
	struct griommu_priv *priv = griommupriv;
	unsigned int mask, irq;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	if (priv->isr == NULL){
		DBG("GRIOMMU wrong isr.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	/* Mask interrupts in GRIOMMU */
	mask = 0 |
		((options & GRIOMMU_INTERRUPT_PARITY_ERROR)? IMASK_PEI:0) |
		((options & GRIOMMU_INTERRUPT_FLUSH_COMPLETED)? IMASK_FCI:0) |
		((options & GRIOMMU_INTERRUPT_FLUSH_START)? IMASK_FLI:0) |
		((options & GRIOMMU_INTERRUPT_ACCESS_DENIED)? IMASK_ADI:0) |
		((options & GRIOMMU_INTERRUPT_TRANSLATION_ERROR)? IMASK_TEI:0);

	/* Clear previous interrupts*/
	griommu_reg_status_clear(STS_ALL);

	/* Get previous mask */
	irq = griommu_reg_imask() & IMASK_ALL;

	/* Set new mask */
	griommu_reg_imask_set(irq & ~(mask));

	return GRIOMMU_ERR_OK;
}

int griommu_error_status(uint32_t * access)
{
	struct griommu_priv *priv = griommupriv;
	int status;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	/* Get status mask */
	status = griommu_reg_status();

	if (status != 0){
		/* Update pointed value */
		if (access != NULL){
			*access = griommu_reg_ahbfas();
		}
		/* Clear errors */
		griommu_reg_status_clear(status);
	}

	return status;
}

/* Print IOMMU masters
 * DEBUG function
 */
int griommu_print(void)
{
	#ifdef DEBUG
	struct griommu_priv * priv = griommupriv;
	unsigned int ctrl;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	/* Print IOMMU status */
	ctrl = griommu_reg_ctrl();

	printf("IOMMU status: prefetching %s, lookup bus %s, ahb update %s,\n"
			"wprot only %s, pagesize %d KiB\n",
			((ctrl & CTRL_DP)? "disabled":"enabled"),
			((ctrl & CTRL_LB)? "bus1":"bus0"),
			((ctrl & CTRL_AU)? "enabled":"disabled"),
			((ctrl & CTRL_WP)? "enabled":"disabled"),
			(4 << ((ctrl & CTRL_PGSZ) >> CTRL_PGSZ_BIT))
		);

	/* Print each master configuration */
	int i, vendor, device, routing;
	unsigned int master;
	for (i=0; i < priv->masters; i++){
		master = griommu_reg_master(i);
		vendor = (master & MASTER_VENDOR) >> MASTER_VENDOR_BIT;
		device = (master & MASTER_DEVICE) >> MASTER_DEVICE_BIT;
		routing = (master & MASTER_BS);
		printf("IOMMU master %d: VENDOR=%s(0x%02x), DEVICE=%s(0x%03x), "
				"BS=%s\n",
				i,
				ambapp_vendor_id2str(vendor), vendor,
				ambapp_device_id2str(vendor,device), device,
				(routing == MASTER_BS_BUS0? "Primary bus" : "Secondary bus")
				);
	}
	#endif
	return GRIOMMU_ERR_OK;
}

void * griommu_apv_new(void)
{
	struct griommu_priv * priv = griommupriv;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return NULL;
	}

	/* Allocate APV */
	unsigned int * orig_ptr = grlib_malloc(
			(GRIOMMU_APV_SIZE/priv->pagesize) + GRIOMMU_APV_ALIGN);
	if (orig_ptr == NULL) return NULL;

	/* Get the aligned pointer */
	unsigned int aligned_ptr = (
		((unsigned int) orig_ptr + GRIOMMU_APV_ALIGN) &
		~(GRIOMMU_APV_ALIGN - 1));

	/* Save the original pointer before the aligned pointer */
	unsigned int ** tmp_ptr = 
		(unsigned int **) (aligned_ptr - sizeof(orig_ptr));
	*tmp_ptr= orig_ptr;

	/* Return aligned pointer */
	return (void *) aligned_ptr;
}

void griommu_apv_delete(void * apv)
{
	/* Recover orignal pointer placed just before the aligned pointer */
	unsigned int * orig_ptr;
	unsigned int ** tmp_ptr =  (unsigned int **) (apv - sizeof(orig_ptr));
	orig_ptr = *tmp_ptr;

	/* Deallocate memory */
	free(orig_ptr);
}

int griommu_enable(int mode)
{
	struct griommu_priv * priv = griommupriv;
	unsigned int ctrl;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	switch (mode){
		case GRIOMMU_MODE_IOMMU:
		default:
			DBG("IOMMU mode not implemented in driver.\n");
			return GRIOMMU_ERR_EINVAL;
			break;
		case GRIOMMU_MODE_GROUPAPV:
			if (priv->apv == 0){
				DBG("IOMMU APV not supported.\n");
				return GRIOMMU_ERR_IMPLEMENTED;
			}
			/* Enable IOMMU */
			ctrl = (griommu_reg_ctrl() & ~(CTRL_PM));
			griommu_reg_ctrl_set(ctrl | CTRL_PM_APV | CTRL_EN);

			/* Wait until change has effect */
			while((griommu_reg_ctrl() & CTRL_EN)==0){};

			DBG("IOMMU enabled.\n");
			return GRIOMMU_ERR_OK;
			break;
	}
	return GRIOMMU_ERR_OK;
}

int griommu_disable(void)
{
	struct griommu_priv * priv = griommupriv;
	unsigned int ctrl;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	/* Disable IOMMU */
	ctrl = (griommu_reg_ctrl() & ~(CTRL_EN));
	griommu_reg_ctrl_set(ctrl);

	/* Wait until change has effect */
	while(griommu_reg_ctrl() & CTRL_EN){};

	return GRIOMMU_ERR_OK;
}

int griommu_group_setup(int group, void * apv, int options)
{
	struct griommu_priv * priv = griommupriv;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	if ((group < 0) || (group >= priv->groups)){
		DBG("Wrong group id.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	if ((options < 0) || (options > GRIOMMU_OPTIONS_GROUP_PASSTHROUGH)){
		DBG("Wrong options.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	if (options == GRIOMMU_OPTIONS_GROUP_DISABLE){
		if ((unsigned int) apv & (GRIOMMU_APV_ALIGN -1)){
			DBG("Wrong pointer.\n");
			return GRIOMMU_ERR_EINVAL;
		}

		/* Disable GROUP */
		griommu_reg_group_set(group, (((unsigned int) apv) & GRP_BASE) | 0);
		DBG("GROUP[%d] DISABLED.\n", group);
		return GRIOMMU_ERR_OK;
	}else if (options == GRIOMMU_OPTIONS_GROUP_PASSTHROUGH){
		if ((unsigned int) apv & (GRIOMMU_APV_ALIGN -1)){
			DBG("Wrong pointer.\n");
			return GRIOMMU_ERR_EINVAL;
		}

		/* Group in passthrough */
		griommu_reg_group_set(group, 
				(((unsigned int) apv) & GRP_BASE) | GRP_P | GRP_AG);
		DBG("GROUP[%d] set to PASSTHROUGH.\n", group);
		return GRIOMMU_ERR_OK;
	}else{
		if (priv->apv == 0){
			DBG("IOMMU APV not supported.\n");
			return GRIOMMU_ERR_IMPLEMENTED;
		}

		if ((apv == NULL) || ((unsigned int) apv & (GRIOMMU_APV_ALIGN -1))){
			DBG("Wrong pointer.\n");
			return GRIOMMU_ERR_EINVAL;
		}

		/* Set up base and enable */
		griommu_reg_group_set(group, 
				(((unsigned int) apv) & GRP_BASE) | GRP_AG);
		DBG("GROUP[%d] set to APV (0x%08x).\n", group, (unsigned int) apv);
		return GRIOMMU_ERR_OK;
	}
}

int griommu_group_apv_init(int group, int options)
{
	struct griommu_priv * priv = griommupriv;
	void * apv;
	int val;
	int ret;
	size_t len;

	/* Flush APV cache if needed.
	 * This function checks for priv and group being valid.*/
	ret = griommu_group_apv_flush(group);
	if (ret < 0){
		return ret;
	}

	/* Get APV group */
	apv = (void *) (griommu_reg_group(group) & GRP_BASE);

	if (apv == NULL){
		DBG("Wrong pointer.\n");
		return GRIOMMU_ERR_NOTFOUND;
	}

	/* Get init value (is a char) */
	if (options == GRIOMMU_OPTIONS_APV_ALLOW){
		val = 0x00;
	}else{
		val = 0xff;
	}

	/* Get APV length */
	len = GRIOMMU_APV_SIZE/priv->pagesize;

	/* Initialize structure */
	memset(apv, val, len);

	return GRIOMMU_ERR_OK;
}

int griommu_group_apv_page_set(int group, int index, int size, int options)
{
	void * apv;
	unsigned int val;
	int ret;

	/* Flush APV cache if needed.
	 * This function checks for priv and group being valid.*/
	ret = griommu_group_apv_flush(group);
	if (ret < 0){
		return ret;
	}

	/* Get APV group */
	apv = (void *) (griommu_reg_group(group) & GRP_BASE);

	if (apv == NULL){
		DBG("Wrong pointer.\n");
		return GRIOMMU_ERR_NOTFOUND;
	}

	/* Get init value */
	if (options == GRIOMMU_OPTIONS_APV_ALLOW){
		val = 0x0;
	}else{
		val = 0xffffffff;
	}

	return griommu_apv_set(apv, index, size, val);
}

int griommu_group_apv_address_set(int group, uint32_t addr, int size, 
		int options)
{
	struct griommu_priv * priv = griommupriv;
	void * apv;
	unsigned int val;
	int ret;
	int startpage;
	int endpage;
	int npages;

	/* Flush APV cache if needed.
	 * This function checks for priv and group being valid.*/
	ret = griommu_group_apv_flush(group);
	if (ret < 0){
		return ret;
	}

	/* Get APV group */
	apv = (void *) (griommu_reg_group(group) & GRP_BASE);

	if (apv == NULL){
		DBG("Wrong pointer.\n");
		return GRIOMMU_ERR_NOTFOUND;
	}

	/* Get init value */
	if (options == GRIOMMU_OPTIONS_APV_ALLOW){
		val = 0x0;
	}else{
		val = 0xffffffff;
	}

	/* Get start page */
	startpage = (addr / priv->pagesize);

	/* Get end page */
	endpage = ((addr + size)/ priv->pagesize);

	/* Get number of pages */
	npages = endpage - startpage + 1;

	return griommu_apv_set(apv, startpage, npages, val);
}

int griommu_apv_init(void * apv, int options)
{
	struct griommu_priv * priv = griommupriv;
	int val;
	size_t len;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	if ((apv == NULL) || ((unsigned int) apv & (GRIOMMU_APV_ALIGN -1))){
		DBG("Wrong pointer.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	/* Get init value (is a char) */
	if (options == GRIOMMU_OPTIONS_APV_ALLOW){
		val = 0x00;
	}else{
		val = 0xff;
	}

	/* Get APV length */
	len = GRIOMMU_APV_SIZE/priv->pagesize;

	/* Initialize structure */
	memset(apv, val, len);

	return GRIOMMU_ERR_OK;
}

int griommu_apv_page_set(void * apv, int index, int size, int options)
{
	struct griommu_priv * priv = griommupriv;
	unsigned int val;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	if ((apv == NULL) || ((unsigned int) apv & (GRIOMMU_APV_ALIGN -1))){
		DBG("Wrong pointer.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	/* Get init value */
	if (options == GRIOMMU_OPTIONS_APV_ALLOW){
		val = 0x0;
	}else{
		val = 0xffffffff;
	}

	return griommu_apv_set(apv, index, size, val);
}

int griommu_apv_address_set(void * apv, uint32_t addr, int size, int options)
{
	struct griommu_priv * priv = griommupriv;
	unsigned int val;
	int startpage;
	int endpage;
	int npages;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	if ((apv == NULL) || ((unsigned int) apv & (GRIOMMU_APV_ALIGN -1))){
		DBG("Wrong pointer.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	/* Get init value */
	if (options == GRIOMMU_OPTIONS_APV_ALLOW){
		val = 0x0;
	}else{
		val = 0xffffffff;
	}

	/* Get start page */
	startpage = (addr / priv->pagesize);

	/* Get end page */
	endpage = ((addr + size)/ priv->pagesize);

	/* Get number of pages */
	npages = endpage - startpage + 1;

	return griommu_apv_set(apv, startpage, npages, val);
}

int griommu_group_info(int group, uint32_t * info)
{
	struct griommu_priv * priv = griommupriv;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	if ((group < 0) || (group >= priv->groups)){
		DBG("Wrong group id.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	if (info == NULL){
		DBG("Wrong pointer.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	/* Get group */
	*info = griommu_reg_group(group);

	return GRIOMMU_ERR_OK;
}

/* Flush APV cache group:
 */
int griommu_group_apv_flush(int group)
{
	struct griommu_priv * priv = griommupriv;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	if ((group < 0) || (group >= priv->groups)){
		DBG("Wrong group id.\n");
		return GRIOMMU_ERR_EINVAL;
	}

	/* Flush cache	*/
	if (priv->cache_enabled){
		if (priv->group_addressing){
			griommu_reg_flush_set(((group << FLUSH_FGRP_BIT) & FLUSH_FGRP) | 
					FLUSH_GF | FLUSH_F);
		}else{
			griommu_reg_flush_set(FLUSH_F);
		}
		DBG("GRIOMMU APV cache flushed.\n");
	}

	return GRIOMMU_ERR_OK;
}

/* Flush APV cache:
 */
int griommu_apv_flush(void)
{
	struct griommu_priv * priv = griommupriv;

	if (priv == NULL){
		DBG("GRIOMMU not initialized.\n");
		return GRIOMMU_ERR_NOINIT;
	}

	/* Flush cache	*/
	if (priv->cache_enabled){
		griommu_reg_flush_set(FLUSH_F);
		DBG("GRIOMMU APV cache flushed.\n");
	}

	return GRIOMMU_ERR_OK;
}

