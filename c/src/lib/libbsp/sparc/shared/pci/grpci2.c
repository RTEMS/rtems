/*  GRLIB GRPCI2 PCI HOST driver.
 * 
 *  COPYRIGHT (c) 2011
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

/* Configures the GRPCI2 core and initialize,
 *  - the PCI Library (pci.c)
 *  - the general part of the PCI Bus driver (pci_bus.c)
 *  
 * System interrupt assigned to PCI interrupt (INTA#..INTD#) is by
 * default taken from Plug and Play, but may be overridden by the 
 * driver resources INTA#..INTD#. GRPCI2 handles differently depending
 * on the design (4 different ways).
 *
 * GRPCI2 IRQ implementation notes
 * -------------------------------
 * Since the Driver Manager pci_bus layer implements IRQ by calling
 * pci_interrupt_* which translates into BSP_shared_interrupt_*, and the
 * root-bus also relies on BSP_shared_interrupt_*, it is safe for the GRPCI2
 * driver to use the drvmgr_interrupt_* routines since they will be
 * accessing the same routines in the end. Otherwise the GRPCI2 driver must
 * have used the pci_interrupt_* routines.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rtems/bspIo.h>
#include <libcpu/byteorder.h>
#include <libcpu/access.h>
#include <pci.h>
#include <pci/cfg.h>

#include <drvmgr/drvmgr.h>
#include <drvmgr/ambapp_bus.h>
#include <ambapp.h>
#include <drvmgr/pci_bus.h>
#include <bsp/grpci2.h>

#ifndef IRQ_GLOBAL_PREPARE
 #define IRQ_GLOBAL_PREPARE(level) rtems_interrupt_level level
#endif

#ifndef IRQ_GLOBAL_DISABLE
 #define IRQ_GLOBAL_DISABLE(level) rtems_interrupt_disable(level)
#endif

#ifndef IRQ_GLOBAL_ENABLE
 #define IRQ_GLOBAL_ENABLE(level) rtems_interrupt_enable(level)
#endif

/* If defined to 1 - byte twisting is enabled by default */
#define DEFAULT_BT_ENABLED 0

/* If defined to 64 - Latency timer is 64 by default */
#define DEFAULT_LATENCY_TIMER 64

/* Interrupt assignment. Set to other value than 0xff in order to 
 * override defaults and plug&play information
 */
#ifndef GRPCI2_INTA_SYSIRQ
 #define GRPCI2_INTA_SYSIRQ 0xff
#endif
#ifndef GRPCI2_INTB_SYSIRQ
 #define GRPCI2_INTB_SYSIRQ 0xff
#endif
#ifndef GRPCI2_INTC_SYSIRQ
 #define GRPCI2_INTC_SYSIRQ 0xff
#endif
#ifndef GRPCI2_INTD_SYSIRQ
 #define GRPCI2_INTD_SYSIRQ 0xff
#endif

/*#define DEBUG 1*/

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...) 
#endif

/*
 * GRPCI2 APB Register MAP
 */
struct grpci2_regs {
	volatile unsigned int ctrl;		/* 0x00 */
	volatile unsigned int sts_cap;		/* 0x04 */
	volatile unsigned int ppref;		/* 0x08 */
	volatile unsigned int io_map;		/* 0x0C */
	volatile unsigned int dma_ctrl;		/* 0x10 */
	volatile unsigned int dma_bdbase;	/* 0x14 */
	volatile unsigned int dma_chact;	/* 0x18 */
	int res1;				/* 0x1C */
	volatile unsigned int bars[6];		/* 0x20 */
	int res2[2];				/* 0x38 */
	volatile unsigned int ahbmst_map[16];	/* 0x40 */
};

#define CTRL_BUS_BIT 16

#define CTRL_SI (1<<27)
#define CTRL_PE (1<<26)
#define CTRL_ER (1<<25)
#define CTRL_EI (1<<24)
#define CTRL_BUS (0xff<<CTRL_BUS_BIT)
#define CTRL_HOSTINT 0xf

#define STS_HOST_BIT	31
#define STS_MST_BIT	30
#define STS_TAR_BIT	29
#define STS_DMA_BIT	28
#define STS_DI_BIT	27
#define STS_HI_BIT	26
#define STS_IRQMODE_BIT	24
#define STS_TRACE_BIT	23
#define STS_CFGERRVALID_BIT 20
#define STS_CFGERR_BIT	19
#define STS_INTTYPE_BIT	12
#define STS_INTSTS_BIT	8
#define STS_FDEPTH_BIT	2
#define STS_FNUM_BIT	0

