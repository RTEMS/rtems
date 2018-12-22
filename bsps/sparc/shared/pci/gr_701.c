/*  GR-701 PCI Target driver.
 * 
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  Configures the GR-701 interface PCI board.
 *  This driver provides a AMBA PnP bus by using the general part
 *  of the AMBA PnP bus driver (ambapp_bus.c).
 *
 *  Driver resources for the AMBA PnP bus provided can be set using
 *  gr701_set_resources().
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <bsp.h>
#include <rtems/bspIo.h>
#include <pci.h>
#include <pci/access.h>

#include <grlib/ambapp.h>

#include <grlib/ambapp.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <drvmgr/pci_bus.h>
#include <grlib/bspcommon.h>
#include <grlib/genirq.h>

#include <grlib/gr_701.h>

#include <grlib/grlib_impl.h>

/* Offset from 0x80000000 (dual bus version) */
#define AHB1_BASE_ADDR 0x80000000
#define AHB1_IOAREA_BASE_ADDR 0x80100000

/* #define DEBUG 1 */

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...) 
#endif

int gr701_init1(struct drvmgr_dev *dev);
int gr701_init2(struct drvmgr_dev *dev);
void gr701_interrupt(void *arg);

#define READ_REG(address) (*(volatile unsigned int *)address)

/* PCI bride reg layout on AMBA side */
struct amba_bridge_regs {
	volatile unsigned int bar0;
	volatile unsigned int bar1;
	volatile unsigned int bar2;
	volatile unsigned int bar3;
	volatile unsigned int bar4;/* 0x10 */
	
	volatile unsigned int unused[4*3-1];
	
	volatile unsigned int ambabars[1]; /* 0x40 */
};

/* PCI bride reg layout on PCI side */
struct pci_bridge_regs {
	volatile unsigned int bar0;
	volatile unsigned int bar1;
	volatile unsigned int bar2;
	volatile unsigned int bar3;
	volatile unsigned int bar4; /* 0x10 */

	volatile unsigned int ilevel;
	volatile unsigned int ipend;
	volatile unsigned int iforce;
	volatile unsigned int istatus;
	volatile unsigned int iclear;
	volatile unsigned int imask;
};

/* Private data structure for driver */
struct gr701_priv {
	/* Driver management */
	struct drvmgr_dev		*dev;
	char				prefix[16];
	SPIN_DECLARE(devlock);

	struct pci_bridge_regs		*pcib;
	struct amba_bridge_regs		*ambab;

	/* PCI */
	pci_dev_t			pcidev;
	struct pci_dev_info		*devinfo;	

	/* IRQ */
	genirq_t			genirq;
	int				interrupt_cnt;

	/* GR-701 Address translation */
	struct drvmgr_map_entry		bus_maps_up[2];
	struct drvmgr_map_entry		bus_maps_down[2];

	/* AMBA Plug&Play information on GR-701 */
	struct ambapp_bus		abus;
	struct ambapp_mmap		amba_maps[3];
        struct ambapp_config		config;
};

int ambapp_gr701_int_register(
	struct drvmgr_dev *dev,
	int irq,
	const char *info,
	drvmgr_isr handler,
	void *arg);
int ambapp_gr701_int_unregister(
	struct drvmgr_dev *dev,
	int irq,
	drvmgr_isr isr,
	void *arg);
