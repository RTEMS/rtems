/*  SPWTDP - SpaceWire Time Distribution Protocol. The driver provides
 *  device discovery and interrupt management.
 *
 *  COPYRIGHT (c) 2017.
 *  Cobham Gaisler AB
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <rtems.h>
#include <rtems/bspIo.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp.h>
#include <grlib/ambapp_bus.h>
#include <bsp.h>
#include <grlib/spwtdp.h>

#include <grlib/grlib_impl.h>

/*#define STATIC*/
#define STATIC static

/*#define INLINE*/
#define INLINE inline

/*#define UNUSED*/
#define UNUSED __attribute__((unused))

#define DEBUG 1

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...) 
#endif

/* Memory and HW Registers Access routines. All 32-bit access routines */
#define REG_WRITE(addr, val) \
	(*(volatile unsigned int *)(addr) = (unsigned int)(val))
#define REG_READ(addr) (*(volatile unsigned int *)(addr))

/*
 * Configuration register definitions
 * DEFINED in header
 */

/*
 * Control register definitions
 * DEFINED in header
 */

/*
 * TSTX Control register definitions
 */
#define TSTXCTRL_TSTC (0xff<<TSTXCTRL_TSTC_BIT)
#define ETCTRL_PF (0xffff<<ETCTRL_PF_BIT)

#define TSTXCTRL_TSTC_BIT 24
#define ETCTRL_PF_BIT 0

#define DEVNAME_LEN 11
/* Private structure of SPWTDP driver. */
struct spwtdp_priv {
	char devname[DEVNAME_LEN];
	struct drvmgr_dev *dev; /* Device */
	struct spwtdp_regs *regs;
	int open;
	int index;
	int initiator; /* Initiator configured */
	int target; /* Target configured */
	int freq; /* Frequency configured */

	/* Spin-lock ISR protection */
	SPIN_DECLARE(devlock);

	/* Driver semaphore */
	rtems_id sem;
	spwtdp_isr_t isr;
	void * isr_arg;
};
int spwtdp_count = 0;
static struct spwtdp_priv *priv_tab[SPWTDP_MAX];


STATIC void spwtdp_isr(void *data);
STATIC int spwtdp_hw_reset(struct spwtdp_priv *priv);
STATIC int spwtdp_init2(struct drvmgr_dev *dev);

struct drvmgr_drv_ops spwtdp_ops =
{
	{NULL, spwtdp_init2, NULL, NULL},
	NULL,
	NULL
};

struct amba_dev_id spwtdp_ids[] =
{
	{VENDOR_GAISLER, GAISLER_SPWTDP},
	{0, 0}	/* Mark end of table */
};

struct amba_drv_info spwtdp_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_SPWTDP_ID,/* Driver ID */
		"SPWTDP_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&spwtdp_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct spwtdp_priv),	/* Let DrvMgr allocate priv */
	},
	&spwtdp_ids[0]
};

/* Register the SPWTDP Driver */
void spwtdp_register_drv(void)
{
	DBG("Registering SPWTDP driver\n");
	drvmgr_drv_register(&spwtdp_drv_info.general);
}

STATIC int spwtdp_init(struct spwtdp_priv *priv)
{
	struct amba_dev_info *ainfo = priv->dev->businfo;
	struct ambapp_apb_info *apb;

	/* Get device information from AMBA PnP information */
	if (ainfo == NULL) {
		return -1;
	}
	apb = ainfo->info.apb_slv;
	priv->regs = (struct spwtdp_regs *)apb->start;

	spwtdp_hw_reset(priv);
	/* Only support 56 bits counter */
	if (REG_READ(&priv->regs->dat_ctrl) != 0x2f00) {
		DBG("SPWTDP only supports 56 bit precission counters.\n");
		return -1;
	}
	DBG("SPWTDP driver initialized\n");

	return 0;
}