#define STS_HOST	(1<<STS_HOST_BIT)
#define STS_MST		(1<<STS_MST_BIT)
#define STS_TAR		(1<<STS_TAR_BIT)
#define STS_DMA		(1<<STS_DMA_BIT)
#define STS_DI		(1<<STS_DI_BIT)
#define STS_HI		(1<<STS_HI_BIT)
#define STS_IRQMODE	(0x3<<STS_IRQMODE_BIT)
#define STS_TRACE	(1<<STS_TRACE_BIT)
#define STS_CFGERRVALID	(1<<STS_CFGERRVALID_BIT)
#define STS_CFGERR	(1<<STS_CFGERR_BIT)
#define STS_INTTYPE	(0x7f<<STS_INTTYPE_BIT)
#define STS_INTSTS	(0xf<<STS_INTSTS_BIT)
#define STS_FDEPTH	(0x7<<STS_FDEPTH_BIT)
#define STS_FNUM	(0x3<<STS_FNUM_BIT)

#define STS_ITIMEOUT	(1<<18)
#define STS_ISYSERR	(1<<17)
#define STS_IDMA	(1<<16)
#define STS_IDMAERR	(1<<15)
#define STS_IMSTABRT	(1<<14)
#define STS_ITGTABRT	(1<<13)
#define STS_IPARERR	(1<<12)

struct grpci2_bd_chan {
	volatile unsigned int ctrl;	/* 0x00 DMA Control */
	volatile unsigned int nchan;	/* 0x04 Next DMA Channel Address */
	volatile unsigned int nbd;	/* 0x08 Next Data Descriptor in channel */
	volatile unsigned int res;	/* 0x0C Reserved */
};

#define BD_CHAN_EN		(1<<BD_CHAN_EN_BIT)
#define BD_CHAN_ID		(0x3<<BD_CHAN_ID_BIT)
#define BD_CHAN_TYPE		(0x3<<BD_CHAN_TYPE_BIT)
#define BD_CHAN_BDCNT		(0xffff<<BD_CHAN_BDCNT_BIT)
#define BD_CHAN_EN_BIT		31
#define BD_CHAN_ID_BIT		22
#define BD_CHAN_TYPE_BIT	20
#define BD_CHAN_BDCNT_BIT	0

struct grpci2_bd_data {
	volatile unsigned int ctrl;	/* 0x00 DMA Data Control */
	volatile unsigned int pci_adr;	/* 0x04 PCI Start Address */
	volatile unsigned int ahb_adr;	/* 0x08 AHB Start address */
	volatile unsigned int next;	/* 0x0C Next Data Descriptor in channel */
};

#define BD_DATA_EN		(0x1<<BD_DATA_EN_BIT)
#define BD_DATA_IE		(0x1<<BD_DATA_IE_BIT)
#define BD_DATA_DR		(0x1<<BD_DATA_DR_BIT)
#define BD_DATA_BE		(0x1<<BD_DATA_BE_BIT)
#define BD_DATA_TYPE		(0x3<<BD_DATA_TYPE_BIT)
#define BD_DATA_ER		(0x1<<BD_DATA_ER_BIT)
#define BD_DATA_LEN		(0xffff<<BD_DATA_LEN_BIT)
#define BD_DATA_EN_BIT		31
#define BD_DATA_IE_BIT		30
#define BD_DATA_DR_BIT		29
#define BD_DATA_BE_BIT		28
#define BD_DATA_TYPE_BIT	20
#define BD_DATA_ER_BIT		19
#define BD_DATA_LEN_BIT		0

/* GRPCI2 Capability */
struct grpci2_cap_first {
	unsigned int ctrl;
	unsigned int pci2ahb_map[6];
	unsigned int ext2ahb_map;
	unsigned int io_map;
	unsigned int pcibar_size[6];
	unsigned int ahb_pref;
};
#define CAP9_CTRL_OFS 0
#define CAP9_BAR_OFS 0x4
#define CAP9_IOMAP_OFS 0x20
#define CAP9_BARSIZE_OFS 0x24
#define CAP9_AHBPREF_OFS 0x3C

/* Used internally for accessing the PCI bridge's configuration space itself */
#define HOST_TGT PCI_DEV(0xff, 0, 0)

struct grpci2_priv *grpci2priv = NULL;

