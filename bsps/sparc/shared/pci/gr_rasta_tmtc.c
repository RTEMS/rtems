/*  GR-RASTA-TMTC PCI Target driver.
 * 
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  Configures the GR-RASTA-TMTC interface PCI board.
 *  This driver provides a AMBA PnP bus by using the general part
 *  of the AMBA PnP bus driver (ambapp_bus.c).
 *
 *  Driver resources for the AMBA PnP bus provided can be set by overriding
 *  the defaults by declaring gr_rasta_tmtc_resources[].
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

#include <grlib/ambapp.h>
#include <grlib/grlib.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>
#include <drvmgr/pci_bus.h>
#include <grlib/bspcommon.h>
#include <grlib/genirq.h>

#include <grlib/gr_rasta_tmtc.h>

#include <grlib/grlib_impl.h>

/* Determines which PCI address the AHB masters will access, it should be
 * set so that the masters can access the CPU RAM. Default is base of CPU RAM,
 * CPU RAM is mapped 1:1 to PCI space.
 */
extern unsigned int _RAM_START;
#define AHBMST2PCIADR (((unsigned int)&_RAM_START) & 0xf0000000)

#define GAISLER_GPIO         0x01a
#define AHB1_BASE_ADDR 0x80000000
#define AHB1_IOAREA_BASE_ADDR 0x80200000
#define AHB1_IOAREA_OFS (AHB1_IOAREA_BASE_ADDR - AHB1_BASE_ADDR)

/* Second revision constants (GRPCI2) */
#define GRPCI2_BAR0_TO_AHB_MAP 0x04  /* Fixme */
#define GRPCI2_BAR1_TO_AHB_MAP 0x08  /* Fixme */
#define GRPCI2_PCI_CONFIG      0x20  /* Fixme */

/* #define DEBUG 1 */

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...) 
#endif

int gr_rasta_tmtc_init1(struct drvmgr_dev *dev);
int gr_rasta_tmtc_init2(struct drvmgr_dev *dev);
void gr_rasta_tmtc_isr (void *arg);

struct grpci_regs {
	volatile unsigned int cfg_stat;
	volatile unsigned int bar0;
	volatile unsigned int page0;
	volatile unsigned int bar1;
	volatile unsigned int page1;
	volatile unsigned int iomap;
	volatile unsigned int stat_cmd;
};

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

struct gr_rasta_tmtc_ver {
	const unsigned int	amba_freq_hz;	/* The frequency */
	const unsigned int	amba_ioarea;	/* The address where the PnP IOAREA starts at */
};

/* Private data structure for driver */
struct gr_rasta_tmtc_priv {
	/* Driver management */
	struct drvmgr_dev		*dev;
	char				prefix[20];
	SPIN_DECLARE(devlock);

	/* PCI */
	pci_dev_t			pcidev;
	struct pci_dev_info		*devinfo;
	uint32_t			ahbmst2pci_map;

	/* IRQ */
	genirq_t			genirq;

	/* GR-RASTA-TMTC */
	struct gr_rasta_tmtc_ver	*version;
	struct irqmp_regs		*irq;
	struct grpci_regs		*grpci;
	struct grpci2_regs		*grpci2;
	struct grgpio_regs		*gpio;
	struct drvmgr_map_entry		bus_maps_down[3];
	struct drvmgr_map_entry		bus_maps_up[2];

	/* AMBA Plug&Play information on GR-RASTA-TMTC */
	struct ambapp_bus		abus;
	struct ambapp_mmap		amba_maps[4];
        struct ambapp_config		config;
};

struct gr_rasta_tmtc_ver gr_rasta_tmtc_ver0 = {
	.amba_freq_hz		= 30000000,
	.amba_ioarea		= AHB1_IOAREA_BASE_ADDR,
};

int ambapp_rasta_tmtc_int_register(
	struct drvmgr_dev *dev,
	int irq,
	const char *info,
	drvmgr_isr handler,
	void *arg);
int ambapp_rasta_tmtc_int_unregister(
	struct drvmgr_dev *dev,
	int irq,
	drvmgr_isr handler,
	void *arg);