/*** INTERFACE TO DRIVER MANAGER ***/
STATIC int spwtdp_init2(struct drvmgr_dev *dev)
{
	int status;
	struct spwtdp_priv *priv;

	DBG("SPWTDP[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	if (spwtdp_count >= SPWTDP_MAX)
		return DRVMGR_ENORES;

	priv = dev->priv;
	if (priv == NULL)
		return DRVMGR_NOMEM;

	/* Register device */
	priv->dev = dev;
	priv->index = spwtdp_count;
	priv_tab[priv->index] = priv;
	snprintf(priv->devname, DEVNAME_LEN, "spwtdp%01u", priv->index);
	spwtdp_count++;

	/* Initialize Semaphore */
	if (rtems_semaphore_create(
		rtems_build_name('S', 'T', 'P', '0' + priv->index), 1,
		RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | \
		RTEMS_NO_INHERIT_PRIORITY | RTEMS_LOCAL | \
		RTEMS_NO_PRIORITY_CEILING, 0, &priv->sem) != RTEMS_SUCCESSFUL) {
		priv->sem = RTEMS_ID_NONE;
		return DRVMGR_FAIL;
	}

	/* Initialize SPWTDP Hardware */
	status = spwtdp_init(priv);
	if (status) {
		printk("Failed to initialize spwtdp driver %d\n", status);
		return -1;
	}

	return DRVMGR_OK;
}

/* Hardware Reset of SPWTDP */
STATIC int spwtdp_hw_reset(struct spwtdp_priv *priv)
{
	int i = 1000;
	SPIN_IRQFLAGS(irqflags);

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	/* Clear interrupts */
	REG_WRITE(&priv->regs->ists, SPWTDP_IRQ_WCLEAR);

	/* Reset the SPWTDP core */
	REG_WRITE(&priv->regs->conf[0], CONF0_RS);

	/* Wait for reset */
	while ((REG_READ(&priv->regs->conf[0]) & CONF0_RS) && (i > 0)) {
		i--;
	}

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return ((i > 0)? SPWTDP_ERR_OK : SPWTDP_ERR_ERROR);
}

int spwtdp_reset(void *spwtdp)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	/* Check priv and unregister isr */
	int ret = spwtdp_isr_unregister(spwtdp);
	if (ret != SPWTDP_ERR_OK)
		return ret;

	priv->initiator=0;
	priv->target=0;
	priv->freq=0;

	return spwtdp_hw_reset(priv);
}

void *spwtdp_open(int dev_no)
{
	struct spwtdp_priv *priv;

	if (dev_no >= spwtdp_count)
		return NULL;

	/* Get Device */
	priv = priv_tab[dev_no];
	if ((priv == NULL)||(priv->open == 1)) {
		return NULL;
	}

	/* Set initial state of software */
	priv->open = 1;

	return priv;
}

int spwtdp_close(void *spwtdp)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	/* Check priv and reset core */
	int ret = spwtdp_reset(spwtdp);
	if (ret != SPWTDP_ERR_OK)
		return ret;

	priv->open = 0;
	return SPWTDP_ERR_OK;
}

int spwtdp_freq_setup(void *spwtdp, uint32_t fsinc, uint32_t cv, uint8_t etinc)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	/* Check priv */
	if (priv == NULL)
		return SPWTDP_ERR_NOINIT;

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Take SPWTDP lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL)
		return SPWTDP_ERR_ERROR;

	REG_WRITE(&priv->regs->conf[1], fsinc & CONF1_FSINC);
	REG_WRITE(&priv->regs->conf[2],
		((cv<<CONF2_CV_BIT) & CONF2_CV) |
		((uint32_t)etinc & CONF2_ETINC));

	rtems_semaphore_release(priv->sem);
	priv->freq = 1;

	return SPWTDP_ERR_OK;
}

#define CONF0_INI_MASK (CONF0_EP|CONF0_ET|CONF0_SP|CONF0_SE|CONF0_LE| \
		CONF0_TD)
