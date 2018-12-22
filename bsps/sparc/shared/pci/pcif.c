/*  GRLIB PCIF PCI HOST driver.
 * 
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  Configures the PCIF core and initialize,
 *   - the PCI Library (pci.c)
 *   - the general part of the PCI Bus driver (pci_bus.c)
 *  
 *  System interrupt assigned to PCI interrupt (INTA#..INTD#) is by
 *  default taken from Plug and Play, but may be overridden by the 
 *  driver resources INTA#..INTD#.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libcpu/byteorder.h>
#include <libcpu/access.h>
#include <rtems/bspIo.h>
#include <pci.h>
#include <pci/cfg.h>

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <grlib/ambapp.h>
#include <drvmgr/pci_bus.h>
#include <grlib/pcif.h>


/* Configuration options */
#define SYSTEM_MAINMEM_START 0x40000000

/* Interrupt assignment. Set to other value than 0xff in order to 
 * override defaults and plug&play information
 */
#ifndef PCIF_INTA_SYSIRQ
 #define PCIF_INTA_SYSIRQ 0xff
#endif
#ifndef PCIF_INTB_SYSIRQ
 #define PCIF_INTB_SYSIRQ 0xff
#endif
#ifndef PCIF_INTC_SYSIRQ
 #define PCIF_INTC_SYSIRQ 0xff
#endif
#ifndef PCIF_INTD_SYSIRQ
 #define PCIF_INTD_SYSIRQ 0xff
#endif

/*#define DEBUG 1  */

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...) 
#endif

/*
 * Bit encode for PCI_CONFIG_HEADER_TYPE register
 */
struct pcif_regs {
	volatile unsigned int bars[4];  /* 0x00-0x10 */
	volatile unsigned int bus;      /* 0x10 */
	volatile unsigned int map_io;   /* 0x14 */
	volatile unsigned int status;   /* 0x18 */
	volatile unsigned int intr;     /* 0x1c */
	int unused[(0x40-0x20)/4];      /* 0x20-0x40 */
	volatile unsigned int maps[(0x80-0x40)/4];   /* 0x40-0x80*/
};

/* Used internally for accessing the PCI bridge's configuration space itself */
#define HOST_TGT PCI_DEV(0xff, 0, 0)

struct pcif_priv *pcifpriv = NULL;
static int pcif_minor = 0;

/* PCI Interrupt assignment. Connects an PCI interrupt pin (INTA#..INTD#)
 * to a system interrupt number.
 */
unsigned char pcif_pci_irq_table[4] =
{
	/* INTA# */	PCIF_INTA_SYSIRQ,
	/* INTB# */	PCIF_INTB_SYSIRQ,
	/* INTC# */	PCIF_INTC_SYSIRQ,
	/* INTD# */	PCIF_INTD_SYSIRQ
};

/* Driver private data struture */
struct pcif_priv {
	struct drvmgr_dev	*dev;
	struct pcif_regs		*regs;
	int				irq;
	int				minor;
	int				irq_mask;

	unsigned int			pci_area;
	unsigned int			pci_area_end;
	unsigned int			pci_io;    
	unsigned int			pci_conf;
	unsigned int			pci_conf_end;

	uint32_t			devVend; /* Host PCI Vendor/Device ID */
	uint32_t			bar1_size;

	struct drvmgr_map_entry		maps_up[2];
	struct drvmgr_map_entry		maps_down[2];
	struct pcibus_config		config;
};

int pcif_init1(struct drvmgr_dev *dev);
int pcif_init3(struct drvmgr_dev *dev);

/* PCIF DRIVER */

struct drvmgr_drv_ops pcif_ops = 
{
	.init = {pcif_init1, NULL, pcif_init3, NULL},
	.remove = NULL,
	.info = NULL
};

struct amba_dev_id pcif_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_PCIF},
	{0, 0}		/* Mark end of table */
};

