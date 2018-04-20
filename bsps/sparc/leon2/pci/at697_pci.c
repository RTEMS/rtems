/*  LEON2 AT697 PCI Host Driver.
 * 
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  Configures the AT697 PCI core and initialize,
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

/* Configurable parameters
 * =======================
 *  INT[A..D]#         Select system IRQ (can be tranlated into I/O interrupt)
 *  INT[A..D]#_PIO     Select PIO used to generate I/O interrupt
 *
 * Notes
 * =====
 *  IRQ must not be enabled before all PCI boards have been enabled, the
 *  IRQ is therefore enabled first in init2. The init2() for this driver
 *  is assumed to be executed earlier that all boards and their devices
 *  driver's init2() function.
 *
 */

#include <rtems/bspIo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libcpu/byteorder.h>
#include <libcpu/access.h>
#include <pci.h>
#include <pci/cfg.h>

#include <drvmgr/drvmgr.h>
#include <drvmgr/pci_bus.h>
#include <drvmgr/leon2_amba_bus.h>

#include <bsp/at697_pci.h>
#include <leon.h>

/* Configuration options */

#define SYSTEM_MAINMEM_START	0x40000000
#define SYSTEM_MAINMEM_START2	0x60000000

/* Interrupt assignment. Set to other value than 0xff in order to 
 * override defaults and plug&play information
 */
#ifndef AT697_INTA_SYSIRQ
 #define AT697_INTA_SYSIRQ 0xff
#endif
#ifndef AT697_INTB_SYSIRQ
 #define AT697_INTB_SYSIRQ 0xff
#endif
#ifndef AT697_INTC_SYSIRQ
 #define AT697_INTC_SYSIRQ 0xff
#endif
#ifndef AT697_INTD_SYSIRQ
 #define AT697_INTD_SYSIRQ 0xff
#endif

#ifndef AT697_INTA_PIO
 #define AT697_INTA_PIO 0xff
#endif
#ifndef AT697_INTB_PIO
 #define AT697_INTB_PIO 0xff
#endif
#ifndef AT697_INTC_PIO
 #define AT697_INTC_PIO 0xff
#endif
#ifndef AT697_INTD_PIO
 #define AT697_INTD_PIO 0xff
#endif


/* AT697 PCI */
#define AT697_PCI_REG_ADR 0x80000100

/* PCI Window used */
#define PCI_MEM_START 0xa0000000
#define PCI_MEM_END   0xf0000000
#define PCI_MEM_SIZE  (PCI_MEM_END - PCI_MEM_START)

/* #define DEBUG 1 */

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...) 
#endif