int spwtdp_initiator_conf(void *spwtdp, uint8_t mapping, uint32_t options)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	/* Check priv */
	if (priv == NULL)
		return SPWTDP_ERR_NOINIT;

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check if configured as target */
	if (priv->target == 1)
		return SPWTDP_ERR_EINVAL;

	/* Take SPWTDP lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL)
		return SPWTDP_ERR_ERROR;

	unsigned int conf0 = REG_READ(&priv->regs->conf[0]);
	conf0 &= ~(CONF0_INI_MASK|CONF0_MAP);
	REG_WRITE(&priv->regs->conf[0],
			conf0 | (options & CONF0_INI_MASK) |
			(((uint32_t)mapping << CONF0_MAP_BIT) & CONF0_MAP));

	priv->initiator = 1;

	rtems_semaphore_release(priv->sem);

	return SPWTDP_ERR_OK;
}

#define CONF0_TAR_MASK (CONF0_JE|CONF0_ST|CONF0_EP|CONF0_ET|CONF0_SP| \
		CONF0_SE|CONF0_LE|CONF0_TD|CONF0_ME)
int spwtdp_target_conf(void *spwtdp, uint8_t mapping, uint32_t options)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	/* Check priv */
	if (priv == NULL)
		return SPWTDP_ERR_NOINIT;

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check if configured as initiator */
	if (priv->initiator == 1)
		return SPWTDP_ERR_EINVAL;

	/* Take SPWTDP lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL)
		return SPWTDP_ERR_ERROR;

	unsigned int conf0 = REG_READ(&priv->regs->conf[0]);
	conf0 &= ~(CONF0_TAR_MASK|CONF0_MAP);
	REG_WRITE(&priv->regs->conf[0],
			conf0 | (options & CONF0_TAR_MASK) |
			(((uint32_t)mapping << CONF0_MAP_BIT) & CONF0_MAP));

	priv->initiator = 1;

	rtems_semaphore_release(priv->sem);

	return SPWTDP_ERR_OK;
}

int spwtdp_initiator_int_conf(void *spwtdp, uint8_t stm, uint8_t inrx,
			      uint8_t intx)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	/* Check priv */
	if (priv == NULL)
		return SPWTDP_ERR_NOINIT;

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check if configured as initiator */
	if (priv->initiator != 1)
		return SPWTDP_ERR_EINVAL;

	REG_WRITE(&priv->regs->conf[3],
			(((uint32_t)stm << CONF3_STM_BIT) & CONF3_STM) |
			(((uint32_t)inrx << CONF3_INRX_BIT) & CONF3_INRX) |
			(((uint32_t)intx << CONF3_INTX_BIT) & CONF3_INTX));

	return SPWTDP_ERR_OK;
}

int spwtdp_target_int_conf(void *spwtdp, uint8_t inrx, uint8_t intx,
			   uint32_t options)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	/* Check priv */
	if (priv == NULL) {
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check if configured as target */
	if (priv->target != 1)
		return SPWTDP_ERR_EINVAL;

	REG_WRITE(&priv->regs->conf[3],
			(options & CONF3_DI) |
			(((uint32_t)inrx << CONF3_INRX_BIT) & CONF3_INRX) |
			(((uint32_t)intx << CONF3_INTX_BIT) & CONF3_INTX));

	return SPWTDP_ERR_OK;
}

int spwtdp_initiator_enable(void *spwtdp)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	/* Check priv */
	if (priv == NULL) {
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check if configured as initiator */
	if (priv->initiator != 1)
		return SPWTDP_ERR_EINVAL;

	/* Check if frequency is configured */
	if (priv->freq != 1)
		return SPWTDP_ERR_EINVAL;

	/* Take SPWTDP lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL)
		return SPWTDP_ERR_ERROR;

	unsigned int conf0 = REG_READ(&priv->regs->conf[0]);
	REG_WRITE(&priv->regs->conf[0], conf0 | CONF0_TE);

	rtems_semaphore_release(priv->sem);

	return SPWTDP_ERR_OK;
}

int spwtdp_target_enable(void *spwtdp)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	/* Check priv */
	if (priv == NULL)
		return SPWTDP_ERR_NOINIT;

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check if configured as target */
	if (priv->target != 1)
		return SPWTDP_ERR_EINVAL;

	/* Check if frequency is configured */
	if (priv->freq != 1)
		return SPWTDP_ERR_EINVAL;

	/* Take SPWTDP lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL)
		return SPWTDP_ERR_ERROR;

	unsigned int conf0 = REG_READ(&priv->regs->conf[0]);
	REG_WRITE(&priv->regs->conf[0], conf0 | CONF0_RE);

	rtems_semaphore_release(priv->sem);

	return SPWTDP_ERR_OK;
}

int spwtdp_initiator_disable(void *spwtdp)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	/* Check priv */
	if (priv == NULL)
		return SPWTDP_ERR_NOINIT;

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Take SPWTDP lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL)
		return SPWTDP_ERR_ERROR;

	unsigned int conf0 = REG_READ(&priv->regs->conf[0]);
	REG_WRITE(&priv->regs->conf[0], conf0 & ~(CONF0_TE));

	rtems_semaphore_release(priv->sem);

	return SPWTDP_ERR_OK;
}

