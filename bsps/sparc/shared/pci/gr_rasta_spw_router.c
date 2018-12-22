/*  GR-RASTA-SPW-ROUTER PCI Target driver.
 * 
 *  COPYRIGHT (c) 2011.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Configures the GR-RASTA-SPW-ROUTER interface PCI board.
 *  This driver provides a AMBA PnP bus by using the general part
 *  of the AMBA PnP bus driver (ambapp_bus.c). Based on the 
 *  GR-RASTA-IO driver.
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

#include <grlib/ambapp.h>
#include <grlib/grlib.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <drvmgr/pci_bus.h>
#include <grlib/bspcommon.h>
#include <grlib/genirq.h>
#include <grlib/gr_rasta_spw_router.h>

#include <grlib/grlib_impl.h>

/* Determines which PCI address the AHB masters will access, it should be
 * set so that the masters can access the CPU RAM. Default is base of CPU RAM,
 * CPU RAM is mapped 1:1 to PCI space.
 */
extern unsigned int _RAM_START;
#define AHBMST2PCIADR (((unsigned int)&_RAM_START) & 0xf0000000)

/* Offset from 0x80000000 (dual bus version) */
#define AHB1_BASE_ADDR 0x80000000
#define AHB1_IOAREA_BASE_ADDR 0x80100000

#define GRPCI2_BAR0_TO_AHB_MAP 0x04  /* Fixme */
#define GRPCI2_PCI_CONFIG      0x20  /* Fixme */

/* #define DEBUG 1 */

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...) 
#endif

/* PCI ID */
#define PCIID_VENDOR_GAISLER		0x1AC8

int gr_rasta_spw_router_init1(struct drvmgr_dev *dev);
int gr_rasta_spw_router_init2(struct drvmgr_dev *dev);
void gr_rasta_spw_router_isr(void *arg);

struct grpci2_regs {
	volatile unsigned int ctrl;
	volatile unsigned int statcap;
	volatile unsigned int pcimstprefetch;
	volatile unsigned int ahbtopciiomap;
	volatile unsigned int dmactrl;
	volatile unsigned int dmadesc;
	volatile unsigned int dmachanact;
	volatile unsigned int reserved;
	volatile unsigned int pcibartoahb[6];
	volatile unsigned int reserved2[2];
	volatile unsigned int ahbtopcimemmap[16];
	volatile unsigned int trcctrl;
	volatile unsigned int trccntmode;
	volatile unsigned int trcadpat;
	volatile unsigned int trcadmask;
	volatile unsigned int trcctrlsigpat;
	volatile unsigned int trcctrlsigmask;
	volatile unsigned int trcadstate;
	volatile unsigned int trcctrlsigstate;
};

struct gr_rasta_spw_router_ver {
	const unsigned int	amba_freq_hz;	/* The frequency */
	const unsigned int	amba_ioarea;	/* The address where the PnP IOAREA starts at */
};

/* Private data structure for driver */
struct gr_rasta_spw_router_priv {
	/* Driver management */
	struct drvmgr_dev	*dev;
	char			prefix[20];
	SPIN_DECLARE(devlock);

	/* PCI */
	pci_dev_t		pcidev;
	struct pci_dev_info	*devinfo;
	uint32_t		ahbmst2pci_map;

	/* IRQ */
	genirq_t		genirq;

	/* GR-RASTA-SPW-ROUTER */
	struct gr_rasta_spw_router_ver	*version;
	struct irqmp_regs	*irq;
	struct grpci2_regs	*grpci2;
	struct drvmgr_map_entry	bus_maps_up[2];
	struct drvmgr_map_entry	bus_maps_down[2];

	/* AMBA Plug&Play information on GR-RASTA-SPW-ROUTER */
	struct ambapp_bus	abus;
	struct ambapp_mmap	amba_maps[3];
	struct ambapp_config	config;
};

struct gr_rasta_spw_router_ver gr_rasta_spw_router_ver0 = {
	.amba_freq_hz		= 50000000,
	.amba_ioarea		= 0xfff00000,
};

int ambapp_rasta_spw_router_int_register(
	struct drvmgr_dev *dev,
	int irq,
	const char *info,
	drvmgr_isr handler,
	void *arg);
