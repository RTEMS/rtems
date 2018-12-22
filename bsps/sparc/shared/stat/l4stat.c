/* L4STAT APB-Register Driver.
 *
 * COPYRIGHT (c) 2017.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <rtems/libio.h>
#include <stdio.h>
#include <bsp.h>
#include <rtems/bspIo.h> /* printk */

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/l4stat.h>

/*#define STATIC*/
#define STATIC static

/*#define DEBUG 1*/

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...) 
#endif

#define REG_WRITE(addr, val) (*(volatile unsigned int *)(addr) = (unsigned int)(val))
#define REG_READ(addr) (*(volatile unsigned int *)(addr))


/*
 * L4STAT CCTRL register fields
 * DEFINED IN HEADER file
 */

struct l4stat_regs {
	unsigned int cval[32];				/* 0x000 */
	unsigned int cctrl[32];				/* 0x080 */
	unsigned int cmax[32];				/* 0x100 */
	unsigned int timestamp;				/* 0x180 */
};

struct l4stat_priv {
	struct drvmgr_dev *dev;

	/* L4STAT control registers */
	struct l4stat_regs *regs;

	/* L4STAT driver register */
	char devname[9];

	int ncpu;
	int ncnt;

	/* L4stat capabilities */
	int max_count_support;
	int internalahb_event_support;
	int dsu_event_support;
	int external_event_support;
	int ahbtrace_event_support;
};

STATIC struct l4stat_priv *l4statpriv = NULL;