struct amba_drv_info pcif_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_PCIF_ID,	/* Driver ID */
		"PCIF_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&pcif_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct pcif_priv),	/* Let drvmgr alloc private */
	},
	&pcif_ids[0]
};

void pcif_register_drv(void)
{
	DBG("Registering PCIF driver\n");
	drvmgr_drv_register(&pcif_info.general);
}

static int pcif_cfg_r32(pci_dev_t dev, int ofs, uint32_t *val)
{
	struct pcif_priv *priv = pcifpriv;
	volatile uint32_t *pci_conf;
	uint32_t devfn;
	int retval;
	int bus = PCI_DEV_BUS(dev);

	if (ofs & 3)
		return PCISTS_EINVAL;

	if (PCI_DEV_SLOT(dev) > 15) {
		*val = 0xffffffff;
		return PCISTS_OK;
	}

	/* PCIF can access "non-standard" devices on bus0 (on AD11.AD16), 
	 * but we skip them.
	 */
	if (dev == HOST_TGT)
		bus = devfn = 0;
	else if (bus == 0)
		devfn = PCI_DEV_DEVFUNC(dev) + PCI_DEV(0, 6, 0);
	else
		devfn = PCI_DEV_DEVFUNC(dev);

	/* Select bus */
	priv->regs->bus = bus << 16;

	pci_conf = (volatile uint32_t *)(priv->pci_conf | (devfn << 8) | ofs);

	*val = *pci_conf;

	if (priv->regs->status & 0x30000000) {
		*val = 0xffffffff;
		retval = PCISTS_MSTABRT;
	} else
		retval = PCISTS_OK;

	DBG("pci_read: [%x:%x:%x] reg: 0x%x => addr: 0x%x, val: 0x%x\n",
		PCI_DEV_EXPAND(dev), ofs, pci_conf, *val);

	return retval;
}

static int pcif_cfg_r16(pci_dev_t dev, int ofs, uint16_t *val)
{
	uint32_t v;
	int retval;

	if (ofs & 1)
		return PCISTS_EINVAL;

	retval = pcif_cfg_r32(dev, ofs & ~0x3, &v);
	*val = 0xffff & (v >> (8*(ofs & 0x3)));

	return retval;
}

static int pcif_cfg_r8(pci_dev_t dev, int ofs, uint8_t *val)
{
	uint32_t v;
	int retval;

	retval = pcif_cfg_r32(dev, ofs & ~0x3, &v);

	*val = 0xff & (v >> (8*(ofs & 3)));

	return retval;
}

static int pcif_cfg_w32(pci_dev_t dev, int ofs, uint32_t val)
{
	struct pcif_priv *priv = pcifpriv;
	volatile uint32_t *pci_conf;
	uint32_t devfn;
	int bus = PCI_DEV_BUS(dev);

	if (ofs & ~0xfc)
		return PCISTS_EINVAL;

	if (PCI_DEV_SLOT(dev) > 15)
		return PCISTS_MSTABRT;

	/* PCIF can access "non-standard" devices on bus0 (on AD11.AD16), 
	 * but we skip them.
	 */
	if (dev == HOST_TGT)
		bus = devfn = 0;
	else if (bus == 0)
		devfn = PCI_DEV_DEVFUNC(dev) + PCI_DEV(0, 6, 0);
	else
		devfn = PCI_DEV_DEVFUNC(dev);

	/* Select bus */
	priv->regs->bus = bus << 16;

	pci_conf = (volatile uint32_t *)(priv->pci_conf | (devfn << 8) | ofs);

	*pci_conf = val;

	DBG("pci_write - [%x:%x:%x] reg: 0x%x => addr: 0x%x, val: 0x%x\n",
		PCI_DEV_EXPAND(dev), ofs, pci_conf, value);

	return PCISTS_OK;
}