struct at697pci_regs {
    volatile unsigned int pciid1;        /* 0x80000100 - PCI Device identification register 1         */
    volatile unsigned int pcisc;         /* 0x80000104 - PCI Status & Command                         */
    volatile unsigned int pciid2;        /* 0x80000108 - PCI Device identification register 2         */
    volatile unsigned int pcibhlc;       /* 0x8000010c - BIST, Header type, Cache line size register  */
    volatile unsigned int mbar1;         /* 0x80000110 - Memory Base Address Register 1               */
    volatile unsigned int mbar2;         /* 0x80000114 - Memory Base Address Register 2               */
    volatile unsigned int iobar3;        /* 0x80000118 - IO Base Address Register 3                   */
    volatile unsigned int dummy1[4];     /* 0x8000011c - 0x80000128                                   */ 
    volatile unsigned int pcisid;        /* 0x8000012c - Subsystem identification register            */
    volatile unsigned int dummy2;        /* 0x80000130                                                */
    volatile unsigned int pcicp;         /* 0x80000134 - PCI capabilities pointer register            */
    volatile unsigned int dummy3;        /* 0x80000138                                                */
    volatile unsigned int pcili;         /* 0x8000013c - PCI latency interrupt register               */
    volatile unsigned int pcirt;         /* 0x80000140 - PCI retry, trdy config                       */
    volatile unsigned int pcicw;         /* 0x80000144 - PCI configuration write register             */
    volatile unsigned int pcisa;         /* 0x80000148 - PCI Initiator Start Address                  */
    volatile unsigned int pciiw;         /* 0x8000014c - PCI Initiator Write Register                 */
    volatile unsigned int pcidma;        /* 0x80000150 - PCI DMA configuration register               */
    volatile unsigned int pciis;         /* 0x80000154 - PCI Initiator Status Register                */
    volatile unsigned int pciic;         /* 0x80000158 - PCI Initiator Configuration                  */
    volatile unsigned int pcitpa;        /* 0x8000015c - PCI Target Page Address Register             */   
    volatile unsigned int pcitsc;        /* 0x80000160 - PCI Target Status-Command Register           */
    volatile unsigned int pciite;        /* 0x80000164 - PCI Interrupt Enable Register                */
    volatile unsigned int pciitp;        /* 0x80000168 - PCI Interrupt Pending Register               */
    volatile unsigned int pciitf;        /* 0x8000016c - PCI Interrupt Force Register                 */
    volatile unsigned int pcid;          /* 0x80000170 - PCI Data Register                            */   
    volatile unsigned int pcibe;         /* 0x80000174 - PCI Burst End Register                       */
    volatile unsigned int pcidmaa;       /* 0x80000178 - PCI DMA Address Register                     */
};

/* PCI Interrupt assignment. Connects an PCI interrupt pin (INTA#..INTD#)
 * to a system interrupt number.
 */
unsigned char at697_pci_irq_table[4] =
{
	/* INTA# */	AT697_INTA_SYSIRQ,
	/* INTB# */	AT697_INTB_SYSIRQ,
	/* INTC# */	AT697_INTC_SYSIRQ,
	/* INTD# */	AT697_INTD_SYSIRQ
};

/* PCI Interrupt PIO assignment. Selects which GPIO pin will be used to
 * generate the system IRQ.
 *
 * PCI IRQ -> GPIO -> 4 x I/O select -> System IRQ
 *              ^- pio_table              ^- irq_select
 */
unsigned char at697_pci_irq_pio_table[4] =
{
	/* INTA# */	AT697_INTA_PIO,
	/* INTB# */	AT697_INTB_PIO,
	/* INTC# */	AT697_INTC_PIO,
	/* INTD# */	AT697_INTD_PIO
};

/* Driver private data struture */
struct at697pci_priv {
	struct drvmgr_dev	*dev;
	struct at697pci_regs	*regs;
	int			minor;

	uint32_t		bar1_pci_adr;
	uint32_t		bar2_pci_adr;

	struct drvmgr_map_entry	maps_up[3];
	struct drvmgr_map_entry	maps_down[2];
	struct pcibus_config	config;
};

struct at697pci_priv *at697pcipriv = NULL;
static int at697pci_minor = 0;

int at697pci_init1(struct drvmgr_dev *dev);
int at697pci_init2(struct drvmgr_dev *dev);

/* AT697 PCI DRIVER */

struct drvmgr_drv_ops at697pci_ops = 
{
	.init = {at697pci_init1, at697pci_init2, NULL, NULL},
	.remove = NULL,
	.info = NULL
};

struct leon2_amba_dev_id at697pci_ids[] = 
{
	{LEON2_AMBA_AT697PCI_ID},
	{0}		/* Mark end of table */
};

struct leon2_amba_drv_info at697pci_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_LEON2_AMBA_AT697PCI,	/* Driver ID */
		"AT697PCI_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_LEON2_AMBA,	/* Bus Type */
		&at697pci_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct at697pci_priv),	/* let drvmgr alloc private */
	},
	&at697pci_ids[0]
};

void at697pci_register_drv(void)
{
	DBG("Registering AT697 PCI driver\n");
	drvmgr_drv_register(&at697pci_info.general);
}