/* Event names */
#ifdef DEBUG
#define L4STAT_BAD_CMD "N/A. Wrong event"
STATIC const char *l4stat_event_names[] = {
	"Instruction cache miss",				/* 0x00 */
	"Instruction MMU TLB miss",				/* 0x01 */
	"Instruction cache hold",				/* 0x02 */
	"Instruction MMU hold",					/* 0x03 */
	L4STAT_BAD_CMD,						/* 0x04 */
	L4STAT_BAD_CMD,						/* 0x05 */
	L4STAT_BAD_CMD,						/* 0x06 */
	L4STAT_BAD_CMD,						/* 0x07 */
	"Data cache (read) miss",				/* 0x08 */
	"Data MMU TLB miss",					/* 0x09 */
	"Data cache hold",					/* 0x0a */
	"Data MMU hold",					/* 0x0b */
	L4STAT_BAD_CMD,						/* 0x0c */
	L4STAT_BAD_CMD,						/* 0x0d */
	L4STAT_BAD_CMD,						/* 0x0e */
	L4STAT_BAD_CMD,						/* 0x0f */
	"Data write buffer hold",				/* 0x10 */
	"Total instruction count",				/* 0x11 */
	"Integer instruction count",				/* 0x12 */
	"Floating-point unit instruction count",		/* 0x13 */
	"Branch prediction miss",				/* 0x14 */
	"Execution time, exluding debug mode",			/* 0x15 */
	L4STAT_BAD_CMD,						/* 0x16 */
	"AHB utilization (per AHB master)",			/* 0x17 */
	"AHB utilization (total)",				/* 0x18 */
	L4STAT_BAD_CMD,						/* 0x19 */
	L4STAT_BAD_CMD,						/* 0x1a */
	L4STAT_BAD_CMD,						/* 0x1b */
	L4STAT_BAD_CMD,						/* 0x1c */
	L4STAT_BAD_CMD,						/* 0x1d */
	L4STAT_BAD_CMD,						/* 0x1e */
	L4STAT_BAD_CMD,						/* 0x1f */
	L4STAT_BAD_CMD,						/* 0x20 */
	L4STAT_BAD_CMD,						/* 0x21 */
	"Integer branches",					/* 0x22 */
	L4STAT_BAD_CMD,						/* 0x23 */
	L4STAT_BAD_CMD,						/* 0x24 */
	L4STAT_BAD_CMD,						/* 0x25 */
	L4STAT_BAD_CMD,						/* 0x26 */
	L4STAT_BAD_CMD,						/* 0x27 */
	"CALL instructions",					/* 0x28 */
	L4STAT_BAD_CMD,						/* 0x29 */
	L4STAT_BAD_CMD,						/* 0x2a */
	L4STAT_BAD_CMD,						/* 0x2b */
	L4STAT_BAD_CMD,						/* 0x2c */
	L4STAT_BAD_CMD,						/* 0x2d */
	L4STAT_BAD_CMD,						/* 0x2e */
	L4STAT_BAD_CMD,						/* 0x2f */
	"Regular type 2 instructions",				/* 0x30 */
	L4STAT_BAD_CMD,						/* 0x31 */
	L4STAT_BAD_CMD,						/* 0x32 */
	L4STAT_BAD_CMD,						/* 0x33 */
	L4STAT_BAD_CMD,						/* 0x34 */
	L4STAT_BAD_CMD,						/* 0x35 */
	L4STAT_BAD_CMD,						/* 0x36 */
	L4STAT_BAD_CMD,						/* 0x37 */
	"LOAD and STORE instructions",				/* 0x38 */
	"LOAD instructions",					/* 0x39 */
	"STORE instructions",					/* 0x3a */
	L4STAT_BAD_CMD,						/* 0x3b */
	L4STAT_BAD_CMD,						/* 0x3c */
	L4STAT_BAD_CMD,						/* 0x3d */
	L4STAT_BAD_CMD,						/* 0x3e */
	L4STAT_BAD_CMD,						/* 0x3f */
	"AHB IDLE cycles",					/* 0x40 */
	"AHB BUSY cycles",					/* 0x41 */
	"AHB Non-Seq. transfers",				/* 0x42 */
	"AHB Seq. transfers",					/* 0x43 */
	"AHB read accesses",					/* 0x44 */
	"AHB write accesses",					/* 0x45 */
	"AHB byte accesses",					/* 0x46 */
	"AHB half-word accesses",				/* 0x47 */
	"AHB word accesses",					/* 0x48 */
	"AHB double word accesses",				/* 0x49 */
	"AHB quad word accesses",				/* 0x4A */
	"AHB eight word accesses",				/* 0x4B */
	"AHB waitstates",					/* 0x4C */
	"AHB RETRY responses",					/* 0x4D */
	"AHB SPLIT responses",					/* 0x4E */
	"AHB SPLIT delay",					/* 0x4F */
	"AHB bus locked",					/* 0x50 */
	L4STAT_BAD_CMD,						/* 0x51 */
	L4STAT_BAD_CMD,						/* 0x52 */
	L4STAT_BAD_CMD,						/* 0x53 */
	L4STAT_BAD_CMD,						/* 0x54 */
	L4STAT_BAD_CMD,						/* 0x55 */
	L4STAT_BAD_CMD,						/* 0x56 */
	L4STAT_BAD_CMD,						/* 0x57 */
	L4STAT_BAD_CMD,						/* 0x58 */
	L4STAT_BAD_CMD,						/* 0x59 */
	L4STAT_BAD_CMD,						/* 0x5a */
	L4STAT_BAD_CMD,						/* 0x5b */
	L4STAT_BAD_CMD,						/* 0x5c */
	L4STAT_BAD_CMD,						/* 0x5d */
	L4STAT_BAD_CMD,						/* 0x5e */
	L4STAT_BAD_CMD,						/* 0x5f */
	"external event 0",					/* 0x60 */
	"external event 1",					/* 0x61 */
	"external event 2",					/* 0x62 */
	"external event 3",					/* 0x63 */
	"external event 4",					/* 0x64 */
	"external event 5",					/* 0x65 */
	"external event 6",					/* 0x66 */
	"external event 7",					/* 0x67 */
	"external event 8",					/* 0x68 */
	"external event 9",					/* 0x69 */
	"external event 10",					/* 0x6A */
	"external event 11",					/* 0x6B */
	"external event 12",					/* 0x6C */
	"external event 13",					/* 0x6D */
	"external event 14",					/* 0x6E */
	"external event 15",					/* 0x6F */
	"AHB IDLE cycles (2)",					/* 0x70 */
	"AHB BUSY cycles (2)",					/* 0x71 */
	"AHB Non-Seq. transfers (2)",				/* 0x72 */
	"AHB Seq. transfers (2)",				/* 0x73 */
	"AHB read accesses (2)",				/* 0x74 */
	"AHB write accesses (2)",				/* 0x75 */
	"AHB byte accesses (2)",				/* 0x76 */
	"AHB half-word accesses (2)",				/* 0x77 */
	"AHB word accesses (2)",				/* 0x78 */
	"AHB double word accesses (2)",				/* 0x79 */
	"AHB quad word accesses (2)",				/* 0x7A */
	"AHB eight word accesses (2)",				/* 0x7B */
	"AHB waitstates (2)",					/* 0x7C */
	"AHB RETRY responses (2)",				/* 0x7D */
	"AHB SPLIT responses (2)",				/* 0x7E */
	"AHB SPLIT delay (2)",					/* 0x7F */
	"PMC: master 0 has grant",				/* 0x80 */
	"PMC: master 1 has grant",				/* 0x81 */
	"PMC: master 2 has grant",				/* 0x82 */
	"PMC: master 3 has grant",				/* 0x83 */
	"PMC: master 4 has grant",				/* 0x84 */
	"PMC: master 5 has grant",				/* 0x85 */
	"PMC: master 6 has grant",				/* 0x86 */
	"PMC: master 7 has grant",				/* 0x87 */
	"PMC: master 8 has grant",				/* 0x88 */
	"PMC: master 9 has grant",				/* 0x89 */
	"PMC: master 10 has grant",				/* 0x8A */
	"PMC: master 11 has grant",				/* 0x8B */
	"PMC: master 12 has grant",				/* 0x8C */
	"PMC: master 13 has grant",				/* 0x8D */
	"PMC: master 14 has grant",				/* 0x8E */
	"PMC: master 15 has grant",				/* 0x8F */
	"PMC: master 0 lacks grant",				/* 0x90 */
	"PMC: master 1 lacks grant",				/* 0x91 */
	"PMC: master 2 lacks grant",				/* 0x92 */
	"PMC: master 3 lacks grant",				/* 0x93 */
	"PMC: master 4 lacks grant",				/* 0x94 */
	"PMC: master 5 lacks grant",				/* 0x95 */
	"PMC: master 6 lacks grant",				/* 0x96 */
	"PMC: master 7 lacks grant",				/* 0x97 */
	"PMC: master 8 lacks grant",				/* 0x98 */
	"PMC: master 9 lacks grant",				/* 0x99 */
	"PMC: master 10 lacks grant",				/* 0x9A */
	"PMC: master 11 lacks grant",				/* 0x9B */
	"PMC: master 12 lacks grant",				/* 0x9C */
	"PMC: master 13 lacks grant",				/* 0x9D */
	"PMC: master 14 lacks grant",				/* 0x9E */
	"PMC: master 15 lacks grant",				/* 0x9F */
	""
};
#endif /* DEBUG */