int spwtdp_target_disable(void *spwtdp)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	/* Check priv */
	if (priv == NULL)
		return SPWTDP_ERR_NOINIT;

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Take SPWTDP lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL)
		return SPWTDP_ERR_ERROR;

	unsigned int conf0 = REG_READ(&priv->regs->conf[0]);
	REG_WRITE(&priv->regs->conf[0], conf0 & ~(CONF0_RE));

	rtems_semaphore_release(priv->sem);

	return SPWTDP_ERR_OK;
}

/* Get and Clear status */
int spwtdp_status(void *spwtdp, uint32_t *sts, uint32_t clrmask)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	unsigned int status = REG_READ(&priv->regs->stat[0]);
	REG_WRITE(&priv->regs->stat[0], status & clrmask);

	if (sts != NULL)
		*sts = status;

	return SPWTDP_ERR_OK;
}

/* Get and Clear interrupts */
int spwtdp_interrupt_status(void *spwtdp, uint32_t *sts, uint32_t clrmask)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;
	SPIN_IRQFLAGS(irqflags);

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	unsigned int status = REG_READ(&priv->regs->ists);
	REG_WRITE(&priv->regs->ists, status & clrmask);
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	if (sts != NULL)
		*sts = status;

	return SPWTDP_ERR_OK;
}

/* Unmask interrupts */
int spwtdp_interrupt_unmask(void *spwtdp, uint32_t irqmask)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	unsigned int ctrl = REG_READ(&priv->regs->ien);
	REG_WRITE(&priv->regs->ien, ctrl | irqmask);

	return SPWTDP_ERR_OK;
}

/* Mask interrupts */
int spwtdp_interrupt_mask(void *spwtdp, uint32_t irqmask)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	unsigned int ctrl = REG_READ(&priv->regs->ien);
	REG_WRITE(&priv->regs->ien, ctrl & ~(irqmask));

	return SPWTDP_ERR_OK;
}

int spwtdp_isr_register(void *spwtdp, spwtdp_isr_t func, void *data)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;
	SPIN_ISR_IRQFLAGS(irqflags);

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check isr */
	if (func == NULL) {
		/* No ISR */
		return SPWTDP_ERR_EINVAL;
	}

	priv->isr = func;
	priv->isr_arg = data;

	/* Register and Enable Interrupt at Interrupt controller */
	drvmgr_interrupt_register(priv->dev, 0, "spwtdp", spwtdp_isr, priv);

	/* Enable AMBA Interrupts */
	SPIN_LOCK(&priv->devlock, irqflags);
	unsigned int cfg0 = REG_READ(&priv->regs->conf[0]);
	REG_WRITE(&priv->regs->conf[0], cfg0 | CONF0_AE);
	SPIN_UNLOCK(&priv->devlock, irqflags);

	return SPWTDP_ERR_OK;
}

int spwtdp_isr_unregister(void *spwtdp)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;
	SPIN_ISR_IRQFLAGS(irqflags);

	/* Disable IRQS (and check for priv!=NULL) */
	int ret=spwtdp_interrupt_mask(spwtdp, SPWTDP_IRQ_WCLEAR);
	if (ret != SPWTDP_ERR_OK)
		return ret;

	/* Disable AMBA Interrupts */
	SPIN_LOCK(&priv->devlock, irqflags);
	unsigned int cfg0 = REG_READ(&priv->regs->conf[0]);
	REG_WRITE(&priv->regs->conf[0], cfg0 & ~(CONF0_AE));
	SPIN_UNLOCK(&priv->devlock, irqflags);

	/* Disable Interrupt at Interrupt controller */
	drvmgr_interrupt_unregister(priv->dev, 0, spwtdp_isr, priv);

	/* Unregister isr */
	priv->isr = NULL;
	priv->isr_arg = NULL;

	return SPWTDP_ERR_OK;
}

STATIC void spwtdp_isr(void *arg)
{
	struct spwtdp_priv *priv = arg;
	unsigned int ists = REG_READ(&priv->regs->ists);
	SPIN_ISR_IRQFLAGS(irqflags);

	/* Return if the SPWTDP didn't generate the IRQ */
	if (ists == 0)
		return;

	SPIN_LOCK(&priv->devlock, irqflags);
	REG_WRITE(&priv->regs->ists, ists); /* clear handled interrupt events */
	SPIN_UNLOCK(&priv->devlock, irqflags);

	/* Let user Handle Interrupt */
	if (priv->isr!=NULL)
		priv->isr(ists, priv->isr_arg);

	return;
}