int ambapp_rasta_spw_router_int_unregister(
	struct drvmgr_dev *dev,
	int irq,
	drvmgr_isr handler,
	void *arg);
int ambapp_rasta_spw_router_int_unmask(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_rasta_spw_router_int_mask(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_rasta_spw_router_int_clear(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_rasta_spw_router_get_params(
	struct drvmgr_dev *dev,
	struct drvmgr_bus_params *params);

struct ambapp_ops ambapp_rasta_spw_router_ops = {
	.int_register = ambapp_rasta_spw_router_int_register,
	.int_unregister = ambapp_rasta_spw_router_int_unregister,
	.int_unmask = ambapp_rasta_spw_router_int_unmask,
	.int_mask = ambapp_rasta_spw_router_int_mask,
	.int_clear = ambapp_rasta_spw_router_int_clear,
	.get_params = ambapp_rasta_spw_router_get_params
};

struct drvmgr_drv_ops gr_rasta_spw_router_ops = 
{
	.init = {gr_rasta_spw_router_init1, gr_rasta_spw_router_init2, NULL, NULL},
	.remove = NULL,
	.info = NULL
};

struct pci_dev_id_match gr_rasta_spw_router_ids[] =
{
	PCIID_DEVVEND(PCIID_VENDOR_GAISLER, PCIID_DEVICE_GR_RASTA_SPW_RTR),
	PCIID_END_TABLE /* Mark end of table */
};

struct pci_drv_info gr_rasta_spw_router_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_PCI_GAISLER_RASTA_SPW_ROUTER_ID,	/* Driver ID */
		"GR-RASTA-SPW_ROUTER_DRV",	/* Driver Name */
		DRVMGR_BUS_TYPE_PCI,		/* Bus Type */
		&gr_rasta_spw_router_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct gr_rasta_spw_router_priv),
	},
	&gr_rasta_spw_router_ids[0]
};

/* Driver resources configuration for the AMBA bus on the GR-RASTA-SPW-ROUTER board.
 * It is declared weak so that the user may override it from the project file,
 * if the default settings are not enough.
 *
 * The configuration consists of an array of configuration pointers, each
 * pointer determine the configuration of one GR-RASTA-SPW-ROUTER board. Pointer
 * zero is for board0, pointer 1 for board1 and so on.
 *
 * The array must end with a NULL pointer.
 */
struct drvmgr_bus_res *gr_rasta_spw_router_resources[] __attribute__((weak)) =
{
	NULL
};

void gr_rasta_spw_router_register_drv(void)
{
	DBG("Registering GR-RASTA-SPW-ROUTER PCI driver\n");
	drvmgr_drv_register(&gr_rasta_spw_router_info.general);
}

void gr_rasta_spw_router_isr(void *arg)
{
	struct gr_rasta_spw_router_priv *priv = arg;
	unsigned int status, tmp;
	int irq;
	SPIN_ISR_IRQFLAGS(irqflags);

	tmp = status = priv->irq->ipend;

	/* DBG("GR-RASTA-SPW-ROUTER: IRQ 0x%x\n",status); */

	SPIN_LOCK(&priv->devlock, irqflags);
	for(irq=0; irq<16; irq++) {
		if ( status & (1<<irq) ) {
			genirq_doirq(priv->genirq, irq);
			priv->irq->iclear = (1<<irq);
			status &= ~(1<<irq);
			if ( status == 0 )
				break;
		}
	}
	SPIN_UNLOCK(&priv->devlock, irqflags);

	/* ACK interrupt, this is because PCI is Level, so the IRQ Controller
	 * still drives the IRQ
	 */
	if ( tmp ) 
		drvmgr_interrupt_clear(priv->dev, 0);

	DBG("RASTA-SPW_ROUTER-IRQ: 0x%x\n", tmp);
}