static int pcif_cfg_w16(pci_dev_t dev, int ofs, uint16_t val)
{
	uint32_t v;
	int retval;

	if (ofs & 1)
		return PCISTS_EINVAL;

	retval = pcif_cfg_r32(dev, ofs & ~0x3, &v);
	if (retval != PCISTS_OK)
		return retval;

	v = (v & ~(0xffff << (8*(ofs&3)))) | ((0xffff&val) << (8*(ofs&3)));

	return pcif_cfg_w32(dev, ofs & ~0x3, v);
}

static int pcif_cfg_w8(pci_dev_t dev, int ofs, uint8_t val)
{
	uint32_t v;
	int retval;

	retval = pcif_cfg_r32(dev, ofs & ~0x3, &v);
	if (retval != PCISTS_OK)
		return retval;

	v = (v & ~(0xff << (8*(ofs&3)))) | ((0xff&val) << (8*(ofs&3)));

	return pcif_cfg_w32(dev, ofs & ~0x3, v);
}


/* Return the assigned system IRQ number that corresponds to the PCI
 * "Interrupt Pin" information from configuration space.
 *
 * The IRQ information is stored in the pcif_pci_irq_table configurable
 * by the user.
 *
 * Returns the "system IRQ" for the PCI INTA#..INTD# pin in irq_pin. Returns
 * 0xff if not assigned.
 */
static uint8_t pcif_bus0_irq_map(pci_dev_t dev, int irq_pin)
{
	uint8_t sysIrqNr = 0; /* not assigned */
	int irq_group;

	if ( (irq_pin >= 1) && (irq_pin <= 4) ) {
		/* Use default IRQ decoding on PCI BUS0 according slot numbering */
		irq_group = PCI_DEV_SLOT(dev) & 0x3;
		irq_pin = ((irq_pin - 1) + irq_group) & 0x3;
		/* Valid PCI "Interrupt Pin" number */
		sysIrqNr = pcif_pci_irq_table[irq_pin];
	}
	return sysIrqNr;
}

static int pcif_translate(uint32_t *address, int type, int dir)
{
	/* No address translation implmented at this point */
	return 0;
}

extern struct pci_memreg_ops pci_memreg_sparc_be_ops;

/* PCIF Big-Endian PCI access routines */
struct pci_access_drv pcif_access_drv = {
	.cfg =
	{
		pcif_cfg_r8,
		pcif_cfg_r16,
		pcif_cfg_r32,
		pcif_cfg_w8,
		pcif_cfg_w16,
		pcif_cfg_w32,
	},
	.io =	/* PCIF only supports Big-endian */
	{
		_ld8,
		_ld_be16,
		_ld_be32,
		_st8,
		_st_be16,
		_st_be32,
	},
	.memreg = &pci_memreg_sparc_be_ops,
	.translate = pcif_translate,
};

/* Initializes the PCIF core hardware
 *
 */
static int pcif_hw_init(struct pcif_priv *priv)
{
	struct pcif_regs *regs;
	uint32_t data, size;
	int mst;
	pci_dev_t host = HOST_TGT;

	regs = priv->regs;

	/* Mask PCI interrupts */
	regs->intr = 0;

	/* Get the PCIF Host PCI ID */
	pcif_cfg_r32(host, PCIR_VENDOR, &priv->devVend);

	/* set 1:1 mapping between AHB -> PCI memory space, for all Master cores */
	for ( mst=0; mst<16; mst++) {
		regs->maps[mst] = priv->pci_area;

		/* Check if this register is implemented */
		if ( regs->maps[mst] != priv->pci_area )
			break;
	}

	/* and map system RAM at pci address SYSTEM_MAINMEM_START. This way
	 * PCI targets can do DMA directly into CPU main memory.
	 */
	regs->bars[0] = SYSTEM_MAINMEM_START;
	regs->bars[1] = 0;
	regs->bars[2] = 0;
	regs->bars[3] = 0;

	/* determine size of target BAR1 */
	pcif_cfg_w32(host, PCIR_BAR(1), 0xffffffff);
	pcif_cfg_r32(host, PCIR_BAR(1), &size);
	priv->bar1_size = (~(size & ~0xf)) + 1;

	pcif_cfg_w32(host, PCIR_BAR(0), 0);
	pcif_cfg_w32(host, PCIR_BAR(1), SYSTEM_MAINMEM_START);
	pcif_cfg_w32(host, PCIR_BAR(2), 0);
	pcif_cfg_w32(host, PCIR_BAR(3), 0);
	pcif_cfg_w32(host, PCIR_BAR(4), 0);
	pcif_cfg_w32(host, PCIR_BAR(5), 0);

	/* set as bus master and enable pci memory responses */  
	pcif_cfg_r32(host, PCIR_COMMAND, &data);
	data |= (PCIM_CMD_MEMEN | PCIM_CMD_BUSMASTEREN);
	pcif_cfg_w32(host, PCIR_COMMAND, data);

	/* Successful */
	return 0;
}