int spwtdp_dat_et_get(void * spwtdp, spwtdp_time_t * val)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check pointer */
	if (val == NULL) {
		return SPWTDP_ERR_EINVAL;
	}

	val->preamble = REG_READ(&priv->regs->dat_ctrl) & ETCTRL_PF;
	unsigned int * buffer = (unsigned int *) val->data;
	buffer[0] = REG_READ(&priv->regs->dat_et[0]);
	buffer[1] = REG_READ(&priv->regs->dat_et[1]);
	buffer[2] = REG_READ(&priv->regs->dat_et[2]);
	buffer[3] = REG_READ(&priv->regs->dat_et[3]);
	buffer[4] = REG_READ(&priv->regs->dat_et[4]);

	return SPWTDP_ERR_OK;
}

int spwtdp_tsrx_et_get(void * spwtdp, spwtdp_time_t * val)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check pointer */
	if (val == NULL) {
		return SPWTDP_ERR_EINVAL;
	}

	val->preamble = REG_READ(&priv->regs->ts_rx_ctrl) & ETCTRL_PF;
	unsigned int * buffer = (unsigned int *) val->data;
	buffer[0] = REG_READ(&priv->regs->ts_rx_et[0]);
	buffer[1] = REG_READ(&priv->regs->ts_rx_et[1]);
	buffer[2] = REG_READ(&priv->regs->ts_rx_et[2]);
	buffer[3] = REG_READ(&priv->regs->ts_rx_et[3]);
	buffer[4] = REG_READ(&priv->regs->ts_rx_et[4]);

	return SPWTDP_ERR_OK;
}

int spwtdp_tstx_et_get(void * spwtdp, spwtdp_time_t * val)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check pointer */
	if (val == NULL) {
		return SPWTDP_ERR_EINVAL;
	}

	val->preamble = REG_READ(&priv->regs->ts_tx_ctrl) & ETCTRL_PF;
	unsigned int * buffer = (unsigned int *) val->data;
	buffer[0] = REG_READ(&priv->regs->ts_tx_et[0]);
	buffer[1] = REG_READ(&priv->regs->ts_tx_et[1]);
	buffer[2] = REG_READ(&priv->regs->ts_tx_et[2]);
	buffer[3] = REG_READ(&priv->regs->ts_tx_et[3]);
	buffer[4] = REG_READ(&priv->regs->ts_tx_et[4]);

	return SPWTDP_ERR_OK;
}

int spwtdp_lat_et_get(void * spwtdp, spwtdp_time_t * val)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check pointer */
	if (val == NULL) {
		return SPWTDP_ERR_EINVAL;
	}

	val->preamble = REG_READ(&priv->regs->lat_ctrl) & ETCTRL_PF;
	unsigned int * buffer = (unsigned int *) val->data;
	buffer[0] = REG_READ(&priv->regs->lat_et[0]);
	buffer[1] = REG_READ(&priv->regs->lat_et[1]);
	buffer[2] = REG_READ(&priv->regs->lat_et[2]);
	buffer[3] = REG_READ(&priv->regs->lat_et[3]);
	buffer[4] = REG_READ(&priv->regs->lat_et[4]);

	return SPWTDP_ERR_OK;
}

int spwtdp_cmd_et_get(void * spwtdp, spwtdp_time_t * val)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check pointer */
	if (val == NULL) {
		return SPWTDP_ERR_EINVAL;
	}

	val->preamble = REG_READ(&priv->regs->cmd_ctrl) & ETCTRL_PF;
	unsigned int * buffer = (unsigned int *) val->data;
	buffer[0] = REG_READ(&priv->regs->cmd_et[0]);
	buffer[1] = REG_READ(&priv->regs->cmd_et[1]);
	buffer[2] = REG_READ(&priv->regs->cmd_et[2]);
	buffer[3] = REG_READ(&priv->regs->cmd_et[3]);
	buffer[4] = REG_READ(&priv->regs->cmd_et[4]);

	return SPWTDP_ERR_OK;
}