/*  The configuration access functions uses the DMA functionality of the
 *  AT697 pci controller to be able access all slots
 */

static int at697pci_cfg_r32(pci_dev_t dev, int offset, uint32_t *val)
{
	struct at697pci_regs *regs;
	volatile unsigned int data = 0;
	unsigned int address;
	int bus = PCI_DEV_BUS(dev);
	int slot = PCI_DEV_SLOT(dev);
	int func = PCI_DEV_FUNC(dev);
	int retval;

	if (slot > 15 || (offset & ~0xfc)) {
		*val = 0xffffffff;
		return PCISTS_EINVAL;
	}

	regs = at697pcipriv->regs;

	regs->pciitp = 0xff; /* clear interrupts */ 

	if ( bus == 0 ) {
		/* PCI Access - TYPE 0 */
		address = (1<<(16+slot)) | (func << 8) | offset;
	} else {
		/* PCI access - TYPE 1 */
		address = ((bus & 0xff) << 16) | ((slot & 0x1f) << 11) |
				(func << 8) | offset | 1;
	}
	regs->pcisa = address;
	regs->pcidma = 0xa01;
	regs->pcidmaa = (unsigned int) &data;

	while (regs->pciitp == 0)
		;

	regs->pciitp = 0xff; /* clear interrupts */ 

	if (regs->pcisc & 0x20000000)  { /* Master Abort */
		regs->pcisc |= 0x20000000;
		*val = 0xffffffff;
		retval = PCISTS_MSTABRT;
	} else {
		*val = data;
		retval = PCISTS_OK;
	}

	DBG("pci_read - bus: %d, dev: %d, fn: %d, off: %d => addr: %x, val: %x\n",
		bus, slot, func, offset,  address, *val); 

	return retval;
}

static int at697pci_cfg_r16(pci_dev_t dev, int ofs, uint16_t *val)
{
	uint32_t v;
	int retval;

	if (ofs & 1)
		return PCISTS_EINVAL;

	retval = at697pci_cfg_r32(dev, ofs & ~0x3, &v);
	*val = 0xffff & (v >> (8*(ofs & 0x3)));

	return retval;
}

static int at697pci_cfg_r8(pci_dev_t dev, int ofs, uint8_t *val)
{
	uint32_t v;
	int retval;

	retval = at697pci_cfg_r32(dev, ofs & ~0x3, &v);

	*val = 0xff & (v >> (8*(ofs & 3)));

	return retval;
}

static int at697pci_cfg_w32(pci_dev_t dev, int offset, uint32_t val)
{
	struct at697pci_regs *regs;
	volatile unsigned int tmp_val = val;
	unsigned int address;
	int bus = PCI_DEV_BUS(dev);
	int slot = PCI_DEV_SLOT(dev);
	int func = PCI_DEV_FUNC(dev);
	int retval;

	if (slot > 15 || (offset & ~0xfc))
		return PCISTS_EINVAL;

	regs = at697pcipriv->regs;

	regs->pciitp = 0xff; /* clear interrupts */

	if ( bus == 0 ) {
		/* PCI Access - TYPE 0 */
		address = (1<<(16+slot)) | (func << 8) | offset;
	} else {
		/* PCI access - TYPE 1 */
		address = ((bus & 0xff) << 16) | ((slot & 0x1f) << 11) |
				(func << 8) | offset | 1;
	}
	regs->pcisa = address;
	regs->pcidma = 0xb01;
	regs->pcidmaa = (unsigned int) &tmp_val;

	while (regs->pciitp == 0)
		;

	if (regs->pcisc & 0x20000000)  { /* Master Abort */
		regs->pcisc |= 0x20000000;
		retval = PCISTS_MSTABRT;
	} else
		retval = PCISTS_OK;

	regs->pciitp = 0xff; /* clear interrupts */

	DBG("pci_write - bus: %d, dev: %d, fn: %d, off: %d => addr: %x, val: %x\n",
		bus, slot, func, offset, address, val);

	return retval;
}