static int gr_rasta_spw_router_hw_init(struct gr_rasta_spw_router_priv *priv)
{
	int i;
	uint32_t data;
	unsigned int ctrl;
	uint8_t tmp2;
	struct ambapp_dev *tmp;
	struct ambapp_ahb_info *ahb;
	uint8_t cap_ptr;
	pci_dev_t pcidev = priv->pcidev;
	struct pci_dev_info *devinfo = priv->devinfo;

	/* Select version of GR-RASTA-SPW-ROUTER board. Currently only one
	 * version
	 */
	switch (devinfo->rev) {
		case 0:
			priv->version = &gr_rasta_spw_router_ver0;
			break;
		default:
			return -2;
	}

	/* Check capabilities list bit */
	pci_cfg_r8(pcidev, PCIR_STATUS, &tmp2);

	if (!((tmp2 >> 4) & 1)) {
		/* Capabilities list not available which it should be in the GRPCI2 */
		return -3;
	}

	/* Read capabilities pointer */
	pci_cfg_r8(pcidev, PCIR_CAP_PTR, &cap_ptr);

	/* Set AHB address mappings for target PCI bars */
	pci_cfg_w32(pcidev, cap_ptr+GRPCI2_BAR0_TO_AHB_MAP, 0xffe00000);  /* APB bus, AHB I/O bus 2 MB */ 

	/* Set PCI bus to be big endian */
	pci_cfg_r32(pcidev, cap_ptr+GRPCI2_PCI_CONFIG, &data);
	data = data & 0xFFFFFFFE;
	pci_cfg_w32(pcidev, cap_ptr+GRPCI2_PCI_CONFIG, data);

#if 0
	/* set parity error response */
	pci_cfg_r32(pcidev, PCIR_COMMAND, &data);
	pci_cfg_w32(pcidev, PCIR_COMMAND, (data|PCIM_CMD_PERRESPEN));
#endif

	/* Scan AMBA Plug&Play */

	/* AMBA MAP bar0 (in router) ==> 0xffe00000(remote amba address) */
	priv->amba_maps[0].size = devinfo->resources[0].size;
	priv->amba_maps[0].local_adr = devinfo->resources[0].address;
	priv->amba_maps[0].remote_adr = 0xffe00000;

	/* Addresses not matching with map be untouched */
	priv->amba_maps[1].size = 0xfffffff0;
	priv->amba_maps[1].local_adr = 0;
	priv->amba_maps[1].remote_adr = 0;

	/* Mark end of table */
	priv->amba_maps[2].size=0;

	/* Start AMBA PnP scan at first AHB bus */
	ambapp_scan(
		&priv->abus,
		devinfo->resources[0].address + 0x100000,
		NULL,
		&priv->amba_maps[0]);

	/* Initialize Frequency of AMBA bus */
	ambapp_freq_init(&priv->abus, NULL, priv->version->amba_freq_hz);

	/* Find IRQ controller, Clear all current IRQs */
	tmp = (struct ambapp_dev *)ambapp_for_each(&priv->abus,
				(OPTIONS_ALL|OPTIONS_APB_SLVS),
				VENDOR_GAISLER, GAISLER_IRQMP,
				ambapp_find_by_idx, NULL);
	if ( !tmp ) {
		return -4;
	}
	priv->irq = (struct irqmp_regs *)DEV_TO_APB(tmp)->start;
	/* Set up GR-RASTA-SPW-ROUTER irq controller */
	priv->irq->mask[0] = 0;
	priv->irq->iclear = 0xffff;
	priv->irq->ilevel = 0;

	priv->bus_maps_down[0].name = "PCI BAR0 -> AMBA";
	priv->bus_maps_down[0].size = priv->amba_maps[0].size;
	priv->bus_maps_down[0].from_adr = (void *)priv->amba_maps[0].local_adr;
	priv->bus_maps_down[0].to_adr = (void *)priv->amba_maps[0].remote_adr;
	priv->bus_maps_down[1].size = 0;

	/* Find GRPCI2 controller AHB Slave interface */
	tmp = (struct ambapp_dev *)ambapp_for_each(&priv->abus,
					(OPTIONS_ALL|OPTIONS_AHB_SLVS),
					VENDOR_GAISLER, GAISLER_GRPCI2,
					ambapp_find_by_idx, NULL);
	if ( !tmp ) {
		return -5;
	}
	ahb = (struct ambapp_ahb_info *)tmp->devinfo;
	priv->bus_maps_up[0].name = "AMBA GRPCI2 Window";
	priv->bus_maps_up[0].size = ahb->mask[0]; /* AMBA->PCI Window on GR-RASTA-SPW-ROUTER board */
	priv->bus_maps_up[0].from_adr = (void *)ahb->start[0];
	priv->bus_maps_up[0].to_adr = (void *)
				(priv->ahbmst2pci_map & ~(ahb->mask[0]-1));
	priv->bus_maps_up[1].size = 0;

	/* Find GRPCI2 controller APB Slave interface */
	tmp = (struct ambapp_dev *)ambapp_for_each(&priv->abus,
					(OPTIONS_ALL|OPTIONS_APB_SLVS),
					VENDOR_GAISLER, GAISLER_GRPCI2,
					ambapp_find_by_idx, NULL);
	if ( !tmp ) {
		return -6;
	}
	priv->grpci2 = (struct grpci2_regs *)
		((struct ambapp_apb_info *)tmp->devinfo)->start;

	/* Set AHB to PCI mapping for all AMBA AHB masters */
	for(i = 0; i < 16; i++) {
		priv->grpci2->ahbtopcimemmap[i] = priv->ahbmst2pci_map &
							~(ahb->mask[0]-1);
	}

	/* Make sure dirq(0) sampling is enabled */
	ctrl = priv->grpci2->ctrl;
	ctrl = (ctrl & 0xFFFFFF0F) | (1 << 4);
	priv->grpci2->ctrl = ctrl;

	/* Successfully registered the RASTA-SPW-ROUTER board */
	return 0;
}