int ambapp_gr701_int_unmask(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_gr701_int_mask(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_gr701_int_clear(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_gr701_get_params(
	struct drvmgr_dev *dev,
	struct drvmgr_bus_params *params);

struct ambapp_ops ambapp_gr701_ops = {
	.int_register = ambapp_gr701_int_register,
	.int_unregister = ambapp_gr701_int_unregister,
	.int_unmask = ambapp_gr701_int_unmask,
	.int_mask = ambapp_gr701_int_mask,
	.int_clear = ambapp_gr701_int_clear,
	.get_params = ambapp_gr701_get_params
};

struct drvmgr_drv_ops gr701_ops = 
{
	.init = {gr701_init1, gr701_init2, NULL, NULL},
	.remove = NULL,
	.info = NULL
};

struct pci_dev_id_match gr701_ids[] = 
{
	PCIID_DEVVEND(PCIID_VENDOR_GAISLER, PCIID_DEVICE_GR_701),
	PCIID_END_TABLE /* Mark end of table */
};

struct pci_drv_info gr701_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_PCI_GAISLER_GR701_ID,	/* Driver ID */
		"GR-701_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_PCI,		/* Bus Type */
		&gr701_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		0,
	},
	&gr701_ids[0]
};

/* Driver resources configuration for the AMBA bus on the GR-701 board.
 * It is declared weak so that the user may override it from the project file,
 * if the default settings are not enough.
 *
 * The configuration consists of an array of configuration pointers, each
 * pointer determine the configuration of one GR-701 board. Pointer
 * zero is for board0, pointer 1 for board1 and so on.
 *
 * The array must end with a NULL pointer.
 */
struct drvmgr_bus_res *gr701_resources[] __attribute__((weak)) =
{
	NULL
};

void gr701_register_drv(void)
{
	DBG("Registering GR-701 PCI driver\n");
	drvmgr_drv_register(&gr701_info.general);
}

void gr701_interrupt(void *arg)
{
	struct gr701_priv *priv = arg;
	unsigned int status;
	int irq = 0;
	SPIN_ISR_IRQFLAGS(irqflags);

	SPIN_LOCK(&priv->devlock, irqflags);
	while ( (status=priv->pcib->istatus) != 0 ) {
		priv->interrupt_cnt++;	/* An interrupt was generated */
		irq = status;
		genirq_doirq(priv->genirq, irq);
		/* ACK interrupt */
		priv->pcib->istatus = 0;
	}
	SPIN_UNLOCK(&priv->devlock, irqflags);

	/* ACK interrupt, this is because PCI is Level, so the IRQ Controller still drives the IRQ. */
	if ( irq )
		drvmgr_interrupt_clear(priv->dev, 0);
}

static int gr701_hw_init(struct gr701_priv *priv)
{
	uint32_t com1;
	struct pci_bridge_regs *pcib;
	struct amba_bridge_regs *ambab;
	int mst;
	unsigned int pci_freq_hz;
	pci_dev_t pcidev = priv->pcidev;
	struct pci_dev_info *devinfo = priv->devinfo;

	/* Set up PCI ==> AMBA */
	priv->pcib = pcib = (void *)devinfo->resources[0].address;
	pcib->bar0 = 0xfc000000;

	/* Set up GR701 AMBA Masters connection to PCI */
	priv->ambab = ambab = (struct amba_bridge_regs *)(
		devinfo->resources[1].address + 0x400);

	/* Init all msters, max 16 */
	for (mst=0; mst<16; mst++) {
		ambab->ambabars[mst] = 0x40000000;
		if (READ_REG(&ambab->ambabars[mst]) != 0x40000000)
			break;
	}

	/* Setup Address translation for AMBA bus, assume that PCI BAR
	 * are mapped 1:1 to CPU.
	 */

	priv->amba_maps[0].size = 0x04000000;
	priv->amba_maps[0].local_adr = devinfo->resources[1].address;
	priv->amba_maps[0].remote_adr = 0xfc000000;

	/* Mark end of table */
	priv->amba_maps[1].size=0;
	priv->amba_maps[1].local_adr = 0;
	priv->amba_maps[1].remote_adr = 0;

	/* Setup DOWN-streams address translation */
	priv->bus_maps_down[0].name = "PCI BAR1 -> AMBA";
	priv->bus_maps_down[0].size = priv->amba_maps[0].size;
	priv->bus_maps_down[0].from_adr = (void *)devinfo->resources[1].address;
	priv->bus_maps_down[0].to_adr = (void *)0xfc000000;

	/* Setup UP-streams address translation */
	priv->bus_maps_up[0].name = "AMBA PCIF Window";
	priv->bus_maps_up[0].size = 0x10000000;
	priv->bus_maps_up[0].from_adr = (void *)0xe0000000;
	priv->bus_maps_up[0].to_adr = (void *)0x40000000;

	/* Mark end of translation tables */
	priv->bus_maps_down[1].size = 0;
	priv->bus_maps_up[1].size = 0;

	/* Enable I/O and Mem accesses */
	pci_cfg_r32(pcidev, PCIR_COMMAND, &com1);
	com1 |= PCIM_CMD_PORTEN | PCIM_CMD_MEMEN;
	pci_cfg_w32(pcidev, PCIR_COMMAND, com1);

	/* Start AMBA PnP scan at first AHB bus */
	ambapp_scan(&priv->abus, devinfo->resources[1].address + 0x3f00000,
			NULL, &priv->amba_maps[0]);

	/* Frequency is the same as the PCI bus frequency */
	drvmgr_freq_get(priv->dev, 0, &pci_freq_hz);

	/* Initialize Frequency of AMBA bus */
	ambapp_freq_init(&priv->abus, NULL, pci_freq_hz);

	/* Init IRQ controller (avoid IRQ generation) */
	pcib->imask = 0x0000;
	pcib->ipend = 0;
	pcib->iclear = 0xffff;
	pcib->iforce = 0;
	pcib->ilevel = 0x0;

	/* Successfully registered the GR-701 board */
	return 0;
}

static void gr701_hw_init2(struct gr701_priv *priv)
{
	/* Enable PCI Master (for DMA) */
	pci_master_enable(priv->pcidev);
}

/* Called when a PCI target is found with the PCI device and vendor ID 
 * given in gr701_ids[].
 */
int gr701_init1(struct drvmgr_dev *dev)
{
	struct gr701_priv *priv;
	struct pci_dev_info *devinfo;
	uint32_t bar0, bar1, bar0_size, bar1_size;
	int resources_cnt;

	priv = grlib_calloc(1, sizeof(*priv));
	if ( !priv )
		return DRVMGR_NOMEM;

	dev->priv = priv;
	priv->dev = dev;

	/* Determine number of configurations */
	resources_cnt = get_resarray_count(gr701_resources);

	/* Generate Device prefix */
	strcpy(priv->prefix, "/dev/gr701_0");
	priv->prefix[11] += dev->minor_drv;
	mkdir(priv->prefix, S_IRWXU | S_IRWXG | S_IRWXO);
	priv->prefix[12] = '/';
	priv->prefix[13] = '\0';

	priv->devinfo = devinfo = (struct pci_dev_info *)dev->businfo;
	priv->pcidev = devinfo->pcidev;
	bar0 = devinfo->resources[0].address;
	bar0_size = devinfo->resources[0].size;
	bar1 = devinfo->resources[1].address;
	bar1_size = devinfo->resources[1].size;
	printk("\n\n--- GR-701[%d] ---\n", dev->minor_drv);
	printk(" PCI BUS: 0x%x, SLOT: 0x%x, FUNCTION: 0x%x\n",
		PCI_DEV_EXPAND(priv->pcidev));
	printk(" PCI VENDOR: 0x%04x, DEVICE: 0x%04x\n\n\n",
		devinfo->id.vendor, devinfo->id.device);
	printk(" PCI BAR[0]: 0x%" PRIx32 " - 0x%" PRIx32 "\n",
		bar0, bar0 + bar0_size - 1);
	printk(" PCI BAR[1]: 0x%" PRIx32 " - 0x%" PRIx32 "\n",
		bar1, bar1 + bar1_size - 1);
	printk(" IRQ: %d\n\n\n", devinfo->irq);

	/* all neccessary space assigned to GR-701 target? */
	if ((bar0_size == 0) || (bar1_size == 0))
		return DRVMGR_ENORES;

	/* Initialize spin-lock for this PCI perihperal device. This is to
	 * protect the Interrupt Controller Registers. The genirq layer is
         * protecting its own internals and ISR dispatching.
         */
	SPIN_INIT(&priv->devlock, priv->prefix);

	priv->genirq = genirq_init(16);
	if ( priv->genirq == NULL ) {
		free(priv);
		dev->priv = NULL;
		return DRVMGR_FAIL;
	}

	if ( gr701_hw_init(priv) ) {
		genirq_destroy(priv->genirq);
		free(priv);
		dev->priv = NULL;
		printk(" Failed to initialize GR-701 HW\n");
		return DRVMGR_FAIL;
	}

	/* Init amba bus */
	priv->config.abus = &priv->abus;
	priv->config.ops = &ambapp_gr701_ops;
	priv->config.maps_up = &priv->bus_maps_up[0];
	priv->config.maps_down = &priv->bus_maps_down[0];
	if ( priv->dev->minor_drv < resources_cnt ) {
		priv->config.resources = gr701_resources[priv->dev->minor_drv];
	} else {
		priv->config.resources = NULL;
	}

	/* Create and register AMBA PnP bus. */
	return ambapp_bus_register(dev, &priv->config);
}

/* Called when a PCI target is found with the PCI device and vendor ID 
 * given in gr701_ids[].
 */
int gr701_init2(struct drvmgr_dev *dev)
{
	struct gr701_priv *priv = dev->priv;

	/* Clear any old interrupt requests */
	drvmgr_interrupt_clear(dev, 0);

	/* Enable System IRQ so that GR-701 PCI target interrupt goes through.
	 *
	 * It is important to enable it in stage init2. If interrupts were
	 * enabled in init1 this might hang the system when more than one PCI
	 * board is connected, this is because PCI interrupts might be shared
	 * and PCI target 2 have not initialized and might therefore drive
	 * interrupt already when entering init1().
	 */
	drvmgr_interrupt_register(dev, 0, "gr701", gr701_interrupt, priv);

	gr701_hw_init2(priv);

	return DRVMGR_OK;
}

int ambapp_gr701_int_register(
	struct drvmgr_dev *dev,
	int irq,
	const char *info,
	drvmgr_isr handler,
	void *arg)
{
	struct gr701_priv *priv = dev->parent->dev->priv;
	SPIN_IRQFLAGS(irqflags);
	int status;
	void *h;

	h = genirq_alloc_handler(handler, arg);
	if ( h == NULL )
		return DRVMGR_FAIL;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	status = genirq_register(priv->genirq, irq, h);
	if ( status == 0 ) {
		/* Clear IRQ for first registered handler */
		priv->pcib->iclear = (1<<irq);
	} else if ( status == 1 )
		status = 0;

	if (status != 0) {
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
		genirq_free_handler(h);
		return DRVMGR_FAIL;
	}

	status = genirq_enable(priv->genirq, irq, handler, arg);
	if ( status == 0 ) {
		/* Enable IRQ for first enabled handler only */
		priv->pcib->imask |= (1<<irq); /* unmask interrupt source */
	} else if ( status == 1 )
		status = DRVMGR_OK;

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return status;
}

int ambapp_gr701_int_unregister(
	struct drvmgr_dev *dev,
	int irq,
	drvmgr_isr isr,
	void *arg)
{
	struct gr701_priv *priv = dev->parent->dev->priv;
	SPIN_IRQFLAGS(irqflags);
	int status;
	void *handler;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	status = genirq_disable(priv->genirq, irq, isr, arg);
	if ( status == 0 ) {
		/* Disable IRQ only when no enabled handler exists */
		priv->pcib->imask &= ~(1<<irq); /* mask interrupt source */
	}

	handler = genirq_unregister(priv->genirq, irq, isr, arg);
	if ( handler == NULL )
		status = DRVMGR_FAIL;
	else
		status = DRVMGR_OK;

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	if (handler)
		genirq_free_handler(handler);

	return status;
}

int ambapp_gr701_int_unmask(
	struct drvmgr_dev *dev,
	int irq)
{
	struct gr701_priv *priv = dev->parent->dev->priv;
	SPIN_IRQFLAGS(irqflags);

	DBG("GR-701 IRQ %d: enable\n", irq);

	if ( genirq_check(priv->genirq, irq) )
		return DRVMGR_FAIL;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	/* Enable IRQ */
	priv->pcib->imask |= (1<<irq); /* unmask interrupt source */

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return DRVMGR_OK;
}

int ambapp_gr701_int_mask(
	struct drvmgr_dev *dev,
	int irq)
{
	struct gr701_priv *priv = dev->parent->dev->priv;
	SPIN_IRQFLAGS(irqflags);

	DBG("GR-701 IRQ %d: disable\n", irq);

	if ( genirq_check(priv->genirq, irq) )
		return DRVMGR_FAIL;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	/* Disable IRQ */
	priv->pcib->imask &= ~(1<<irq); /* mask interrupt source */

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return DRVMGR_OK;
}

int ambapp_gr701_int_clear(
	struct drvmgr_dev *dev,
	int irq)
{
	struct gr701_priv *priv = dev->parent->dev->priv;

	if ( genirq_check(priv->genirq, irq) )
		return DRVMGR_FAIL;

	priv->pcib->iclear = (1<<irq);

	return DRVMGR_OK;
}

int ambapp_gr701_get_params(struct drvmgr_dev *dev, struct drvmgr_bus_params *params)
{
	struct gr701_priv *priv = dev->parent->dev->priv;

	/* Device name prefix pointer, skip /dev */
	params->dev_prefix = &priv->prefix[5];

	return 0;
}

void gr701_print_dev(struct drvmgr_dev *dev, int options)
{
	struct gr701_priv *priv = dev->priv;
	struct pci_dev_info *devinfo = priv->devinfo;
	unsigned int freq_hz;
	uint32_t bar0, bar1, bar0_size, bar1_size;

	/* Print */
	printf("--- GR-701 [bus 0x%x, dev 0x%x, fun 0x%x] ---\n",
		PCI_DEV_EXPAND(priv->pcidev));
	bar0 = devinfo->resources[0].address;
	bar0_size = devinfo->resources[0].size;
	bar1 = devinfo->resources[1].address;
	bar1_size = devinfo->resources[1].size;

	printf(" PCI BAR[0]: 0x%" PRIx32 " - 0x%" PRIx32 "\n",
		bar0, bar0 + bar0_size - 1);
	printf(" PCI BAR[1]: 0x%" PRIx32 " - 0x%" PRIx32 "\n",
		bar1, bar1 + bar1_size - 1);
	printf(" IRQ:             %d\n", devinfo->irq);

	/* Frequency is the same as the PCI bus frequency */
	drvmgr_freq_get(dev, 0, &freq_hz);

	printf(" FREQ:            %u Hz\n", freq_hz);
	printf(" IMASK:           0x%08x\n", priv->pcib->imask);
	printf(" IPEND:           0x%08x\n", priv->pcib->ipend);

	/* Print amba config */
	if ( options & GR701_OPTIONS_AMBA ) {
		ambapp_print(&priv->abus, 10);
	}

#if 0
	/* Print IRQ handlers and their arguments */
	if ( options & GR701_OPTIONS_IRQ ) {
		int i;
		for(i=0; i<16; i++) {
			printf(" IRQ[%02d]:         0x%x, arg: 0x%x\n", 
				i, (unsigned int)priv->isrs[i].handler, (unsigned int)priv->isrs[i].arg);
		}
	}
#endif
}

void gr701_print(int options)
{
	struct pci_drv_info *drv = &gr701_info;
	struct drvmgr_dev *dev;

	dev = drv->general.dev;
	while(dev) {
		gr701_print_dev(dev, options);
		dev = dev->next_in_drv;
	}
}