static int at697pci_cfg_w16(pci_dev_t dev, int ofs, uint16_t val)
{
	uint32_t v;
	int retval;

	if (ofs & 1)
		return PCISTS_EINVAL;

	retval = at697pci_cfg_r32(dev, ofs & ~0x3, &v);
	if (retval != PCISTS_OK)
		return retval;

	v = (v & ~(0xffff << (8*(ofs&3)))) | ((0xffff&val) << (8*(ofs&3)));

	return at697pci_cfg_w32(dev, ofs & ~0x3, v);
}

static int at697pci_cfg_w8(pci_dev_t dev, int ofs, uint8_t val)
{
	uint32_t v;

	at697pci_cfg_r32(dev, ofs & ~0x3, &v);

	v = (v & ~(0xff << (8*(ofs&3)))) | ((0xff&val) << (8*(ofs&3)));

	return at697pci_cfg_w32(dev, ofs & ~0x3, v);
}

/* Return the assigned system IRQ number that corresponds to the PCI
 * "Interrupt Pin" information from configuration space.
 *
 * The IRQ information is stored in the at697_pci_irq_table configurable
 * by the user.
 *
 * Returns the "system IRQ" for the PCI INTA#..INTD# pin in irq_pin. Returns
 * 0xff if not assigned.
 */
static uint8_t at697pci_bus0_irq_map(pci_dev_t dev, int irq_pin)
{
	uint8_t sysIrqNr = 0; /* not assigned */
	int irq_group;

	if ( (irq_pin >= 1) && (irq_pin <= 4) ) {
		/* Use default IRQ decoding on PCI BUS0 according slot numbering */
		irq_group = PCI_DEV_SLOT(dev) & 0x3;
		irq_pin = ((irq_pin - 1) + irq_group) & 0x3;
		/* Valid PCI "Interrupt Pin" number */
		sysIrqNr = at697_pci_irq_table[irq_pin];
	}
	return sysIrqNr;
}

static int at697pci_translate(uint32_t *address, int type, int dir)
{
	/* No address translation implmented at this point */
	return 0;
}

extern struct pci_memreg_ops pci_memreg_sparc_be_ops;

/* AT697 Big-Endian PCI access routines */
static struct pci_access_drv at697pci_access_drv = {
	.cfg =
	{
		at697pci_cfg_r8,
		at697pci_cfg_r16,
		at697pci_cfg_r32,
		at697pci_cfg_w8,
		at697pci_cfg_w16,
		at697pci_cfg_w32,
	},
	.io =
	{	/* AT697 only supports non-standard Big-Endian PCI Bus */
		_ld8,
		_ld_be16,
		_ld_be32,
		_st8,
		_st_be16,
		_st_be32,

	},
	.memreg = &pci_memreg_sparc_be_ops,
	.translate = at697pci_translate,
};

/* Initializes the AT697PCI core hardware
 *
 */
static int at697pci_hw_init(struct at697pci_priv *priv)
{
	struct at697pci_regs *regs = priv->regs;
	unsigned short vendor = regs->pciid1 >> 16;

	/* Must match ATMEL or ESA ID */
	if ( !((vendor == 0x1202) || (vendor == 0x1E0F)) ) {
		/* No AT697 PCI, quit */
		return -1;
	}

	/* If not in system slot we are not host and we must abort.
	 * This is a host only driver.
	 */
	if ((regs->pciis & 0x1000) != 0) {
		return -1;
	}

	/* Reset PCI Core */
	regs->pciic = 0xffffffff;

	/* Mask PCI interrupts */
	regs->pciite = 0;

	/* Map parts of AT697 main memory into PCI (for DMA) */
	regs->mbar1  = priv->bar1_pci_adr;
	regs->mbar2  = priv->bar2_pci_adr;
	regs->pcitpa = (priv->bar1_pci_adr & 0xff000000) |
	               ((priv->bar2_pci_adr>>16) & 0xff00);

	/* Enable PCI master and target memory command response  */
	regs->pcisc |= 0x40 | 0x6;

	/* Set latency timer to 64 */
	regs->pcibhlc = 0x00004000;

	/* Set Inititator configuration so that AHB slave accesses generate memory read/write commands */
	regs->pciic = 0x41;

	return 0;
}