static int gr_rasta_spw_router_hw_init2(struct gr_rasta_spw_router_priv *priv)
{
	/* Enable DMA by enabling PCI target as master */
	pci_master_enable(priv->pcidev);

	return DRVMGR_OK;
}

/* Called when a PCI target is found with the PCI device and vendor ID 
 * given in gr_rasta_spw_router_ids[].
 */
int gr_rasta_spw_router_init1(struct drvmgr_dev *dev)
{
	struct gr_rasta_spw_router_priv *priv;
	struct pci_dev_info *devinfo;
	int status;
	uint32_t bar0, bar0_size;
	union drvmgr_key_value *value;
	int resources_cnt;

	priv = dev->priv;
	if (!priv)
		return DRVMGR_NOMEM;

	memset(priv, 0, sizeof(*priv));
	dev->priv = priv;
	priv->dev = dev;

	/* Determine number of configurations */
	resources_cnt = get_resarray_count(gr_rasta_spw_router_resources);

	/* Generate Device prefix */

	strcpy(priv->prefix, "/dev/spwrouter0");
	priv->prefix[14] += dev->minor_drv;
	mkdir(priv->prefix, S_IRWXU | S_IRWXG | S_IRWXO);
	priv->prefix[15] = '/';
	priv->prefix[16] = '\0';

	priv->devinfo = devinfo = (struct pci_dev_info *)dev->businfo;
	priv->pcidev = devinfo->pcidev;
	bar0 = devinfo->resources[0].address;
	bar0_size = devinfo->resources[0].size;
	printk("\n\n--- GR-RASTA-SPW-ROUTER[%d] ---\n", dev->minor_drv);
	printk(" PCI BUS: 0x%x, SLOT: 0x%x, FUNCTION: 0x%x\n",
		PCI_DEV_EXPAND(priv->pcidev));
	printk(" PCI VENDOR: 0x%04x, DEVICE: 0x%04x\n",
		devinfo->id.vendor, devinfo->id.device);
	printk(" PCI BAR[0]: 0x%08" PRIx32 " - 0x%08" PRIx32 "\n",
		bar0, bar0 + bar0_size - 1);
	printk(" IRQ: %d\n\n\n", devinfo->irq);

	/* all neccessary space assigned to GR-RASTA-SPW-ROUTER target? */
	if (bar0_size == 0)
		return DRVMGR_ENORES;

	/* Initialize spin-lock for this PCI peripheral device. This is to
	 * protect the Interrupt Controller Registers. The genirq layer is
         * protecting its own internals and ISR dispatching.
         */
	SPIN_INIT(&priv->devlock, priv->prefix);

	/* Let user override which PCI address the AHB masters of the
	 * GR-RASTA-SPW board access when doing DMA to CPU RAM. The AHB masters
	 * access the PCI Window of the AMBA bus, the MSB 4-bits of that address
	 * is translated according this config option before the address
	 * goes out on the PCI bus.
	 * Only the 4 MSB bits have an effect;
	 */
	value = drvmgr_dev_key_get(priv->dev, "ahbmst2pci", DRVMGR_KT_INT);
	if (value)
		priv->ahbmst2pci_map = value->i;
	else
		priv->ahbmst2pci_map = AHBMST2PCIADR; /* default */	

	priv->genirq = genirq_init(16);
	if ( priv->genirq == NULL )
		return DRVMGR_FAIL;

	if ((status = gr_rasta_spw_router_hw_init(priv)) != 0) {
		genirq_destroy(priv->genirq);
		printk(" Failed to initialize GR-RASTA-SPW-ROUTER HW: %d\n", status);
		return DRVMGR_FAIL;
	}

	/* Init amba bus */
	priv->config.abus = &priv->abus;
	priv->config.ops = &ambapp_rasta_spw_router_ops;
	priv->config.maps_up = &priv->bus_maps_up[0];
	priv->config.maps_down = &priv->bus_maps_down[0];
	if ( priv->dev->minor_drv < resources_cnt ) {
		priv->config.resources = gr_rasta_spw_router_resources[priv->dev->minor_drv];
	} else {
		priv->config.resources = NULL;
	}

	/* Create and register AMBA PnP bus. */
	return ambapp_bus_register(dev, &priv->config);
}