/* Initializes the PCIF core and driver, must be called before calling init_pci() 
 *
 * Return values
 *  0             Successful initalization
 *  -1            Error during initialization, for example "PCI core not found".
 *  -2            Error PCI controller not HOST (targets not supported)
 *  -3            Error due to PCIF hardware initialization
 *  -4            Error registering driver to PCI layer
 */
static int pcif_init(struct pcif_priv *priv)
{
	struct ambapp_apb_info *apb;
	struct ambapp_ahb_info *ahb;
	int pin;
	union drvmgr_key_value *value;
	char keyname[6];
	struct amba_dev_info *ainfo = priv->dev->businfo;

	/* Find PCI core from Plug&Play information */
	apb = ainfo->info.apb_slv;
	ahb = ainfo->info.ahb_slv;

	/* Found PCI core, init private structure */
	priv->irq = apb->irq;
	priv->regs = (struct pcif_regs *)apb->start;

	/* Calculate the PCI windows 
	 *  AMBA->PCI Window:                       AHB SLAVE AREA0
	 *  AMBA->PCI I/O cycles Window:            AHB SLAVE AREA1 Lower half
	 *  AMBA->PCI Configuration cycles Window:  AHB SLAVE AREA1 Upper half
	 */
	priv->pci_area     = ahb->start[0];
	priv->pci_area_end = ahb->start[0] + ahb->mask[0];
	priv->pci_io       = ahb->start[1];
	priv->pci_conf     = ahb->start[1] + (ahb->mask[1] >> 1);
	priv->pci_conf_end = ahb->start[1] + ahb->mask[1];

	/* On systems where PCI I/O area and configuration area is apart of the "PCI Window" 
	 * the PCI Window stops at the start of the PCI I/O area
	 */
	if ( (priv->pci_io > priv->pci_area) && (priv->pci_io < (priv->pci_area_end-1)) ) {
		priv->pci_area_end = priv->pci_io;
	}

	/* Init PCI interrupt assignment table to all use the interrupt routed through
	 * the PCIF core.
	 */
	strcpy(keyname, "INTX#");
	for (pin=1; pin<5; pin++) {
		if ( pcif_pci_irq_table[pin-1] == 0xff ) {
			pcif_pci_irq_table[pin-1] = priv->irq;

			/* User may override Plug & Play IRQ */
			keyname[3] = 'A' + (pin-1);
			value = drvmgr_dev_key_get(priv->dev, keyname, DRVMGR_KT_INT);
			if ( value )
				pcif_pci_irq_table[pin-1] = value->i;
		}
	}

	priv->irq_mask = 0xf;
	value = drvmgr_dev_key_get(priv->dev, "", DRVMGR_KT_INT);
	if ( value )
		priv->irq_mask = value->i & 0xf;

	/* This driver only support HOST systems, we check for HOST */
	if ( priv->regs->status & 0x00000001 ) {
		/* Target not supported */
		return -2;
	}

	/* Init the PCI Core */
	if ( pcif_hw_init(priv) ) {
		return -3;
	}

	/* Down streams translation table */
	priv->maps_down[0].name = "AMBA -> PCI MEM Window";
	priv->maps_down[0].size = priv->pci_area_end - priv->pci_area;
	priv->maps_down[0].from_adr = (void *)priv->pci_area;
	priv->maps_down[0].to_adr = (void *)priv->pci_area;
	/* End table */
	priv->maps_down[1].size = 0;

	/* Up streams translation table */
	priv->maps_up[0].name = "Target BAR1 -> AMBA";
	priv->maps_up[0].size = priv->bar1_size;
	priv->maps_up[0].from_adr = (void *)SYSTEM_MAINMEM_START;
	priv->maps_up[0].to_adr = (void *)SYSTEM_MAINMEM_START;
	/* End table */
	priv->maps_up[1].size = 0;

	return 0;
}