/* Initializes the AT697PCI core and driver, must be called before calling init_pci() 
 *
 * Return values
 *  0             Successful initalization
 *  -1            Error during initialization.
 */
static int at697pci_init(struct at697pci_priv *priv)
{
	int pin;
	union drvmgr_key_value *value;
	char keyname_sysirq[6];
	char keyname_pio[10];

	/* PCI core, init private structure */
	priv->regs = (struct at697pci_regs *) AT697_PCI_REG_ADR;

	/* Init PCI interrupt assignment table to all use the interrupt routed
	 * through the GPIO core.
	 *
	 * INT[A..D]# selects system IRQ (and I/O interrupt)
	 * INT[A..D]#_PIO selects PIO used to generate I/O interrupt
	 */
	strcpy(keyname_sysirq, "INTX#");
	strcpy(keyname_pio, "INTX#_PIO");
	for (pin=1; pin<5; pin++) {
		if ( at697_pci_irq_table[pin-1] == 0xff ) {
			/* User may override hardcoded IRQ setup */
			keyname_sysirq[3] = 'A' + (pin-1);
			value = drvmgr_dev_key_get(priv->dev,
					keyname_sysirq, DRVMGR_KT_INT);
			if ( value )
				at697_pci_irq_table[pin-1] = value->i;
		}
		if ( at697_pci_irq_pio_table[pin-1] == 0xff ) {
			/* User may override hardcoded IRQ setup */
			keyname_pio[3] = 'A' + (pin-1);
			value = drvmgr_dev_key_get(priv->dev,
						keyname_pio, DRVMGR_KT_INT);
			if ( value )
				at697_pci_irq_pio_table[pin-1] = value->i;
		}
	}

	/* Use GRPCI target BAR1 and BAR2 to map CPU RAM to PCI, this is to
	 * make it possible for PCI peripherals to do DMA directly to CPU memory
	 *
	 * Defualt is to map system RAM at pci address 0x40000000 and system
	 * SDRAM to pci address 0x60000000
	 */
	value = drvmgr_dev_key_get(priv->dev, "tgtbar1", DRVMGR_KT_INT);
	if (value)
		priv->bar1_pci_adr = value->i;
	else
		priv->bar1_pci_adr = SYSTEM_MAINMEM_START; /* default */

	value = drvmgr_dev_key_get(priv->dev, "tgtbar2", DRVMGR_KT_INT);
	if (value)
		priv->bar2_pci_adr = value->i;
	else
		priv->bar2_pci_adr = SYSTEM_MAINMEM_START2; /* default */

	/* Init the PCI Core */
	if ( at697pci_hw_init(priv) ) {
		return -3;
	}

	/* Down streams translation table */
	priv->maps_down[0].name = "AMBA -> PCI MEM Window";
	priv->maps_down[0].size = 0xF0000000 - 0xA0000000;
	priv->maps_down[0].from_adr = (void *)0xA0000000;
	priv->maps_down[0].to_adr = (void *)0xA0000000;
	/* End table */
	priv->maps_down[1].size = 0;

	/* Up streams translation table, 2x16Mb mapped 1:1  */
	priv->maps_up[0].name = "Target BAR0 -> AMBA";
	priv->maps_up[0].size = 0x01000000; /* 16Mb BAR1 */
	priv->maps_up[0].from_adr = (void *)priv->bar1_pci_adr;
	priv->maps_up[0].to_adr = (void *)priv->bar1_pci_adr;
	priv->maps_up[1].name = "Target BAR1 -> AMBA";
	priv->maps_up[1].size = 0x01000000; /* 16Mb BAR2 */
	priv->maps_up[1].from_adr = (void *)priv->bar2_pci_adr;
	priv->maps_up[1].to_adr = (void *)priv->bar2_pci_adr;
	/* End table */
	priv->maps_up[2].size = 0;

	return 0;
}