/* PCI Interrupt assignment. Connects an PCI interrupt pin (INTA#..INTD#)
 * to a system interrupt number.
 */
unsigned char grpci2_pci_irq_table[4] =
{
	/* INTA# */	GRPCI2_INTA_SYSIRQ,
	/* INTB# */	GRPCI2_INTB_SYSIRQ,
	/* INTC# */	GRPCI2_INTC_SYSIRQ,
	/* INTD# */	GRPCI2_INTD_SYSIRQ
};

/* Start of workspace/dynamical area */
extern unsigned int _end;
#define DMA_START ((unsigned int) &_end)

/* Default BAR mapping, set BAR0 256MB 1:1 mapped base of CPU RAM */
struct grpci2_pcibar_cfg grpci2_default_bar_mapping[6] = {
	/* BAR0 */ {DMA_START, DMA_START, 0x10000000},
	/* BAR1 */ {0, 0, 0},
	/* BAR2 */ {0, 0, 0},
	/* BAR3 */ {0, 0, 0},
	/* BAR4 */ {0, 0, 0},
	/* BAR5 */ {0, 0, 0},
};

/* Driver private data struture */
struct grpci2_priv {
	struct drvmgr_dev	*dev;
	struct grpci2_regs		*regs;
	unsigned char			ver;
	char				irq;
	char				irq_mode; /* IRQ Mode from CAPSTS REG */
	char				bt_enabled;
	unsigned int			irq_mask;
	unsigned int			latency_timer;

	struct grpci2_pcibar_cfg	*barcfg;

	unsigned int			pci_area;
	unsigned int			pci_area_end;
	unsigned int			pci_io;    
	unsigned int			pci_conf;
	unsigned int			pci_conf_end;

	uint32_t			devVend; /* Host PCI Device/Vendor ID */

	struct drvmgr_map_entry		maps_up[7];
	struct drvmgr_map_entry		maps_down[2];
	struct pcibus_config		config;
};

int grpci2_init1(struct drvmgr_dev *dev);
int grpci2_init3(struct drvmgr_dev *dev);
void grpci2_err_isr(void *arg);

/* GRPCI2 DRIVER */