/* Driver prototypes */

STATIC int l4stat_init(struct l4stat_priv *priv);

int l4stat_init1(struct drvmgr_dev *dev);

struct drvmgr_drv_ops l4stat_ops = 
{
	.init = {l4stat_init1, NULL, NULL, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id l4stat_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_L4STAT},
	{VENDOR_GAISLER, GAISLER_L3STAT},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info l4stat_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_L4STAT_ID,/* Driver ID */
		"L4STAT_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&l4stat_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct l4stat_priv),	/* Let DRVMGR allocate for us */
	},
	&l4stat_ids[0],
};

void l4stat_register_drv (void)
{
	DBG("Registering L4STAT driver\n");
	drvmgr_drv_register(&l4stat_drv_info.general);
}

STATIC int l4stat_init(struct l4stat_priv *priv)
{
	struct ambapp_apb_info *apb;
	struct amba_dev_info *ainfo = priv->dev->businfo;
	unsigned int tmp;
	unsigned short dev_id;

	/* Find L4STAT core from Plug&Play information */
	apb = ainfo->info.apb_slv;

	/* Check if L4STAT or L3STAT core from Plug&Play information */
	dev_id = ainfo->id.device;

	/* Check if rev 1 of core (only rev 0 supported) */
	if (apb->ver != 0) {
		DBG("L4STAT rev 0 only supported.\n");
		return L4STAT_ERR_ERROR;
	}

	/* Found L4STAT core, init private structure */
	priv->regs = (struct l4stat_regs *)apb->start;

	DBG("L4STAT regs 0x%08x\n", (unsigned int) priv->regs);

	/* Find L4STAT capabilities */
	tmp = REG_READ(&priv->regs->cctrl[0]);
	/* The CPU field in the register is just information of the
	 * cpus that are connected to the stat unit, but it is not
	 * really used for anything else. I can still have more masters
	 * on the bus (e.g. IOMMU) that I can collect stats from,
	 * so it makes no sense to limit the cpus to the actual cpus.
	 * Therefore, I will take the maximum number as 16. */
	/*priv->ncpu = ((tmp & CCTRL_NCPU) >> CCTRL_NCPU_BIT) + 1;*/
	priv->ncpu = 16;
	if (dev_id == GAISLER_L3STAT) {
		priv->ncnt = ((tmp & CCTRL_NCNT_L3STAT) >> CCTRL_NCNT_BIT) + 1;
	}else{
		priv->ncnt = ((tmp & CCTRL_NCNT) >> CCTRL_NCNT_BIT) + 1;
	}
	priv->max_count_support = (tmp & CCTRL_MC) >> CCTRL_MC_BIT;
	priv->internalahb_event_support = (tmp & CCTRL_IA) >> CCTRL_IA_BIT;
	priv->dsu_event_support = (tmp & CCTRL_DS) >> CCTRL_DS_BIT;
	priv->external_event_support = (tmp & CCTRL_EE) >> CCTRL_EE_BIT;
	priv->ahbtrace_event_support = (tmp & CCTRL_AE) >> CCTRL_AE_BIT;

	/* DEBUG print */
	DBG("L4STAT with following capabilities:\n");
	DBG(" -NCPU: %d, NCNT: %d, MaxCNT: %s\n", priv->ncpu, priv->ncnt, 
			(priv->max_count_support?"Available":"N/A"));
	DBG(" -Events= InternalAHB: %s, DSU: %s, External: %s, AHBTRACE: %s\n",
			(priv->internalahb_event_support?"Available":"N/A"),
			(priv->dsu_event_support?"Available":"N/A"),
			(priv->external_event_support?"Available":"N/A"),
			(priv->ahbtrace_event_support?"Available":"N/A"));

	return L4STAT_ERR_OK;
}