int ambapp_rasta_tmtc_int_unmask(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_rasta_tmtc_int_mask(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_rasta_tmtc_int_clear(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_rasta_tmtc_get_params(
	struct drvmgr_dev *dev,
	struct drvmgr_bus_params *params);

struct ambapp_ops ambapp_rasta_tmtc_ops = {
	.int_register = ambapp_rasta_tmtc_int_register,
	.int_unregister = ambapp_rasta_tmtc_int_unregister,
	.int_unmask = ambapp_rasta_tmtc_int_unmask,
	.int_mask = ambapp_rasta_tmtc_int_mask,
	.int_clear = ambapp_rasta_tmtc_int_clear,
	.get_params = ambapp_rasta_tmtc_get_params
};

struct drvmgr_drv_ops gr_rasta_tmtc_ops = 
{
	.init = {gr_rasta_tmtc_init1, gr_rasta_tmtc_init2, NULL, NULL},
	.remove = NULL,
	.info = NULL,
};

struct pci_dev_id_match gr_rasta_tmtc_ids[] =
{
	PCIID_DEVVEND(PCIID_VENDOR_GAISLER, PCIID_DEVICE_GR_RASTA_TMTC),
	PCIID_END_TABLE /* Mark end of table */
};

struct pci_drv_info gr_rasta_tmtc_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_PCI_GAISLER_RASTATMTC_ID,/* Driver ID */
		"GR-RASTA-TMTC_DRV",		/* Driver Name */
		DRVMGR_BUS_TYPE_PCI,		/* Bus Type */
		&gr_rasta_tmtc_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct gr_rasta_tmtc_priv) /* Let drvmgr alloc private */
	},
	&gr_rasta_tmtc_ids[0]
};

/* Driver resources configuration for the AMBA bus on the GR-RASTA-TMTC board.
 * It is declared weak so that the user may override it from the project file,
 * if the default settings are not enough.
 *
 * The configuration consists of an array of configuration pointers, each
 * pointer determine the configuration of one GR-RASTA-TMTC board. Pointer
 * zero is for board0, pointer 1 for board1 and so on.
 *
 * The array must end with a NULL pointer.
 */
struct drvmgr_bus_res *gr_rasta_tmtc_resources[] __attribute__((weak)) =
{
	NULL,
};

void gr_rasta_tmtc_register_drv(void)
{
	DBG("Registering GR-RASTA-TMTC PCI driver\n");
	drvmgr_drv_register(&gr_rasta_tmtc_info.general);
}

void gr_rasta_tmtc_isr (void *arg)
{
	struct gr_rasta_tmtc_priv *priv = arg;
	unsigned int status, tmp;
	int irq;
	SPIN_ISR_IRQFLAGS(irqflags);

	tmp = status = priv->irq->ipend;

	/* printk("GR-RASTA-TMTC: IRQ 0x%x\n",status); */

	SPIN_LOCK(&priv->devlock, irqflags);
	for(irq=0; irq<32; irq++) {
		if ( status & (1<<irq) ) {
			genirq_doirq(priv->genirq, irq);
			priv->irq->iclear = (1<<irq);
			status &= ~(1<<irq);
			if ( status == 0 )
				break;
		}
	}
	SPIN_UNLOCK(&priv->devlock, irqflags);

	/* ACK interrupt, this is because PCI is Level, so the IRQ Controller still drives the IRQ. */
	if ( tmp )
		drvmgr_interrupt_clear(priv->dev, 0);

	DBG("RASTA-TMTC-IRQ: 0x%x\n", tmp);
}

/* Init AMBA bus frequency, IRQ controller, GPIO register, bus maps and other
 * common stuff between rev0 and rev1.
 */