int gr_rasta_spw_router_init2(struct drvmgr_dev *dev)
{
	struct gr_rasta_spw_router_priv *priv = dev->priv;

	/* Clear any old interrupt requests */
	drvmgr_interrupt_clear(dev, 0);

	/* Enable System IRQ so that GR-RASTA-SPW-ROUTER PCI target interrupt
	 * goes through.
	 *
	 * It is important to enable it in stage init2. If interrupts were
	 * enabled in init1 this might hang the system when more than one
	 * PCI board is connected, this is because PCI interrupts might
	 * be shared and PCI board 2 have not initialized and
	 * might therefore drive interrupt already when entering init1().
	 */
	drvmgr_interrupt_register(
		dev,
		0,
		"gr_rasta_spw_router",
		gr_rasta_spw_router_isr,
		(void *)priv);

	return gr_rasta_spw_router_hw_init2(priv);
}

int ambapp_rasta_spw_router_int_register(
	struct drvmgr_dev *dev,
	int irq,
	const char *info,
	drvmgr_isr handler,
	void *arg)
{
	struct gr_rasta_spw_router_priv *priv = dev->parent->dev->priv;
	SPIN_IRQFLAGS(irqflags);
	int status;
	void *h;

	h = genirq_alloc_handler(handler, arg);
	if ( h == NULL )
		return DRVMGR_FAIL;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	status = genirq_register(priv->genirq, irq, h);
	if (status == 0) {
		/* Clear IRQ for first registered handler */
		priv->irq->iclear = (1<<irq);
	} else if (status == 1)
		status = 0;

	if (status != 0) {
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
		genirq_free_handler(h);
		return DRVMGR_FAIL;
	}

	status = genirq_enable(priv->genirq, irq, handler, arg);
	if ( status == 0 ) {
		/* Enable IRQ for first enabled handler only */
		priv->irq->mask[0] |= (1<<irq); /* unmask interrupt source */
	} else if ( status == 1 )
		status = 0;

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return status;
}