struct drvmgr_drv_ops grpci2_ops = 
{
	.init = {grpci2_init1, NULL, grpci2_init3, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id grpci2_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_GRPCI2},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info grpci2_info =
{
	{
		DRVMGR_OBJ_DRV,                 /* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_GRPCI2_ID,/* Driver ID */
		"GRPCI2_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&grpci2_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct grpci2_priv),	/* Make drvmgr alloc private */
	},
	&grpci2_ids[0]
};

void grpci2_register_drv(void)
{
	DBG("Registering GRPCI2 driver\n");
	drvmgr_drv_register(&grpci2_info.general);
}

static int grpci2_cfg_r32(pci_dev_t dev, int ofs, uint32_t *val)
{
	struct grpci2_priv *priv = grpci2priv;
	volatile uint32_t *pci_conf;
	unsigned int tmp, devfn;
	IRQ_GLOBAL_PREPARE(oldLevel);
	int retval, bus = PCI_DEV_BUS(dev);

	if ((unsigned int)ofs & 0xffffff03) {
		retval = PCISTS_EINVAL;
		goto out2;
	}

	if (PCI_DEV_SLOT(dev) > 15) {
		retval = PCISTS_MSTABRT;
		goto out;
	}

	/* GRPCI2 can access "non-standard" devices on bus0 (on AD11.AD16), 
	 * we skip them.
	 */
	if (dev == HOST_TGT)
		bus = devfn = 0;
	else if (bus == 0)
		devfn = PCI_DEV_DEVFUNC(dev) + PCI_DEV(0, 6, 0);
	else
		devfn = PCI_DEV_DEVFUNC(dev);

	pci_conf = (volatile uint32_t *) (priv->pci_conf | (devfn << 8) | ofs);

	IRQ_GLOBAL_DISABLE(oldLevel); /* protect regs */

	/* Select bus */
	priv->regs->ctrl = (priv->regs->ctrl & ~(0xff<<16)) | (bus<<16);
	/* clear old status */
	priv->regs->sts_cap = (STS_CFGERR | STS_CFGERRVALID);

	tmp = *pci_conf;

	/* Wait until GRPCI2 signals that CFG access is done, it should be
	 * done instantaneously unless a DMA operation is ongoing...
	 */
	while ((priv->regs->sts_cap & STS_CFGERRVALID) == 0)
		;

	if (priv->regs->sts_cap & STS_CFGERR) {
		retval = PCISTS_MSTABRT;
	} else {
		/* Bus always little endian (unaffected by byte-swapping) */
		*val = CPU_swap_u32(tmp);
		retval = PCISTS_OK;
	}

	IRQ_GLOBAL_ENABLE(oldLevel);

out:
	if (retval != PCISTS_OK)
		*val = 0xffffffff;

	DBG("pci_read: [%x:%x:%x] reg: 0x%x => addr: 0x%x, val: 0x%x  (%d)\n",
		PCI_DEV_EXPAND(dev), ofs, pci_conf, *val, retval);

out2:
	return retval;
}

static int grpci2_cfg_r16(pci_dev_t dev, int ofs, uint16_t *val)
{
	uint32_t v;
	int retval;

	if (ofs & 1)
		return PCISTS_EINVAL;

	retval = grpci2_cfg_r32(dev, ofs & ~0x3, &v);
	*val = 0xffff & (v >> (8*(ofs & 0x3)));

	return retval;
}

static int grpci2_cfg_r8(pci_dev_t dev, int ofs, uint8_t *val)
{
	uint32_t v;
	int retval;

	retval = grpci2_cfg_r32(dev, ofs & ~0x3, &v);

	*val = 0xff & (v >> (8*(ofs & 3)));

	return retval;
}

static int grpci2_cfg_w32(pci_dev_t dev, int ofs, uint32_t val)
{
	struct grpci2_priv *priv = grpci2priv;
	volatile uint32_t *pci_conf;
	uint32_t value, devfn;
	int retval, bus = PCI_DEV_BUS(dev);
	IRQ_GLOBAL_PREPARE(oldLevel);

	if ((unsigned int)ofs & 0xffffff03)
		return PCISTS_EINVAL;

	if (PCI_DEV_SLOT(dev) > 15)
		return PCISTS_MSTABRT;

	value = CPU_swap_u32(val);

	/* GRPCI2 can access "non-standard" devices on bus0 (on AD11.AD16), 
	 * we skip them.
	 */
	if (dev == HOST_TGT)
		bus = devfn = 0;
	else if (bus == 0)
		devfn = PCI_DEV_DEVFUNC(dev) + PCI_DEV(0, 6, 0);
	else
		devfn = PCI_DEV_DEVFUNC(dev);

	pci_conf = (volatile uint32_t *) (priv->pci_conf | (devfn << 8) | ofs);

	IRQ_GLOBAL_DISABLE(oldLevel); /* protect regs */

	/* Select bus */
	priv->regs->ctrl = (priv->regs->ctrl & ~(0xff<<16)) | (bus<<16);
	/* clear old status */
	priv->regs->sts_cap = (STS_CFGERR | STS_CFGERRVALID);

	*pci_conf = value;

	/* Wait until GRPCI2 signals that CFG access is done, it should be
	 * done instantaneously unless a DMA operation is ongoing...
	 */
	while ((priv->regs->sts_cap & STS_CFGERRVALID) == 0)
		;

	if (priv->regs->sts_cap & STS_CFGERR)
		retval = PCISTS_MSTABRT;
	else
		retval = PCISTS_OK;

	IRQ_GLOBAL_ENABLE(oldLevel);

	DBG("pci_write - [%x:%x:%x] reg: 0x%x => addr: 0x%x, val: 0x%x  (%d)\n",
		PCI_DEV_EXPAND(dev), ofs, pci_conf, value, retval);

	return retval;
}

static int grpci2_cfg_w16(pci_dev_t dev, int ofs, uint16_t val)
{
	uint32_t v;
	int retval;

	if (ofs & 1)
		return PCISTS_EINVAL;

	retval = grpci2_cfg_r32(dev, ofs & ~0x3, &v);
	if (retval != PCISTS_OK)
		return retval;

	v = (v & ~(0xffff << (8*(ofs&3)))) | ((0xffff&val) << (8*(ofs&3)));

	return grpci2_cfg_w32(dev, ofs & ~0x3, v);
}

static int grpci2_cfg_w8(pci_dev_t dev, int ofs, uint8_t val)
{
	uint32_t v;
	int retval;

	retval = grpci2_cfg_r32(dev, ofs & ~0x3, &v);
	if (retval != PCISTS_OK)
		return retval;

	v = (v & ~(0xff << (8*(ofs&3)))) | ((0xff&val) << (8*(ofs&3)));

	return grpci2_cfg_w32(dev, ofs & ~0x3, v);
}

/* Return the assigned system IRQ number that corresponds to the PCI
 * "Interrupt Pin" information from configuration space.
 *
 * The IRQ information is stored in the grpci2_pci_irq_table configurable
 * by the user.
 *
 * Returns the "system IRQ" for the PCI INTA#..INTD# pin in irq_pin. Returns
 * 0xff if not assigned.
 */
static uint8_t grpci2_bus0_irq_map(pci_dev_t dev, int irq_pin)
{
	uint8_t sysIrqNr = 0; /* not assigned */
	int irq_group;

	if ( (irq_pin >= 1) && (irq_pin <= 4) ) {
		/* Use default IRQ decoding on PCI BUS0 according slot numbering */
		irq_group = PCI_DEV_SLOT(dev) & 0x3;
		irq_pin = ((irq_pin - 1) + irq_group) & 0x3;
		/* Valid PCI "Interrupt Pin" number */
		sysIrqNr = grpci2_pci_irq_table[irq_pin];
	}
	return sysIrqNr;
}

static int grpci2_translate(uint32_t *address, int type, int dir)
{
	uint32_t adr, start, end;
	struct grpci2_priv *priv = grpci2priv;
	int i;

	if (type == 1) {
		/* I/O */
		if (dir != 0) {
			/* The PCI bus can not access the CPU bus from I/O
			 * because GRPCI2 core does not support I/O BARs
			 */
			return -1;
		}

		/* We have got a PCI IO BAR address that the CPU want to access.
		 * Check that it is within the PCI I/O window, I/O adresses
		 * are NOT mapped 1:1 with GRPCI2 driver... translation needed.
		 */
		adr = *(uint32_t *)address;
		if (adr < 0x100 || adr > 0x10000)
			return -1;
		*address = adr + priv->pci_io;
	} else {
		/* MEMIO and MEM.
		 * Memory space is mapped 1:1 so no translation is needed.
		 * Check that address is within accessible windows.
		 */
		adr = *(uint32_t *)address;
		if (dir == 0) {
			/* PCI BAR to AMBA-CPU address.. check that it is
			 * located within GRPCI2 PCI Memory Window
			 * adr = PCI address.
			 */
			if (adr < priv->pci_area || adr >= priv->pci_area_end)
				return -1;
		} else {
			/* We have a CPU address and want to get access to it
			 * from PCI space, typically when doing DMA into CPU
			 * RAM. The GRPCI2 core may have multiple target BARs
			 * that PCI masters can access, the BARs are user
			 * configurable in the following ways:
			 *  BAR_SIZE, PCI_BAR Address and MAPPING (AMBA ADR)
			 *
			 * The below code tries to find a BAR for which the
			 * AMBA bar may have been mapped onto, and translate
			 * the AMBA-CPU address into a PCI address using the
			 * given mapping.
			 *
			 * adr = AMBA address.
			 */
			for(i=0; i<6; i++) {
				start = priv->barcfg[i].ahbadr;
				end = priv->barcfg[i].ahbadr +
					priv->barcfg[i].barsize;
				if (adr >= start && adr < end) {
					/* BAR match: Translate address */
					*address = (adr - start) +
						priv->barcfg[i].pciadr;
					return 0;
				}
			}
			return -1;
		}
	}

	return 0;
}

extern struct pci_memreg_ops pci_memreg_sparc_le_ops;
extern struct pci_memreg_ops pci_memreg_sparc_be_ops;

/* GRPCI2 PCI access routines, default to Little-endian PCI Bus */
struct pci_access_drv grpci2_access_drv = {
	.cfg =
	{
		grpci2_cfg_r8,
		grpci2_cfg_r16,
		grpci2_cfg_r32,
		grpci2_cfg_w8,
		grpci2_cfg_w16,
		grpci2_cfg_w32,
	},
	.io =
	{
		_ld8,
		_ld_le16,
		_ld_le32,
		_st8,
		_st_le16,
		_st_le32,
	},
	.memreg = &pci_memreg_sparc_le_ops,
	.translate = grpci2_translate,
};

struct pci_io_ops grpci2_io_ops_be =
{
	_ld8,
	_ld_be16,
	_ld_be32,
	_st8,
	_st_be16,
	_st_be32,
};

/* PCI Error Interrupt handler, called when there may be a PCI Target/Master
 * Abort.
 */
void grpci2_err_isr(void *arg)
{
	struct grpci2_priv *priv = arg;
	unsigned int sts = priv->regs->sts_cap;

	if (sts & (STS_IMSTABRT | STS_ITGTABRT | STS_IPARERR | STS_ISYSERR | STS_ITIMEOUT)) {
		/* A PCI error IRQ ... Error handler unimplemented
		 * add your code here...
		 */
		if (sts & STS_IMSTABRT) {
			printk("GRPCI2: unhandled Master Abort IRQ\n");
		}
		if (sts & STS_ITGTABRT) {
			printk("GRPCI2: unhandled Target Abort IRQ\n");
		}
		if (sts & STS_IPARERR) {
			printk("GRPCI2: unhandled Parity Error IRQ\n");
		}
		if (sts & STS_ISYSERR) {
			printk("GRPCI2: unhandled System Error IRQ\n");
		}
		if (sts & STS_ITIMEOUT) {
			printk("GRPCI2: unhandled PCI target access timeout IRQ\n");
		}
	}
}

static int grpci2_hw_init(struct grpci2_priv *priv)
{
	struct grpci2_regs *regs = priv->regs;
	int i;
	uint8_t capptr;
	uint32_t data, io_map, ahbadr, pciadr, size;
	pci_dev_t host = HOST_TGT;
	struct grpci2_pcibar_cfg *barcfg = priv->barcfg;

	/* Reset any earlier setup */
	regs->ctrl = 0;
	regs->sts_cap = ~0; /* Clear Status */
	regs->dma_ctrl = 0;
	regs->dma_bdbase = 0;

	/* Translate I/O accesses 1:1, (will not work for PCI 2.3) */
	regs->io_map = priv->pci_io & 0xffff0000;

	/* set 1:1 mapping between AHB -> PCI memory space, for all Masters
	 * Each AHB master has it's own mapping registers. Max 16 AHB masters.
	 */
	for (i=0; i<16; i++)
		regs->ahbmst_map[i] = priv->pci_area;

	/* Get the GRPCI2 Host PCI ID */
	grpci2_cfg_r32(host, PCIR_VENDOR, &priv->devVend);

	/* Get address to first (always defined) capability structure */
	grpci2_cfg_r8(host, PCIR_CAP_PTR, &capptr);
	if (capptr == 0)
		return -1;

	/* Limit the prefetch for GRPCI2 version 0. */
	if (priv->ver == 0)
		grpci2_cfg_w32(host, capptr+CAP9_AHBPREF_OFS, 0);

	/* Enable/Disable Byte twisting */
	grpci2_cfg_r32(host, capptr+CAP9_IOMAP_OFS, &io_map);
	io_map = (io_map & ~0x1) | (priv->bt_enabled ? 1 : 0);
	grpci2_cfg_w32(host, capptr+CAP9_IOMAP_OFS, io_map);

	/* Setup the Host's PCI Target BARs for others to access (DMA) */
	for (i=0; i<6; i++) {
		/* Make sure address is properly aligned */
		size = ~(barcfg[i].barsize-1);
		barcfg[i].pciadr &= size;
		barcfg[i].ahbadr &= size;

		pciadr = barcfg[i].pciadr;
		ahbadr = barcfg[i].ahbadr;
		size |= PCIM_BAR_MEM_PREFETCH;

		grpci2_cfg_w32(host, capptr+CAP9_BARSIZE_OFS+i*4, size);
		grpci2_cfg_w32(host, capptr+CAP9_BAR_OFS+i*4, ahbadr);
		grpci2_cfg_w32(host, PCIR_BAR(0)+i*4, pciadr);
	}

	/* set as bus master and enable pci memory responses */  
	grpci2_cfg_r32(host, PCIR_COMMAND, &data);
	data |= (PCIM_CMD_MEMEN | PCIM_CMD_BUSMASTEREN);
	grpci2_cfg_w32(host, PCIR_COMMAND, data);

	/* set latency timer */
	grpci2_cfg_r32(host, PCIR_CACHELNSZ, &data);
	data &= ~0xff00;
	data |= ((priv->latency_timer & 0xff) << 8);
	grpci2_cfg_w32(host, PCIR_CACHELNSZ, data);

	/* Enable Error respone (CPU-TRAP) on illegal memory access */
	regs->ctrl = CTRL_ER | CTRL_PE;

	/* Successful */
	return 0;
}

/* Initializes the GRPCI2 core and driver, must be called before calling
 * init_pci()
 *
 * Return values
 *  0             Successful initalization
 *  -1            Error during initialization, for example "PCI core not found".
 *  -2            Error PCI controller not HOST (targets not supported)
 *  -3            Error due to GRPCI2 hardware initialization
 */
static int grpci2_init(struct grpci2_priv *priv)
{
	struct ambapp_apb_info *apb;
	struct ambapp_ahb_info *ahb;
	int pin, i, j;
	union drvmgr_key_value *value;
	char keyname[6];
	struct amba_dev_info *ainfo = priv->dev->businfo;
	struct grpci2_pcibar_cfg *barcfg;
	unsigned int size;

	/* Find PCI core from Plug&Play information */
	apb = ainfo->info.apb_slv;
	ahb = ainfo->info.ahb_slv;

	/* Found PCI core, init private structure */
	priv->irq = apb->irq;
	priv->ver = apb->ver;
	priv->regs = (struct grpci2_regs *)apb->start;
	priv->bt_enabled = DEFAULT_BT_ENABLED;
	priv->irq_mode = (priv->regs->sts_cap & STS_IRQMODE) >> STS_IRQMODE_BIT;
	priv->latency_timer = DEFAULT_LATENCY_TIMER;

	/* Calculate the PCI windows 
	 *  AMBA->PCI Window:                       AHB SLAVE AREA0
	 *  AMBA->PCI I/O cycles Window:            AHB SLAVE AREA1 Lower half
	 *  AMBA->PCI Configuration cycles Window:  AHB SLAVE AREA1 Upper half
	 */
	priv->pci_area     = ahb->start[0];
	priv->pci_area_end = ahb->start[0] + ahb->mask[0];
	priv->pci_io       = ahb->start[1];
	priv->pci_conf     = ahb->start[1] + 0x10000;
	priv->pci_conf_end = priv->pci_conf + 0x10000;

	/* On systems where PCI I/O area and configuration area is apart of the
	 * "PCI Window" the PCI Window stops at the start of the PCI I/O area
	 */
	if ((priv->pci_io > priv->pci_area) &&
	    (priv->pci_io < (priv->pci_area_end-1))) {
		priv->pci_area_end = priv->pci_io;
	}

	/* Init PCI interrupt assignment table to all use the interrupt routed
	 * through the GRPCI2 core.
	 */
	strcpy(keyname, "INTX#");
	for (pin=1; pin<5; pin++) {
		if (grpci2_pci_irq_table[pin-1] == 0xff) {
			if (priv->irq_mode < 2) {
				/* PCI Interrupts are shared */
				grpci2_pci_irq_table[pin-1] = priv->irq;
			} else {
				/* Unique IRQ per PCI INT Pin */
				grpci2_pci_irq_table[pin-1] = priv->irq + pin-1;
			}

			/* User may override Both hardcoded IRQ setup and Plug & Play IRQ */
			keyname[3] = 'A' + (pin-1);
			value = drvmgr_dev_key_get(priv->dev, keyname, DRVMGR_KT_INT);
			if (value)
				grpci2_pci_irq_table[pin-1] = value->i;
		}

		/* Remember which IRQs are enabled */
		if (grpci2_pci_irq_table[pin-1] != 0)
			priv->irq_mask |= 1 << (pin-1);
	}

	/* User may override DEFAULT_BT_ENABLED to enable/disable byte twisting */
	value = drvmgr_dev_key_get(priv->dev, "byteTwisting", DRVMGR_KT_INT);
	if (value)
		priv->bt_enabled = value->i;

	/* Let user Configure the 6 target BARs */
	value = drvmgr_dev_key_get(priv->dev, "tgtBarCfg", DRVMGR_KT_POINTER);
	if (value)
		priv->barcfg = value->ptr;
	else
		priv->barcfg = grpci2_default_bar_mapping;

	/* User may override DEFAULT_LATENCY_TIMER */
	value = drvmgr_dev_key_get(priv->dev, "latencyTimer", DRVMGR_KT_INT);
	if (value)
		priv->latency_timer = value->i;

	/* This driver only support HOST systems, we check that it can act as a 
	 * PCI Master and that it is in the Host slot. */
	if ((priv->regs->sts_cap&STS_HOST) || !(priv->regs->sts_cap&STS_MST))
		return -2; /* Target not supported */

	/* Init the PCI Core */
	if (grpci2_hw_init(priv))
		return -3;

	/* Down streams translation table */
	priv->maps_down[0].name = "AMBA -> PCI MEM Window";
	priv->maps_down[0].size = priv->pci_area_end - priv->pci_area;
	priv->maps_down[0].from_adr = (void *)priv->pci_area;
	priv->maps_down[0].to_adr = (void *)priv->pci_area;
	/* End table */
	priv->maps_down[1].size = 0;

	/* Up streams translation table */
	/* Setup the Host's PCI Target BARs for others to access (DMA) */
	barcfg = priv->barcfg;
	for (i=0,j=0; i<6; i++) {
		size = barcfg[i].barsize;
		if (size == 0)
			continue;

		/* Make sure address is properly aligned */
		priv->maps_up[j].name = "Target BAR[I] -> AMBA";
		priv->maps_up[j].size = size;
		priv->maps_up[j].from_adr = (void *)
					(barcfg[i].pciadr & ~(size - 1));
		priv->maps_up[j].to_adr = (void *)
					(barcfg[i].ahbadr & ~(size - 1));
		j++;
	}

	/* End table */
	priv->maps_up[j].size = 0;

	return 0;
}

/* Called when a core is found with the AMBA device and vendor ID 
 * given in grpci2_ids[]. IRQ, Console does not work here
 */
int grpci2_init1(struct drvmgr_dev *dev)
{
	int status;
	struct grpci2_priv *priv;
	struct pci_auto_setup grpci2_auto_cfg;

	DBG("GRPCI2[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	if (grpci2priv) {
		DBG("Driver only supports one PCI core\n");
		return DRVMGR_FAIL;
	}

	if ((strcmp(dev->parent->dev->drv->name, "AMBAPP_GRLIB_DRV") != 0) &&
	    (strcmp(dev->parent->dev->drv->name, "AMBAPP_LEON2_DRV") != 0)) {
		/* We only support GRPCI2 driver on local bus */
		return DRVMGR_FAIL;
	}

	priv = dev->priv;
	if (!priv)
		return DRVMGR_NOMEM;

	priv->dev = dev;
	grpci2priv = priv;

	/* Initialize GRPCI2 Hardware */
	status = grpci2_init(priv);
	if (status) {
		printf("Failed to initialize grpci2 driver %d\n", status);
		return -1;
	}

	/* Register the PCI core at the PCI layers */

	if (priv->bt_enabled == 0) {
		/* Host is Big-Endian */
		pci_endian = PCI_BIG_ENDIAN;

		memcpy(&grpci2_access_drv.io, &grpci2_io_ops_be,
						sizeof(grpci2_io_ops_be));
		grpci2_access_drv.memreg = &pci_memreg_sparc_be_ops;
	}

	if (pci_access_drv_register(&grpci2_access_drv)) {
		/* Access routines registration failed */
		return DRVMGR_FAIL;
	}

	/* Prepare memory MAP */
	grpci2_auto_cfg.options = 0;
	grpci2_auto_cfg.mem_start = 0;
	grpci2_auto_cfg.mem_size = 0;
	grpci2_auto_cfg.memio_start = priv->pci_area;
	grpci2_auto_cfg.memio_size = priv->pci_area_end - priv->pci_area;
	grpci2_auto_cfg.io_start = 0x100; /* avoid PCI address 0 */
	grpci2_auto_cfg.io_size = 0x10000 - 0x100; /* lower 64kB I/O 16 */
	grpci2_auto_cfg.irq_map = grpci2_bus0_irq_map;
	grpci2_auto_cfg.irq_route = NULL; /* use standard routing */
	pci_config_register(&grpci2_auto_cfg);

	if (pci_config_init()) {
		/* PCI configuration failed */
		return DRVMGR_FAIL;
	}

	/* Initialize/Register Driver Manager PCI Bus */
	priv->config.maps_down = &priv->maps_down[0];
	priv->config.maps_up = &priv->maps_up[0];
	return pcibus_register(dev, &priv->config);
}

int grpci2_init3(struct drvmgr_dev *dev)
{
	struct grpci2_priv *priv = dev->priv;

	/* Install and Enable PCI Error interrupt handler */
	drvmgr_interrupt_register(dev, 0, "grpci2", grpci2_err_isr, priv);

	/* Unmask Error IRQ and all PCI interrupts at PCI Core. For this to be
	 * safe every PCI board have to be resetted (no IRQ generation) before
	 * Global IRQs are enabled (Init is reached or similar)
	 */
	priv->regs->ctrl |= (CTRL_EI | priv->irq_mask);

	return DRVMGR_OK;
}