int l4stat_init1(struct drvmgr_dev *dev)
{
	struct l4stat_priv *priv = dev->priv;

	DBG("L4STAT[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	if (l4statpriv) {
		DBG("Driver only supports one L4STAT core\n");
		return DRVMGR_FAIL;
	}

	if (priv == NULL) {
		return DRVMGR_NOMEM;
	}
	priv->dev = dev;
	l4statpriv = priv;

	/* Initilize driver struct */
	if (l4stat_init(priv) != L4STAT_ERR_OK) {
		return DRVMGR_FAIL;
	}

	/* Startup Action:
	 *	- None
	 */

	/* Device name */
	sprintf(priv->devname, "l4stat0");

	return DRVMGR_OK;
}

int l4stat_counter_enable(unsigned int counter, int event, int cpu, int options)
{
	struct l4stat_priv *priv = l4statpriv;
	unsigned int ctrl;

	if (priv == NULL) {
		DBG("L4STAT Device not initialized\n");
		return L4STAT_ERR_EINVAL;
	}

	if (counter >= priv->ncnt) {
		DBG("L4STAT Wrong counter\n");
		return L4STAT_ERR_EINVAL;
	}

	if ((cpu < 0) || (cpu >= priv->ncpu)) {
		DBG("L4STAT Wrong cpu\n");
		return L4STAT_ERR_EINVAL;
	}

	if ((options & L4STAT_OPTIONS_MAXIMUM_DURATION) || 
			(options & L4STAT_OPTIONS_EVENT_LEVEL_ENABLE)) {
		if (priv->max_count_support == 0) {
			DBG("L4STAT maximum duration count not supported\n");
			return L4STAT_ERR_IMPLEMENTED;
		}
	}

	/* Check event is supported */
	if ((event < 0) || (event >= 0x80)) {
		DBG("L4STAT Wrong event\n");
		return L4STAT_ERR_EINVAL;
	}
	if ((event == 0x18) || (event == 0x17)) {
		if (priv->internalahb_event_support == 0) {
			DBG("L4STAT internal ahb event not supported\n");
			return L4STAT_ERR_IMPLEMENTED;
		}
	}
	if ((event >= 0x40) && (event < 0x60)) {
		if (priv->dsu_event_support == 0) {
			DBG("L4STAT dsu event not supported\n");
			return L4STAT_ERR_IMPLEMENTED;
		}
	}
	if ((event >= 0x60) && (event < 0x70)) {
		if (priv->external_event_support == 0) {
			DBG("L4STAT external event not supported\n");
			return L4STAT_ERR_IMPLEMENTED;
		}
	}
	if ((event >= 0x70) && (event < 0x80)) {
		if (priv->ahbtrace_event_support == 0) {
			DBG("L4STAT ahbtrace event not supported\n");
			return L4STAT_ERR_IMPLEMENTED;
		}
	}

	/* Prepare counter control */
	ctrl = (options & ~(CCTRL_EVENTID | CCTRL_CPUAHBM));
	/* Put event id */
	ctrl = (ctrl | ((event << CCTRL_EVENTID_BIT) & CCTRL_EVENTID));
	/* Put cpu id */
	ctrl = (ctrl | ((cpu << CCTRL_CPUAHBM_BIT) & CCTRL_CPUAHBM));
	/* Enable counter */
	ctrl = (ctrl | CCTRL_EN);

	REG_WRITE(&priv->regs->cctrl[counter], ctrl);

	/* DEBUG print */
	DBG("L4STAT COUNTER[%d] enabled with event: %s, cpu: %d\n", counter, 
			l4stat_event_names[event],cpu);

	return L4STAT_ERR_OK;
}