int spwtdp_initiator_tstx_conf(void * spwtdp, uint8_t tstc)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check if configured as initiator */
	if (priv->initiator != 1)
		return SPWTDP_ERR_EINVAL;

	REG_WRITE(&priv->regs->ts_tx_ctrl,
		  (((uint32_t)tstc) << TSTXCTRL_TSTC_BIT) & TSTXCTRL_TSTC);

	return SPWTDP_ERR_OK;
}

int spwtdp_initiator_cmd_et_set(void *spwtdp, spwtdp_time_t val)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check if configured as initiator */
	if (priv->initiator != 1)
		return SPWTDP_ERR_EINVAL;

	unsigned int * buffer = (unsigned int *) val.data;
	REG_WRITE(&priv->regs->lat_et[0], buffer[0]);
	REG_WRITE(&priv->regs->lat_et[1], buffer[1]);
	REG_WRITE(&priv->regs->lat_et[2], buffer[2]);
	REG_WRITE(&priv->regs->lat_et[3], buffer[3]);
	REG_WRITE(&priv->regs->lat_et[4], buffer[4]);


	/* Take SPWTDP lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL)
		return SPWTDP_ERR_ERROR;

	/* Signal new command */
	unsigned int ctrl = REG_READ(&priv->regs->cmd_ctrl);
	REG_WRITE(&priv->regs->cmd_ctrl, ctrl | CTRL_NC);

	rtems_semaphore_release(priv->sem);

	return SPWTDP_ERR_OK;
}

int spwtdp_initiator_cmd_spwtc_set(void *spwtdp, uint8_t spwtc)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check if configured as initiator */
	if (priv->initiator != 1)
		return SPWTDP_ERR_EINVAL;

	/* Take SPWTDP lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL)
		return SPWTDP_ERR_ERROR;

	unsigned int ctrl = (REG_READ(&priv->regs->cmd_ctrl) &~ CTRL_SPWTC);
	REG_WRITE(&priv->regs->cmd_ctrl,
		  ctrl | (((uint32_t)spwtc << CTRL_SPWTC_BIT) & CTRL_SPWTC));

	rtems_semaphore_release(priv->sem);

	return SPWTDP_ERR_OK;
}

#define CTRL_TAR_MASK (CTRL_NC|CTRL_IS)
int spwtdp_target_cmd_conf(void *spwtdp, uint8_t spwtc, uint16_t cpf,
			   uint32_t options)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	/* Check if configured as target */
	if (priv->target != 1)
		return SPWTDP_ERR_EINVAL;

	/* Take SPWTDP lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL)
		return SPWTDP_ERR_ERROR;

	REG_WRITE(&priv->regs->cmd_ctrl, 
			(options & CTRL_TAR_MASK) |
			((cpf << CTRL_CPF_BIT) & CTRL_CPF) |
			(((uint32_t)spwtc << CTRL_SPWTC_BIT) & CTRL_SPWTC));

	rtems_semaphore_release(priv->sem);

	return SPWTDP_ERR_OK;
}

int spwtdp_precision_get(void *spwtdp, uint8_t *fine, uint8_t *coarse)
{
	struct spwtdp_priv *priv = (struct spwtdp_priv *)spwtdp;
	int coarse_precision, fine_precision;

	if (priv == NULL) {
		/* SPWTDP not initialized */
		return SPWTDP_ERR_NOINIT;
	}

	if (priv->open == 0)
		return SPWTDP_ERR_EINVAL;

	unsigned int preamble = REG_READ(&priv->regs->dat_ctrl);

	if (preamble & 0x80) {
		DBG("Pfield second extension set: unknown format");
		return SPWTDP_ERR_ERROR;
	}
	if (!((preamble & 0x7000) == 0x2000 || (preamble & 0x7000) == 0x1000)) {
		DBG(" PField indicates not unsegmented code: unknown format");
		return SPWTDP_ERR_ERROR;
	}
	/*
	 * coarse_precision = 32;
	 * fine_precision = 24;
	 */
	coarse_precision = ((preamble >> 10) & 0x3) + 1;
	if (preamble & 0x80)
		coarse_precision += (preamble >> 5) & 0x3;
	fine_precision = (preamble >> 8) & 0x3;
	if (preamble & 0x80)
		fine_precision += (preamble >> 2) & 0x7;
	if (coarse!=NULL)
		*coarse = coarse_precision;
	if (fine!=NULL)
		*fine = fine_precision;

	return SPWTDP_ERR_OK;
}