/* Called when a core is found with the AMBA device and vendor ID 
 * given in at697pci_ids[].
 */
int at697pci_init1(struct drvmgr_dev *dev)
{
	struct at697pci_priv *priv;
	struct pci_auto_setup at697pci_auto_cfg;

	DBG("AT697PCI[%d] on bus %s\n", dev->minor_drv,
		dev->parent->dev->name);

	if ( at697pci_minor != 0 ) {
		DBG("Driver only supports one PCI core\n");
		return DRVMGR_FAIL;
	}

	at697pcipriv = priv = dev->priv;
	if ( !priv )
		return DRVMGR_NOMEM;

	priv->dev = dev;
	priv->minor = at697pci_minor++;

	if (at697pci_init(priv)) {
		DBG("Failed to initialize at697pci driver\n");
		return DRVMGR_EIO;
	}

	/* Host is always Big-Endian */
	pci_endian = PCI_BIG_ENDIAN;

	if (pci_access_drv_register(&at697pci_access_drv)) {
		/* Access routines registration failed */
		return DRVMGR_FAIL;
	}

	/* Prepare memory MAP */
	at697pci_auto_cfg.options = 0;
	at697pci_auto_cfg.mem_start = 0;
	at697pci_auto_cfg.mem_size = 0;
	at697pci_auto_cfg.memio_start = PCI_MEM_START;
	at697pci_auto_cfg.memio_size = PCI_MEM_SIZE;
	at697pci_auto_cfg.io_start = 0;
	at697pci_auto_cfg.io_size = 0;
	at697pci_auto_cfg.irq_map = at697pci_bus0_irq_map;
	at697pci_auto_cfg.irq_route = NULL; /* use standard routing */
	pci_config_register(&at697pci_auto_cfg);

	if (pci_config_init()) {
		/* PCI configuration failed */
		return DRVMGR_FAIL;
	}

	priv->config.maps_down = &priv->maps_down[0];
	priv->config.maps_up = &priv->maps_up[0];
	return pcibus_register(dev, &priv->config);
}

int at697pci_init2(struct drvmgr_dev *dev)
{
#if 0
	struct at697pci_priv *priv = dev->priv;
#endif
	int pin, irq, pio, ioport;
	LEON_Register_Map *regs = (LEON_Register_Map *)0x80000000;

	/* Enable interrupts now that init1 has been reached for all devices
	 * on the bus.
	 */

	for (pin=1; pin<5; pin++) {
		irq = at697_pci_irq_table[pin-1];
		pio = at697_pci_irq_pio_table[pin-1];
		if ( (pio < 16) && (irq >= 4) && (irq <= 7) ) {
			/* AT697 I/O IRQ, we know how to set up this 
			 *
			 * IRQ 4 -> I/O 0
			 * IRQ 5 -> I/O 1
			 * IRQ 6 -> I/O 2
			 * IRQ 7 -> I/O 3
			 */
			ioport = irq - 4;

			/* First disable interrupts */
			regs->PIO_Interrupt &= ~(0xff << (ioport * 8));
			/* Set PIO as input pin */
			regs->PIO_Direction &= ~(1 << pio);
			/* Set Low Level sensitivity */
			regs->PIO_Interrupt |= ((0x80 | pio) << (ioport * 8));
		}
	}

	/* Unmask Interrupt */
	/*priv->regs->pciite = 0xff;*/

	return DRVMGR_OK;
}