int l4stat_counter_disable(unsigned int counter)
{
	struct l4stat_priv *priv = l4statpriv;

	if (priv == NULL) {
		DBG("L4STAT Device not initialized\n");
		return L4STAT_ERR_EINVAL;
	}

	if (counter >= priv->ncnt) {
		DBG("L4STAT Wrong counter\n");
		return L4STAT_ERR_EINVAL;
	}

	/* Disable counter */
	REG_WRITE(&priv->regs->cctrl[counter], 0);

	/* DEBUG print */
	DBG("L4STAT COUNTER[%d] disabled\n", counter);

	return L4STAT_ERR_OK;
}

int l4stat_counter_get(unsigned int counter, uint32_t * val)
{
	struct l4stat_priv *priv = l4statpriv;

	if (priv == NULL) {
		DBG("L4STAT Device not initialized\n");
		return L4STAT_ERR_EINVAL;
	}

	if (counter >= priv->ncnt) {
		DBG("L4STAT Wrong counter\n");
		return L4STAT_ERR_EINVAL;
	}

	if (val == NULL) {
		DBG("L4STAT Wrong pointer\n");
		return L4STAT_ERR_EINVAL;
	}

	*val = REG_READ(&priv->regs->cval[counter]);

	return L4STAT_ERR_OK;
}