int ambapp_rasta_spw_router_int_unregister(
	struct drvmgr_dev *dev,
	int irq,
	drvmgr_isr isr,
	void *arg)
{
	struct gr_rasta_spw_router_priv *priv = dev->parent->dev->priv;
	SPIN_IRQFLAGS(irqflags);
	int status;
	void *handler;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	status = genirq_disable(priv->genirq, irq, isr, arg);
	if ( status == 0 ) {
		/* Disable IRQ only when no enabled handler exists */
		priv->irq->mask[0] &= ~(1<<irq); /* mask interrupt source */
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

int ambapp_rasta_spw_router_int_unmask(
	struct drvmgr_dev *dev,
	int irq)
{
	struct gr_rasta_spw_router_priv *priv = dev->parent->dev->priv;
	SPIN_IRQFLAGS(irqflags);

	DBG("RASTA-SPW-ROUTER IRQ %d: unmask\n", irq);

	if ( genirq_check(priv->genirq, irq) )
		return DRVMGR_EINVAL;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	/* Enable IRQ for first enabled handler only */
	priv->irq->mask[0] |= (1<<irq); /* unmask interrupt source */

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return DRVMGR_OK;
}

int ambapp_rasta_spw_router_int_mask(
	struct drvmgr_dev *dev,
	int irq)
{
	struct gr_rasta_spw_router_priv *priv = dev->parent->dev->priv;
	SPIN_IRQFLAGS(irqflags);

	DBG("RASTA-SPW-ROUTER IRQ %d: mask\n", irq);

	if ( genirq_check(priv->genirq, irq) )
		return DRVMGR_EINVAL;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	/* Disable/mask IRQ */
	priv->irq->mask[0] &= ~(1<<irq); /* mask interrupt source */

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return DRVMGR_OK;
}

int ambapp_rasta_spw_router_int_clear(
	struct drvmgr_dev *dev,
	int irq)
{
	struct gr_rasta_spw_router_priv *priv = dev->parent->dev->priv;

	if ( genirq_check(priv->genirq, irq) )
		return DRVMGR_EINVAL;

	priv->irq->iclear = (1<<irq);

	return DRVMGR_OK;
}

int ambapp_rasta_spw_router_get_params(struct drvmgr_dev *dev, struct drvmgr_bus_params *params)
{
	struct gr_rasta_spw_router_priv *priv = dev->parent->dev->priv;

	/* Device name prefix pointer, skip /dev */
	params->dev_prefix = &priv->prefix[5];

	return 0;
}

void gr_rasta_spw_router_print_dev(struct drvmgr_dev *dev, int options)
{
	struct gr_rasta_spw_router_priv *priv = dev->priv;
	struct pci_dev_info *devinfo = priv->devinfo;
	uint32_t bar0, bar0_size;

	/* Print */
	printf("--- GR-RASTA-SPW-ROUTER [bus 0x%x, dev 0x%x, fun 0x%x] ---\n",
		PCI_DEV_EXPAND(priv->pcidev));

	bar0 = devinfo->resources[0].address;
	bar0_size = devinfo->resources[0].size;
	printf(" PCI BAR[0]: 0x%" PRIx32 " - 0x%" PRIx32 "\n",
		bar0, bar0 + bar0_size - 1);
	printf(" IRQ REGS:        0x%" PRIxPTR "\n", (uintptr_t)priv->irq);
	printf(" IRQ:             %d\n", devinfo->irq);
	printf(" PCI REVISION:    %d\n", devinfo->rev);
	printf(" FREQ:            %d Hz\n", priv->version->amba_freq_hz);
	printf(" IMASK:           0x%08x\n", priv->irq->mask[0]);
	printf(" IPEND:           0x%08x\n", priv->irq->ipend);

	/* Print amba config */
	if (options & RASTA_SPW_ROUTER_OPTIONS_AMBA)
		ambapp_print(&priv->abus, 10);

#if 0
	/* Print IRQ handlers and their arguments */
	if (options & RASTA_SPW_ROUTER_OPTIONS_IRQ) {
		int i;
		for(i = 0; i < 16; i++) {
			printf(" IRQ[%02d]:         0x%x, arg: 0x%x\n", 
				i, (unsigned int)priv->isrs[i].handler,
				(unsigned int)priv->isrs[i].arg);
		}
	}
#endif
}

void gr_rasta_spw_router_print(int options)
{
	struct pci_drv_info *drv = &gr_rasta_spw_router_info;
	struct drvmgr_dev *dev;

	dev = drv->general.dev;
	while(dev) {
		gr_rasta_spw_router_print_dev(dev, options);
		dev = dev->next_in_drv;
	}
}