static int gr_rasta_tmtc_hw_init_common(struct gr_rasta_tmtc_priv *priv)
{
	struct ambapp_dev *tmp;
	unsigned int pci_freq_hz;

	/* Initialize Frequency of AMBA bus. The AMBA bus runs at same
	 * frequency as PCI bus
	 */
	drvmgr_freq_get(priv->dev, 0, &pci_freq_hz);
	ambapp_freq_init(&priv->abus, NULL, pci_freq_hz);

	/* Find IRQ controller, Clear all current IRQs */
	tmp = (struct ambapp_dev *)ambapp_for_each(&priv->abus,
					(OPTIONS_ALL|OPTIONS_APB_SLVS),
					VENDOR_GAISLER, GAISLER_IRQMP,
					ambapp_find_by_idx, NULL);
	if ( !tmp ) {
		return -4;
	}
	priv->irq = (struct irqmp_regs *)DEV_TO_APB(tmp)->start;
	/* Set up GR-RASTA-TMTC irq controller */
	priv->irq->mask[0] = 0;
	priv->irq->iclear = 0xffffffff;
	priv->irq->ilevel = 0;

	/* Find First GPIO controller */
	tmp = (struct ambapp_dev *)ambapp_for_each(&priv->abus,
					(OPTIONS_ALL|OPTIONS_APB_SLVS),
					VENDOR_GAISLER, GAISLER_GPIO,
					ambapp_find_by_idx, NULL);
	if ( !tmp ) {
		return -5;
	}
	priv->gpio = (struct grgpio_regs *) (((struct ambapp_apb_info *)tmp->devinfo)->start);
	/* Clear GR-RASTA-TMTC GPIO controller */
	priv->gpio->imask = 0;
	priv->gpio->ipol = 0;
	priv->gpio->iedge = 0;
	priv->gpio->bypass = 0;
	/* Set up GR-RASTA-TMTC GPIO controller to select GRTM and GRTC */
	priv->gpio->output = (GR_TMTC_GPIO_GRTM_SEL|GR_TMTC_GPIO_TRANSP_CLK) | (GR_TMTC_GPIO_TC_BIT_LOCK|GR_TMTC_GPIO_TC_RF_AVAIL|GR_TMTC_GPIO_TC_ACTIVE_HIGH|GR_TMTC_GPIO_TC_RISING_CLK);
	priv->gpio->dir = 0xffffffff;
	DBG("GR-TMTC GPIO: 0x%x\n", (unsigned int)priv->gpio);

	/* DOWN streams translation table */
	priv->bus_maps_down[0].name = "PCI BAR0 -> AMBA";
	priv->bus_maps_down[0].size = priv->amba_maps[0].size;
	priv->bus_maps_down[0].from_adr = (void *)priv->amba_maps[0].local_adr;
	priv->bus_maps_down[0].to_adr = (void *)priv->amba_maps[0].remote_adr;

	priv->bus_maps_down[1].name = "PCI BAR1 -> AMBA";
	priv->bus_maps_down[1].size = priv->amba_maps[1].size;
	priv->bus_maps_down[1].from_adr = (void *)priv->amba_maps[1].local_adr;
	priv->bus_maps_down[1].to_adr = (void *)priv->amba_maps[1].remote_adr;

	/* Mark end of translation table */
	priv->bus_maps_down[2].size = 0;

	return 0;
}