int l4stat_counter_set(unsigned int counter, uint32_t val)
{
	struct l4stat_priv *priv = l4statpriv;

	if (priv == NULL) {
		DBG("L4STAT Device not initialized\n");
		return L4STAT_ERR_EINVAL;
	}

	if (counter >= priv->ncnt) {
		DBG("L4STAT Wrong counter\n");
		return L4STAT_ERR_EINVAL;
	}

	REG_WRITE(&priv->regs->cval[counter],val);

	return L4STAT_ERR_OK;
}

int l4stat_counter_max_get(unsigned int counter, uint32_t * val)
{
	struct l4stat_priv *priv = l4statpriv;

	if (priv == NULL) {
		DBG("L4STAT Device not initialized\n");
		return L4STAT_ERR_EINVAL;
	}

	if (counter >= priv->ncnt) {
		DBG("L4STAT Wrong counter\n");
		return L4STAT_ERR_EINVAL;
	}

	if (val == NULL) {
		DBG("L4STAT Wrong pointer\n");
		return L4STAT_ERR_EINVAL;
	}

	*val = REG_READ(&priv->regs->cmax[counter]);

	return L4STAT_ERR_OK;
}

int l4stat_counter_max_set(unsigned int counter, uint32_t val)
{
	struct l4stat_priv *priv = l4statpriv;

	if (priv == NULL) {
		DBG("L4STAT Device not initialized\n");
		return L4STAT_ERR_EINVAL;
	}

	if (counter >= priv->ncnt) {
		DBG("L4STAT Wrong counter\n");
		return L4STAT_ERR_EINVAL;
	}

	REG_WRITE(&priv->regs->cmax[counter],val);

	return L4STAT_ERR_OK;
}

int l4stat_tstamp_get(uint32_t * val)
{
	struct l4stat_priv *priv = l4statpriv;

	if (priv == NULL) {
		DBG("L4STAT Device not initialized\n");
		return L4STAT_ERR_EINVAL;
	}

	if (val == NULL) {
		DBG("L4STAT Wrong pointer\n");
		return L4STAT_ERR_EINVAL;
	}

	*val = REG_READ(&priv->regs->timestamp);

	return L4STAT_ERR_OK;
}

int l4stat_tstamp_set(uint32_t val)
{
	struct l4stat_priv *priv = l4statpriv;

	if (priv == NULL) {
		DBG("L4STAT Device not initialized\n");
		return L4STAT_ERR_EINVAL;
	}

	REG_WRITE(&priv->regs->timestamp,val);

	return L4STAT_ERR_OK;
}

int l4stat_counter_print(unsigned int counter)
{
#ifdef DEBUG
	struct l4stat_priv *priv = l4statpriv;
	unsigned int val;
	unsigned int ctrl;
	unsigned int event;

	if (priv == NULL) {
		DBG("L4STAT Device not initialized\n");
		return L4STAT_ERR_EINVAL;
	}

	if (counter >= priv->ncnt) {
		DBG("L4STAT Wrong counter\n");
		return L4STAT_ERR_EINVAL;
	}

	/* Get counter val*/
	val = REG_READ(&priv->regs->cval[counter]);

	/* Get counter info*/
	ctrl = REG_READ(&priv->regs->cctrl[counter]);
	if ((ctrl & CCTRL_EN) == 0) {
		DBG("L4STAT COUNTER[%d] disabled\n", counter);
		return L4STAT_ERR_OK;
	}

	event = (ctrl & CCTRL_EVENTID) >> CCTRL_EVENTID_BIT;

	/* DEBUG print */
	DBG("L4STAT COUNTER[%d], Event: %s, Count: %d [0x%08x]\n", 
		counter, l4stat_event_names[event],val,val);
#endif /* DEBUG */

	return L4STAT_ERR_OK;
}