/* Called when a core is found with the AMBA device and vendor ID 
 * given in pcif_ids[].
 */
int pcif_init1(struct drvmgr_dev *dev)
{
	struct pcif_priv *priv;
	struct pci_auto_setup pcif_auto_cfg;

	DBG("PCIF[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);

	if ( pcif_minor != 0 ) {
		printk("Driver only supports one PCI core\n");
		return DRVMGR_FAIL;
	}

	priv = dev->priv;
	if ( !priv )
		return DRVMGR_NOMEM;

	dev->priv = priv;
	priv->dev = dev;
	priv->minor = pcif_minor++;

	pcifpriv = priv;
	if ( pcif_init(priv) ) {
		printk("Failed to initialize PCIF driver\n");
		free(priv);
		dev->priv = NULL;
		return DRVMGR_FAIL;
	}

	/* Host is always Big-Endian */
	pci_endian = PCI_BIG_ENDIAN;

	/* Register the PCI core at the PCI layer */

	if (pci_access_drv_register(&pcif_access_drv)) {
		/* Access routines registration failed */
		return DRVMGR_FAIL;
	}

	/* Prepare memory MAP */
	pcif_auto_cfg.options = 0;
	pcif_auto_cfg.mem_start = 0;
	pcif_auto_cfg.mem_size = 0;
	pcif_auto_cfg.memio_start = priv->pci_area;
	pcif_auto_cfg.memio_size = priv->pci_area_end - priv->pci_area;
	pcif_auto_cfg.io_start = priv->pci_io;
	pcif_auto_cfg.io_size = priv->pci_conf - priv->pci_io;
	pcif_auto_cfg.irq_map = pcif_bus0_irq_map;
	pcif_auto_cfg.irq_route = NULL; /* use standard routing */
	pci_config_register(&pcif_auto_cfg);

	if (pci_config_init()) {
		/* PCI configuration failed */
		return DRVMGR_FAIL;
	}

	priv->config.maps_down = &priv->maps_down[0];
	priv->config.maps_up = &priv->maps_up[0];
	return pcibus_register(dev, &priv->config);
}

int pcif_init3(struct drvmgr_dev *dev)
{
	struct pcif_priv *priv = dev->priv;

	/* Unmask all interrupts, on some sytems this 
	 * might be problematic because all PCI IRQs are
	 * not connected on the PCB or used for something
	 * else. The irqMask driver resource can be used to 
	 * control which PCI IRQs are used to generate the
	 * PCI system IRQ, example:
	 *
	 * 0xf - enable all  (DEFAULT)
	 * 0x8 - enable one PCI irq
	 *
	 * Before unmasking PCI IRQ, all PCI boards must
	 * have been initialized and IRQ turned off to avoid
	 * system hang.
	 */

	priv->regs->intr = priv->irq_mask;

	return DRVMGR_OK;
}