/* PCI Hardware (Revision 0) initialization */
static int gr_rasta_tmtc0_hw_init(struct gr_rasta_tmtc_priv *priv)
{
	unsigned int *page0 = NULL;
	struct ambapp_dev *tmp;
	struct ambapp_ahb_info *ahb;
	int status;
	pci_dev_t pcidev = priv->pcidev;
	struct pci_dev_info *devinfo = priv->devinfo;
	uint32_t bar0, bar0_size;

	/* Select version of GR-RASTA-TMTC board */
	switch (devinfo->rev) {
		case 0:
			priv->version = &gr_rasta_tmtc_ver0;
			break;
		default:
			return -2;
	}

	bar0 = devinfo->resources[0].address;
	bar0_size = devinfo->resources[0].size;
	page0 = (unsigned int *)(bar0 + bar0_size/2); 

	/* Point PAGE0 to start of Plug and Play information */
	*page0 = priv->version->amba_ioarea & 0xf0000000;

#if 0
	{
		uint32_t data;
		/* set parity error response */
		pci_cfg_r32(pcidev, PCIR_COMMAND, &data);
		pci_cfg_w32(pcidev, PCIR_COMMAND, (data|PCIM_CMD_PERRESPEN));
	}
#endif

	/* Setup cache line size. Default cache line size will result in
	 * poor performance (256 word fetches), 0xff will set it according
	 * to the max size of the PCI FIFO.
	 */
	pci_cfg_w8(pcidev, PCIR_CACHELNSZ, 0xff);

	/* Scan AMBA Plug&Play */

	/* AMBA MAP bar0 (in CPU) ==> 0x80000000(remote amba address) */
	priv->amba_maps[0].size = 0x10000000;
	priv->amba_maps[0].local_adr = bar0;
	priv->amba_maps[0].remote_adr = AHB1_BASE_ADDR;

	/* AMBA MAP bar1 (in CPU) ==> 0x40000000(remote amba address) */
	priv->amba_maps[1].size = devinfo->resources[1].size;
	priv->amba_maps[1].local_adr = devinfo->resources[1].address;
	priv->amba_maps[1].remote_adr = 0x40000000;

	/* Addresses not matching with map be untouched */
	priv->amba_maps[2].size = 0xfffffff0;
	priv->amba_maps[2].local_adr = 0;
	priv->amba_maps[2].remote_adr = 0;

	/* Mark end of table */
	priv->amba_maps[3].size=0;
	priv->amba_maps[3].local_adr = 0;
	priv->amba_maps[3].remote_adr = 0;

	/* Start AMBA PnP scan at first AHB bus */
	ambapp_scan(&priv->abus,
		bar0 + (priv->version->amba_ioarea & ~0xf0000000),
		NULL, &priv->amba_maps[0]);

	/* Point PAGE0 to start of APB area */
	*page0 = AHB1_BASE_ADDR;

	/* Find GRPCI controller */
	tmp = (struct ambapp_dev *)ambapp_for_each(&priv->abus,
					(OPTIONS_ALL|OPTIONS_APB_SLVS),
					VENDOR_GAISLER, GAISLER_PCIFBRG,
					ambapp_find_by_idx, NULL);
	if ( !tmp ) {
		return -3;
	}
	priv->grpci = (struct grpci_regs *)((struct ambapp_apb_info *)tmp->devinfo)->start;

	/* Set GRPCI mmap so that AMBA masters can access CPU-RAM over
	 * the PCI window.
	 */
	priv->grpci->cfg_stat = (priv->grpci->cfg_stat & 0x0fffffff) |
				(priv->ahbmst2pci_map & 0xf0000000);
	priv->grpci->page1 = 0x40000000;

	/* init AMBA bus, IRQCtrl, GPIO, bus down-maps */
	status = gr_rasta_tmtc_hw_init_common(priv);
	if (status)
		return status;

	/* Find GRPCI controller AHB Slave interface */
	tmp = (struct ambapp_dev *)ambapp_for_each(&priv->abus,
				(OPTIONS_ALL|OPTIONS_AHB_SLVS),
				VENDOR_GAISLER, GAISLER_PCIFBRG,
				ambapp_find_by_idx, NULL);
	if ( !tmp ) {
		return -6;
	}
	ahb = (struct ambapp_ahb_info *)tmp->devinfo;

	/* UP streams translation table */
	priv->bus_maps_up[0].name = "AMBA GRPCI Window";
	priv->bus_maps_up[0].size = ahb->mask[0]; /* AMBA->PCI Window on GR-RASTA-TMTC board */
	priv->bus_maps_up[0].from_adr = (void *)ahb->start[0];
	priv->bus_maps_up[0].to_adr = (void *)
					(priv->ahbmst2pci_map & 0xf0000000);

	/* Mark end of translation table */
	priv->bus_maps_up[1].size = 0;

	/* Successfully registered the RASTA board */
	return 0;
}

/* PCI Hardware (Revision 1) initialization */
static int gr_rasta_tmtc1_hw_init(struct gr_rasta_tmtc_priv *priv)
{
	int i;
	uint32_t data;
	unsigned int ctrl;
	uint8_t tmp2;
	struct ambapp_dev *tmp;
	int status;
	struct ambapp_ahb_info *ahb;
	uint8_t cap_ptr;
	pci_dev_t pcidev = priv->pcidev;
	struct pci_dev_info *devinfo = priv->devinfo;

	/* Check capabilities list bit */
	pci_cfg_r8(pcidev, PCIR_STATUS, &tmp2);

	if (!((tmp2 >> 4) & 1)) {
		/* Capabilities list not available which it should be in the
		 * GRPCI2
		 */
		return -3;
	}

	/* Read capabilities pointer */
	pci_cfg_r8(pcidev, PCIR_CAP_PTR, &cap_ptr);

	/* Set AHB address mappings for target PCI bars
	 * BAR0: 16MB  : Mapped to I/O at 0x80000000
	 * BAR1: 256MB : Mapped to MEM at 0x40000000
	 */
	pci_cfg_w32(pcidev, cap_ptr+GRPCI2_BAR0_TO_AHB_MAP, AHB1_BASE_ADDR);
	pci_cfg_w32(pcidev, cap_ptr+GRPCI2_BAR1_TO_AHB_MAP, 0x40000000);

	/* Set PCI bus to be same endianess as PCI system */
	pci_cfg_r32(pcidev, cap_ptr+GRPCI2_PCI_CONFIG, &data);
	if (pci_endian == PCI_BIG_ENDIAN)
		data = data & 0xFFFFFFFE;
	else
		data = data | 0x00000001;
	pci_cfg_w32(pcidev, cap_ptr+GRPCI2_PCI_CONFIG, data);

#if 0
	/* set parity error response */
	pci_cfg_r32(pcidev, PCIR_COMMAND, &data);
	pci_cfg_w32(pcidev, PCIR_COMMAND, (data|PCIM_CMD_PERRESPEN));
#endif

	/* Scan AMBA Plug&Play */

	/* AMBA MAP bar0 (in PCI) ==> 0x40000000 (remote amba address) */
	priv->amba_maps[0].size = devinfo->resources[0].size;
	priv->amba_maps[0].local_adr = devinfo->resources[0].address;
	priv->amba_maps[0].remote_adr = AHB1_BASE_ADDR;

	/* AMBA MAP bar0 (in PCI) ==> 0x80000000 (remote amba address) */
	priv->amba_maps[1].size = devinfo->resources[1].size;
	priv->amba_maps[1].local_adr = devinfo->resources[1].address;
	priv->amba_maps[1].remote_adr = 0x40000000;

	/* Addresses not matching with map be untouched */
	priv->amba_maps[2].size = 0xfffffff0;
	priv->amba_maps[2].local_adr = 0;
	priv->amba_maps[2].remote_adr = 0;

	/* Mark end of table */
	priv->amba_maps[3].size=0;

	/* Start AMBA PnP scan at first AHB bus */
	ambapp_scan(
		&priv->abus,
		devinfo->resources[0].address + AHB1_IOAREA_OFS,
		NULL,
		&priv->amba_maps[0]);

	/* init AMBA bus, IRQCtrl, GPIO, bus down-maps */
	status = gr_rasta_tmtc_hw_init_common(priv);
	if (status)
		return status;

	/* Find GRPCI2 controller AHB Slave interface */
	tmp = (struct ambapp_dev *)ambapp_for_each(&priv->abus,
					(OPTIONS_ALL|OPTIONS_AHB_SLVS),
					VENDOR_GAISLER, GAISLER_GRPCI2,
					ambapp_find_by_idx, NULL);
	if ( !tmp ) {
		return -6;
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
		return -7;
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

static void gr_rasta_tmtc_hw_init2(struct gr_rasta_tmtc_priv *priv)
{
	/* Enable DMA by enabling PCI target as master */
	pci_master_enable(priv->pcidev);
}

/* Called when a PCI target is found with the PCI device and vendor ID 
 * given in gr_rasta_tmtc_ids[].
 */
int gr_rasta_tmtc_init1(struct drvmgr_dev *dev)
{
	struct gr_rasta_tmtc_priv *priv;
	struct pci_dev_info *devinfo;
	int status;
	uint32_t bar0, bar1, bar0_size, bar1_size;
	union drvmgr_key_value *value;
	int resources_cnt;

	priv = dev->priv;
	if (!priv)
		return DRVMGR_NOMEM;
	priv->dev = dev;

	/* Determine number of configurations */
	resources_cnt = get_resarray_count(gr_rasta_tmtc_resources);

	/* Generate Device prefix */

	strcpy(priv->prefix, "/dev/rastatmtc0");
	priv->prefix[14] += dev->minor_drv;
	mkdir(priv->prefix, S_IRWXU | S_IRWXG | S_IRWXO);
	priv->prefix[15] = '/';
	priv->prefix[16] = '\0';

	priv->devinfo = devinfo = (struct pci_dev_info *)dev->businfo;
	priv->pcidev = devinfo->pcidev;
	bar0 = devinfo->resources[0].address;
	bar0_size = devinfo->resources[0].size;
	bar1 = devinfo->resources[1].address;
	bar1_size = devinfo->resources[1].size;
	printk("\n\n--- GR-RASTA-TMTC[%d] ---\n", dev->minor_drv);
	printk(" PCI BUS: 0x%x, SLOT: 0x%x, FUNCTION: 0x%x\n",
		PCI_DEV_EXPAND(priv->pcidev));
	printk(" PCI VENDOR: 0x%04x, DEVICE: 0x%04x\n",
		devinfo->id.vendor, devinfo->id.device);
	printk(" PCI BAR[0]: 0x%" PRIx32 " - 0x%" PRIx32 "\n",
		bar0, bar0 + bar0_size - 1);
	printk(" PCI BAR[1]: 0x%" PRIx32 " - 0x%" PRIx32 "\n",
		bar1, bar1 + bar1_size - 1);
	printk(" IRQ: %d\n\n\n", devinfo->irq);

	/* all neccessary space assigned to GR-RASTA-IO target? */
	if ((bar0_size == 0) || (bar1_size == 0))
		return DRVMGR_ENORES;

	/* Initialize spin-lock for this PCI peripheral device. This is to
	 * protect the Interrupt Controller Registers. The genirq layer is
         * protecting its own internals and ISR dispatching.
         */
	SPIN_INIT(&priv->devlock, priv->prefix);

	/* Let user override which PCI address the AHB masters of the
	 * GR-RASTA-TMTC board access when doing DMA to CPU RAM. The AHB masters
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

	priv->genirq = genirq_init(32);
	if ( priv->genirq == NULL )
		return DRVMGR_FAIL;

	/* Select version of GR-RASTA-IO board */
	switch (devinfo->rev) {
		case 0:
			puts("GR-RASTA-TMTC: REVISION 0");
			status = gr_rasta_tmtc0_hw_init(priv);
			break;
		case 1:
			puts("GR-RASTA-TMTC: REVISION 1");
			status = gr_rasta_tmtc1_hw_init(priv);
			break;
		default:
			return DRVMGR_ENOSYS; /* HW not supported */
	}

	if ( status != 0 ) {
		genirq_destroy(priv->genirq);
		printk(" Failed to initialize GR-RASTA-TMTC HW: %d\n", status);
		return DRVMGR_FAIL;
	}

	/* Init amba bus */
	priv->config.abus = &priv->abus;
	priv->config.ops = &ambapp_rasta_tmtc_ops;
	priv->config.maps_up = &priv->bus_maps_up[0];
	priv->config.maps_down = &priv->bus_maps_down[0];
	if ( priv->dev->minor_drv < resources_cnt ) {
		priv->config.resources = gr_rasta_tmtc_resources[priv->dev->minor_drv];
	} else {
		priv->config.resources = NULL;
	}

	return ambapp_bus_register(dev, &priv->config);
}

int gr_rasta_tmtc_init2(struct drvmgr_dev *dev)
{
	struct gr_rasta_tmtc_priv *priv = dev->priv;

	/* Clear any old interrupt requests */
	drvmgr_interrupt_clear(priv->dev, 0);

	/* Enable System IRQ so that GR-RASTA-TMTC PCI target interrupt goes
	 * through.
	 *
	 * It is important to enable it in stage init2. If interrupts were
	 * enabled in init1 this might hang the system when more than one
	 * PCI target is connected, this is because PCI interrupts might 
	 * be shared and PCI board 2 have not initialized and
	 * might therefore drive interrupt already when entering init1().
	 */
	drvmgr_interrupt_register(
		priv->dev,
		0,
		"gr_rasta_tmtc",
		gr_rasta_tmtc_isr,
		(void *)priv);

	gr_rasta_tmtc_hw_init2(priv);

	return DRVMGR_OK;
}

int ambapp_rasta_tmtc_int_register(
	struct drvmgr_dev *dev,
	int irq,
	const char *info,
	drvmgr_isr handler,
	void *arg)
{
	struct gr_rasta_tmtc_priv *priv = dev->parent->dev->priv;
	SPIN_IRQFLAGS(irqflags);
	int status;
	void *h;

	h = genirq_alloc_handler(handler, arg);
	if ( h == NULL )
		return DRVMGR_FAIL;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	status = genirq_register(priv->genirq, irq, h);
	if ( status == 0 ) {
		/* Disable and clear IRQ for first registered handler */
		priv->irq->iclear = (1<<irq);
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
		priv->irq->mask[0] |= (1<<irq); /* unmask interrupt source */
	} else if ( status == 1 )
		status = 0;

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return status;
}

int ambapp_rasta_tmtc_int_unregister(
	struct drvmgr_dev *dev,
	int irq,
	drvmgr_isr isr,
	void *arg)
{
	struct gr_rasta_tmtc_priv *priv = dev->parent->dev->priv;
	SPIN_IRQFLAGS(irqflags);
	int status;
	void *handler;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	status = genirq_disable(priv->genirq, irq, isr, arg);
	if ( status == 0 ) {
		/* Disable IRQ only when no enabled handler exists */
		priv->irq->mask[0] &= ~(1<<irq); /* mask interrupt source */
	} else if ( status == 1 )
		status = 0;

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

int ambapp_rasta_tmtc_int_unmask(
	struct drvmgr_dev *dev,
	int irq)
{
	struct gr_rasta_tmtc_priv *priv = dev->parent->dev->priv;
	SPIN_IRQFLAGS(irqflags);

	DBG("RASTA-TMTC IRQ %d: unmask\n", irq);

	if ( genirq_check(priv->genirq, irq) )
		return DRVMGR_EINVAL;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	/* Enable IRQ */
	priv->irq->mask[0] |= (1<<irq); /* unmask interrupt source */

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return DRVMGR_OK;
}

int ambapp_rasta_tmtc_int_mask(
	struct drvmgr_dev *dev,
	int irq)
{
	struct gr_rasta_tmtc_priv *priv = dev->parent->dev->priv;
	SPIN_IRQFLAGS(irqflags);

	DBG("RASTA-TMTC IRQ %d: mask\n", irq);

	if ( genirq_check(priv->genirq, irq) )
		return DRVMGR_EINVAL;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	/* Disable IRQ */
	priv->irq->mask[0] &= ~(1<<irq); /* mask interrupt source */

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return DRVMGR_OK;
}

int ambapp_rasta_tmtc_int_clear(
	struct drvmgr_dev *dev,
	int irq)
{
	struct gr_rasta_tmtc_priv *priv = dev->parent->dev->priv;

	if ( genirq_check(priv->genirq, irq) )
		return DRVMGR_FAIL;

	priv->irq->iclear = (1<<irq);

	return DRVMGR_OK;
}

int ambapp_rasta_tmtc_get_params(struct drvmgr_dev *dev, struct drvmgr_bus_params *params)
{
	struct gr_rasta_tmtc_priv *priv = dev->parent->dev->priv;

	/* Device name prefix pointer, skip /dev */
	params->dev_prefix = &priv->prefix[5];

	return 0;
}

void gr_rasta_tmtc_print_dev(struct drvmgr_dev *dev, int options)
{
	struct gr_rasta_tmtc_priv *priv = dev->priv;
	struct pci_dev_info *devinfo = priv->devinfo;
	uint32_t bar0, bar1, bar0_size, bar1_size;

	/* Print */
	printf("--- GR-RASTA-TMTC [bus 0x%x, dev 0x%x, fun 0x%x] ---\n",
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
	printf(" PCI REVISION:    %d\n", devinfo->rev);
	printf(" FREQ:            %d Hz\n", priv->version->amba_freq_hz);
	printf(" IMASK:           0x%08x\n", priv->irq->mask[0]);
	printf(" IPEND:           0x%08x\n", priv->irq->ipend);

	/* Print amba config */
	if ( options & RASTA_TMTC_OPTIONS_AMBA ) {
		ambapp_print(&priv->abus, 10);
	}

#if 0
	/* Print IRQ handlers and their arguments */
	if ( options & RASTA_TMTC_OPTIONS_IRQ ) {
		int i;
		for(i=0; i<16; i++) {
			printf(" IRQ[%02d]:         0x%x, arg: 0x%x\n", 
				i, (unsigned int)priv->isrs[i].handler, (unsigned int)priv->isrs[i].arg);
		}
	}
#endif
}

void gr_rasta_tmtc_print(int options)
{
	struct pci_drv_info *drv = &gr_rasta_tmtc_info;
	struct drvmgr_dev *dev;

	dev = drv->general.dev;
	while(dev) {
		gr_rasta_tmtc_print_dev(dev, options);
		dev = dev->next_in_drv;
	}
}
