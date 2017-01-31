/*  GR-CPCI-LEON4-N2X (NGFP) PCI Peripheral driver
 *
 *  COPYRIGHT (c) 2013.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 *  Configures the GR-CPIC-LEON4-N2X interface PCI board in peripheral
 *  mode. This driver provides a AMBA PnP bus by using the general part
 *  of the AMBA PnP bus driver (ambapp_bus.c).
 *
 *
 *  Driver resource options:
 *   NAME          DEFAULT VALUE
 *   ahbmst2pci    _RAM_START            AMBA->PCI translation PCI base address
 *   ambaFreq      200000000 (200MHz)    AMBA system frequency of LEON4-N2X
 *   cgEnMask      0x1f (all)            Clock gating enable mask
 *
 * TODO/UNTESTED
 *   Interrupt testing
 *   bar0 RESOURCE 0x00000000            L2-Cache SDRAM memory
 *   bar1 RESOURCE 0xf0000000            L2-Cache registers
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <bsp.h>
#include <rtems/bspIo.h>
#include <pci.h>

#include <ambapp.h>
#include <grlib.h>
#include <drvmgr/drvmgr.h>
#include <drvmgr/ambapp_bus.h>
#include <drvmgr/pci_bus.h>
#include <drvmgr/bspcommon.h>
#include <bsp/genirq.h>

#include <bsp/gr_leon4_n2x.h>

/* Determines which PCI address the AHB masters on the LEON-N2X board will
 * access when accessing the AHB to PCI window, it should be set so that the
 * masters can access the HOST RAM.
 * Default is base of HOST RAM, HOST RAM is mapped 1:1 to PCI memory space.
 */
extern unsigned int _RAM_START;
#define AHBMST2PCIADR (((unsigned int)&_RAM_START) & 0xc0000000)

#define GRPCI2_BAR0_TO_AHB_MAP 0x04
#define GRPCI2_BAR1_TO_AHB_MAP 0x08
#define GRPCI2_BAR2_TO_AHB_MAP 0x0c
#define GRPCI2_PCI_CONFIG      0x20
#define CAP9_AHBPREF_OFS 0x3c

/* #define DEBUG 1 */

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...) 
#endif

int gr_cpci_leon4_n2x_init1(struct drvmgr_dev *dev);
int gr_cpci_leon4_n2x_init2(struct drvmgr_dev *dev);
void gr_cpci_leon4_n2x_isr(void *arg);

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

/* Clock gating unit register layout */
struct l4n2x_grcg_regs {
	volatile unsigned int unlock;
	volatile unsigned int enable;
	volatile unsigned int reset;
	volatile unsigned int cpu_fpu;
};
#define CG_MASK 0x1f

/* Private data structure for driver */
struct gr_cpci_leon4_n2x_priv {
	/* Driver management */
	struct drvmgr_dev	*dev;
	char			prefix[20];

	/* PCI */
	pci_dev_t		pcidev;
	struct pci_dev_info	*devinfo;
	uint32_t		ahbmst2pci_map;

	/* IRQ */
	int			eirq;
	genirq_t		genirq;

	/* GR-CPCI-LEON4-N2X */
	unsigned int		amba_freq_hz;
	unsigned int		cg_en_mask;		/* Enabled cores */
	struct irqmp_regs	*irq;
	struct l4n2x_grcg_regs	*cg;			/* Clock-gating unit */
	struct grpci2_regs	*grpci2;
	struct drvmgr_map_entry	bus_maps_up[2];
	struct drvmgr_map_entry	bus_maps_down[4];

	/* AMBA Plug&Play information on GR-CPCI-LEON4-N2X */
	struct ambapp_bus	abus;
	struct ambapp_mmap	amba_maps[5];
	struct ambapp_config	config;
};

int ambapp_leon4_n2x_int_register(
	struct drvmgr_dev *dev,
	int irq,
	const char *info,
	drvmgr_isr handler,
	void *arg);
int ambapp_leon4_n2x_int_unregister(
	struct drvmgr_dev *dev,
	int irq,
	drvmgr_isr handler,
	void *arg);
int ambapp_leon4_n2x_int_unmask(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_leon4_n2x_int_mask(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_leon4_n2x_int_clear(
	struct drvmgr_dev *dev,
	int irq);
int ambapp_leon4_n2x_get_params(
	struct drvmgr_dev *dev,
	struct drvmgr_bus_params *params);

static struct ambapp_ops ambapp_leon4_n2x_ops = {
	.int_register = ambapp_leon4_n2x_int_register,
	.int_unregister = ambapp_leon4_n2x_int_unregister,
	.int_unmask = ambapp_leon4_n2x_int_unmask,
	.int_mask = ambapp_leon4_n2x_int_mask,
	.int_clear = ambapp_leon4_n2x_int_clear,
	.get_params = ambapp_leon4_n2x_get_params
};

struct drvmgr_drv_ops gr_cpci_leon4_n2x_ops = 
{
	.init = {gr_cpci_leon4_n2x_init1, gr_cpci_leon4_n2x_init2, NULL, NULL},
	.remove = NULL,
	.info = NULL
};

struct pci_dev_id_match gr_cpci_leon4_n2x_ids[] =
{
	PCIID_DEVVEND(PCIID_VENDOR_GAISLER, PCIID_DEVICE_GR_LEON4_N2X),
	PCIID_DEVVEND(PCIID_VENDOR_GAISLER, PCIID_DEVICE_GR_NGMP_PROTO),
	PCIID_END_TABLE /* Mark end of table */
};

struct pci_drv_info gr_cpci_leon4_n2x_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_PCI_GAISLER_LEON4_N2X_ID,/* Driver ID */
		"GR-CPCI-LEON4-N2X",		/* Driver Name */
		DRVMGR_BUS_TYPE_PCI,		/* Bus Type */
		&gr_cpci_leon4_n2x_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct gr_cpci_leon4_n2x_priv),
	},
	&gr_cpci_leon4_n2x_ids[0]
};

/* Driver resources configuration for the AMBA bus on the GR-CPCI-LEON4-N2X board.
 * It is declared weak so that the user may override it from the project file,
 * if the default settings are not enough.
 *
 * The configuration consists of an array of configuration pointers, each
 * pointer determine the configuration of one GR-CPCI-LEON4-N2X board. Pointer
 * zero is for board0, pointer 1 for board1 and so on.
 *
 * The array must end with a NULL pointer.
 */
struct drvmgr_bus_res *gr_leon4_n2x_resources[] __attribute__((weak)) =
{
	NULL
};

void gr_cpci_leon4_n2x_register_drv(void)
{
	DBG("Registering GR-CPCI-LEON4-N2X PCI driver\n");
	drvmgr_drv_register(&gr_cpci_leon4_n2x_info.general);
}

void gr_cpci_leon4_n2x_isr(void *arg)
{
	struct gr_cpci_leon4_n2x_priv *priv = arg;
	unsigned int status, tmp;
	int irq, eirq;
	tmp = status = priv->irq->ipend;

	/* DBG("GR-CPCI-LEON4-N2X: IRQ 0x%x\n",status); */

	for(irq = 0; irq < 32; irq++) {
		if (status & (1 << irq)) {
			if (irq == priv->eirq) {
				while ((eirq = priv->irq->intid[0] & 0x1f)) {
					if ((eirq & 0x10) == 0)
						continue;
					genirq_doirq(priv->genirq, eirq);
					priv->irq->iclear = (1 << eirq);
				}
			} else {
				genirq_doirq(priv->genirq, irq);
			}
			priv->irq->iclear = (1 << irq);
			status &= ~(1 << irq);
			if ( status == 0 )
				break;
		}
	}

	/* ACK interrupt, this is because PCI is Level, so the IRQ Controller
	 * still drives the IRQ
	 */
	if ( tmp ) 
		drvmgr_interrupt_clear(priv->dev, 0);

	DBG("GR-CPCI-LEON4-N2X-IRQ: 0x%x\n", tmp);
}

static int gr_cpci_leon4_n2x_hw_init1(struct gr_cpci_leon4_n2x_priv *priv)
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
	unsigned int cgmask, enabled;

	/* Check capabilities list bit and read its pointer */
	pci_cfg_r8(pcidev, PCIR_STATUS, &tmp2);
	if (!((tmp2 >> 4) & 1)) {
		/* Capabilities list not available which it should be in the GRPCI2 */
		return -2;
	}
	pci_cfg_r8(pcidev, PCIR_CAP_PTR, &cap_ptr);

	/* Workarounds depends on PCI revision of GR-CPCI-LEON4-N2X board */
	switch (devinfo->rev) {
	case 0:
		/* Limit the AMBA prefetch for GRPCI2 version 0. */
		pci_cfg_w32(pcidev, cap_ptr+CAP9_AHBPREF_OFS, 0);
	default:
		break;
	}

	/* Set AHB address mappings for target PCI bars
	 *  BAR0 maps to 0x00000000-0x07ffffff 128MB (SDRAM/DDR2 memory)
	 *  BAR1 maps to 0xf0000000-0xf7ffffff 128MB (L2-Cache regs/diagnostics)
	 *  BAR2 maps to 0xff800000-0xffffffff   8MB (PnP, I/O regs)
	 */
	pci_cfg_w32(pcidev, cap_ptr+GRPCI2_BAR0_TO_AHB_MAP, 0x00000000);
	pci_cfg_w32(pcidev, cap_ptr+GRPCI2_BAR1_TO_AHB_MAP, 0xf0000000);
	pci_cfg_w32(pcidev, cap_ptr+GRPCI2_BAR2_TO_AHB_MAP, 0xff800000);

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

	/* AMBA MAP bar0 (in leon4-n2x) ==> 0x00000000 (remote amba address) */
	priv->amba_maps[0].size = devinfo->resources[0].size;
	priv->amba_maps[0].local_adr = devinfo->resources[0].address;
	priv->amba_maps[0].remote_adr = 0x00000000;

	priv->amba_maps[1].size = devinfo->resources[1].size;
	priv->amba_maps[1].local_adr = devinfo->resources[1].address;
	priv->amba_maps[1].remote_adr = 0xf0000000;

	priv->amba_maps[2].size = devinfo->resources[2].size;
	priv->amba_maps[2].local_adr = devinfo->resources[2].address;
	priv->amba_maps[2].remote_adr = 0xff800000;

	/* Addresses not matching with map be untouched */
	priv->amba_maps[3].size = 0xfffffff0;
	priv->amba_maps[3].local_adr = 0;
	priv->amba_maps[3].remote_adr = 0;

	/* Mark end of table */
	priv->amba_maps[4].size=0;

	/* Start AMBA PnP scan at first AHB bus */
	ambapp_scan(
		&priv->abus,
		devinfo->resources[2].address + 0x00700000,
		NULL,
		&priv->amba_maps[0]);

	/* Initialize Frequency of AMBA bus */
	ambapp_freq_init(&priv->abus, NULL, priv->amba_freq_hz);

	/* Find IRQ controller, Clear all current IRQs */
	tmp = (struct ambapp_dev *)ambapp_for_each(&priv->abus,
				(OPTIONS_ALL|OPTIONS_APB_SLVS),
				VENDOR_GAISLER, GAISLER_IRQMP,
				ambapp_find_by_idx, NULL);
	if ( !tmp ) {
		return -4;
	}
	priv->irq = (struct irqmp_regs *)DEV_TO_APB(tmp)->start;
	/* Set up GR-CPCI-LEON4-N2X irq controller
	 * Interrupts are routed from IRQCtrl0, we leave the configuration
	 * for the other CPUs, as the board's CPUs may be running something.
	 * We assume IRQCtrl has been set up properly, or at least the reset
	 * values shuold work with this code..
	 */
	priv->irq->mask[0] = 0;
	priv->irq->iforce = 0;
	priv->irq->force[0] = 0;
	priv->irq->ilevel = 0;
	priv->irq->ipend = 0;
	priv->irq->iclear = 0xffffffff;
	priv->irq->ilevel = 0;
	/* Get extended Interrupt controller IRQ number */
	priv->eirq = (priv->irq->mpstat >> 16) & 0xf;

	/* Find first Clock-Gating unit, enable/disable the requested cores.
	 * It is optional in order to support FPGA prototypes.
	 */
	priv->cg = NULL;
	tmp = (struct ambapp_dev *)ambapp_for_each(&priv->abus,
				(OPTIONS_ALL|OPTIONS_APB_SLVS),
				VENDOR_GAISLER, GAISLER_CLKGATE,
				ambapp_find_by_idx, NULL);
	if (tmp)
		priv->cg = (struct l4n2x_grcg_regs *)DEV_TO_APB(tmp)->start;

	/* Do reset and enable sequence only if not already enabled */
	if (priv->cg && ((enabled = priv->cg->enable) != priv->cg_en_mask)) {
		/* First disable already enabled cores */
		cgmask = ~priv->cg_en_mask & enabled;
		if (cgmask) {
			priv->cg->unlock = cgmask;
			priv->cg->enable = enabled = ~cgmask & enabled;
			priv->cg->unlock = 0;
		}
		/* Enable disabled cores */
		cgmask = priv->cg_en_mask & ~enabled;
		if (cgmask) {
			priv->cg->unlock = cgmask;
			priv->cg->reset |= cgmask;
			priv->cg->enable = cgmask | enabled;
			priv->cg->reset &= ~cgmask;
			priv->cg->unlock = 0;
		}
	}

	priv->bus_maps_down[0].name = "PCI BAR0 -> AMBA";
	priv->bus_maps_down[0].size = priv->amba_maps[0].size;
	priv->bus_maps_down[0].from_adr = (void *)priv->amba_maps[0].local_adr;
	priv->bus_maps_down[0].to_adr = (void *)priv->amba_maps[0].remote_adr;
	priv->bus_maps_down[1].name = "PCI BAR1 -> AMBA";
	priv->bus_maps_down[1].size = priv->amba_maps[1].size;
	priv->bus_maps_down[1].from_adr = (void *)priv->amba_maps[1].local_adr;
	priv->bus_maps_down[1].to_adr = (void *)priv->amba_maps[1].remote_adr;
	priv->bus_maps_down[2].name = "PCI BAR2 -> AMBA";
	priv->bus_maps_down[2].size = priv->amba_maps[2].size;
	priv->bus_maps_down[2].from_adr = (void *)priv->amba_maps[2].local_adr;
	priv->bus_maps_down[2].to_adr = (void *)priv->amba_maps[2].remote_adr;
	priv->bus_maps_down[3].size = 0;

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
	priv->bus_maps_up[0].size = ahb->mask[0]; /* AMBA->PCI Window on GR-CPCI-LEON4-N2X board */
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

	/* Workarounds depends on PCI revision of GR-CPCI-LEON4-N2X board */
	switch (devinfo->rev) {
	case 0:
		/* Limit the PCI master prefetch for GRPCI2 version 0.
		 * This fix is required only when PCI Host bridge is
		 * GRPCI2 rev 0.
		 */
		priv->grpci2->ppref = 0xffff0000;
	default:
		break;
	}

	/* Set AHB to PCI mapping for all AMBA AHB masters */
	for(i = 0; i < 16; i++) {
		priv->grpci2->ahbmst_map[i] = priv->ahbmst2pci_map &
							~(ahb->mask[0]-1);
	}

	/* Make sure dirq(0) sampling is enabled */
	ctrl = priv->grpci2->ctrl;
	ctrl = (ctrl & 0xFFFFFF0F) | (1 << 4);
	priv->grpci2->ctrl = ctrl;

	/* Successfully registered the LEON4-N2X board */
	return 0;
}

static int gr_cpci_leon4_n2x_hw_init2(struct gr_cpci_leon4_n2x_priv *priv)
{
	/* Enable DMA by enabling PCI target as master */
	pci_master_enable(priv->pcidev);

	return DRVMGR_OK;
}

/* Called when a PCI target is found with the PCI device and vendor ID 
 * given in gr_cpci_leon4_n2x_ids[].
 */
int gr_cpci_leon4_n2x_init1(struct drvmgr_dev *dev)
{
	struct gr_cpci_leon4_n2x_priv *priv;
	struct pci_dev_info *devinfo;
	int status, i;
	union drvmgr_key_value *value;
	int resources_cnt;

	priv = dev->priv;
	if (!priv)
		return DRVMGR_NOMEM;

	memset(priv, 0, sizeof(*priv));
	dev->priv = priv;
	priv->dev = dev;

	/* Determine number of configurations */
	resources_cnt = get_resarray_count(gr_leon4_n2x_resources);

	/* Generate Device prefix */

	strcpy(priv->prefix, "/dev/leon4n2x0");
	priv->prefix[13] += dev->minor_drv;
	mkdir(priv->prefix, S_IRWXU | S_IRWXG | S_IRWXO);
	priv->prefix[14] = '/';
	priv->prefix[15] = '\0';

	priv->devinfo = devinfo = (struct pci_dev_info *)dev->businfo;
	priv->pcidev = devinfo->pcidev;
	printf("\n\n--- GR-CPCI-LEON4-N2X[%d] ---\n", dev->minor_drv);
	printf(" PCI BUS: 0x%x, SLOT: 0x%x, FUNCTION: 0x%x\n",
		PCI_DEV_EXPAND(priv->pcidev));
	printf(" PCI VENDOR: 0x%04x, DEVICE: 0x%04x\n",
		devinfo->id.vendor, devinfo->id.device);
	for (i = 0; i < 3; i++) {
		printf(" PCI BAR[%d]: 0x%08lx - 0x%08lx\n",
			i, devinfo->resources[i].address,
			devinfo->resources[i].address +
			(devinfo->resources[i].size - 1));
		/* all neccessary space assigned to GR-CPCI-LEON4-N2X target? */
		if (devinfo->resources[i].size == 0)
			return DRVMGR_ENORES;
	}
	printf(" IRQ: %d\n\n\n", devinfo->irq);

	/* Let user override which PCI address the AHB masters of the
	 * LEON4-N2X board access when doing DMA to HOST RAM. The AHB masters
	 * access the PCI Window of the AMBA bus, the MSB 2-bits of that address
	 * is translated according this config option before the address goes
	 * out on the PCI bus.
	 *
	 * Only the 2 MSB bits have an effect.
	 */
	value = drvmgr_dev_key_get(priv->dev, "ahbmst2pci", DRVMGR_KT_INT);
	if (value)
		priv->ahbmst2pci_map = value->i;
	else
		priv->ahbmst2pci_map = AHBMST2PCIADR; /* default */

	/* Let user override the default AMBA system frequency of the 
	 * CPU-bus of the remote GR-CPCI-LEON4-N2X. Default is 200MHz.
	 */
	value = drvmgr_dev_key_get(priv->dev, "ambaFreq", DRVMGR_KT_INT);
	if (value)
		priv->amba_freq_hz = value->i;
	else
		priv->amba_freq_hz = 200000000; /* default */

	/* Let user determine clock-gating unit configuration. The default
	 * is to turn all cores on (disable gating). PCI is always turned ON.
	 */
	value = drvmgr_dev_key_get(priv->dev, "cgEnMask", DRVMGR_KT_INT);
	if (value)
		priv->cg_en_mask = (value->i & CG_MASK) | 0x08;
	else
		priv->cg_en_mask = CG_MASK; /* default all ON */

	priv->genirq = genirq_init(32);
	if (priv->genirq == NULL)
		return DRVMGR_FAIL;

	if ((status = gr_cpci_leon4_n2x_hw_init1(priv)) != 0) {
		genirq_destroy(priv->genirq);
		printf(" Failed to initialize GR-CPCI-LEON4-N2X HW: %d\n", status);
		return DRVMGR_FAIL;
	}

	/* Init amba bus */
	priv->config.abus = &priv->abus;
	priv->config.ops = &ambapp_leon4_n2x_ops;
	priv->config.maps_up = &priv->bus_maps_up[0];
	priv->config.maps_down = &priv->bus_maps_down[0];
	if ( priv->dev->minor_drv < resources_cnt ) {
		priv->config.resources = gr_leon4_n2x_resources[priv->dev->minor_drv];
	} else {
		priv->config.resources = NULL;
	}

	/* Create and register AMBA PnP bus. */
	return ambapp_bus_register(dev, &priv->config);
}

int gr_cpci_leon4_n2x_init2(struct drvmgr_dev *dev)
{
	struct gr_cpci_leon4_n2x_priv *priv = dev->priv;

	/* Clear any old interrupt requests */
	drvmgr_interrupt_clear(dev, 0);

	/* Enable System IRQ so that GR-CPCI-LEON4-N2X PCI target interrupt
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
		"gr_cpci_leon4_n2x",
		gr_cpci_leon4_n2x_isr,
		(void *)priv);

	return gr_cpci_leon4_n2x_hw_init2(priv);
}

int ambapp_leon4_n2x_int_register(
	struct drvmgr_dev *dev,
	int irq,
	const char *info,
	drvmgr_isr handler,
	void *arg)
{
	struct gr_cpci_leon4_n2x_priv *priv = dev->parent->dev->priv;
	rtems_interrupt_level level;
	int status;

	rtems_interrupt_disable(level);

	status = genirq_register(priv->genirq, irq, handler, arg);
	if (status == 0) {
		/* Clear IRQ for first registered handler */
		priv->irq->iclear = (1<<irq);
	} else if (status == 1)
		status = 0;

	if (status != 0) {
		rtems_interrupt_enable(level);
		return DRVMGR_FAIL;
	}

	status = genirq_enable(priv->genirq, irq, handler, arg);
	if ( status == 0 ) {
		/* Enable IRQ for first enabled handler only */
		priv->irq->mask[0] |= (1<<irq); /* unmask interrupt source */
	} else if ( status == 1 )
		status = 0;

	rtems_interrupt_enable(level);

	return status;
}

int ambapp_leon4_n2x_int_unregister(
	struct drvmgr_dev *dev,
	int irq,
	drvmgr_isr isr,
	void *arg)
{
	struct gr_cpci_leon4_n2x_priv *priv = dev->parent->dev->priv;
	rtems_interrupt_level level;
	int status;

	rtems_interrupt_disable(level);

	status = genirq_disable(priv->genirq, irq, isr, arg);
	if ( status == 0 ) {
		/* Disable IRQ only when no enabled handler exists */
		priv->irq->mask[0] &= ~(1<<irq); /* mask interrupt source */
	}

	status = genirq_unregister(priv->genirq, irq, isr, arg);
	if ( status != 0 )
		status = DRVMGR_FAIL;

	rtems_interrupt_enable(level);

	return status;
}

int ambapp_leon4_n2x_int_unmask(
	struct drvmgr_dev *dev,
	int irq)
{
	struct gr_cpci_leon4_n2x_priv *priv = dev->parent->dev->priv;
	rtems_interrupt_level level;

	DBG("LEON4-N2X IRQ %d: unmask\n", irq);

	if ( genirq_check(priv->genirq, irq) )
		return DRVMGR_EINVAL;

	rtems_interrupt_disable(level);

	/* Enable IRQ for first enabled handler only */
	priv->irq->mask[0] |= (1<<irq); /* unmask interrupt source */

	rtems_interrupt_enable(level);

	return DRVMGR_OK;
}

int ambapp_leon4_n2x_int_mask(
	struct drvmgr_dev *dev,
	int irq)
{
	struct gr_cpci_leon4_n2x_priv *priv = dev->parent->dev->priv;
	rtems_interrupt_level level;

	DBG("LEON4-N2X IRQ %d: mask\n", irq);

	if ( genirq_check(priv->genirq, irq) )
		return DRVMGR_EINVAL;

	rtems_interrupt_disable(level);

	/* Disable/mask IRQ */
	priv->irq->mask[0] &= ~(1<<irq); /* mask interrupt source */

	rtems_interrupt_enable(level);

	return DRVMGR_OK;
}

int ambapp_leon4_n2x_int_clear(
	struct drvmgr_dev *dev,
	int irq)
{
	struct gr_cpci_leon4_n2x_priv *priv = dev->parent->dev->priv;

	if ( genirq_check(priv->genirq, irq) )
		return DRVMGR_EINVAL;

	priv->irq->iclear = (1<<irq);

	return DRVMGR_OK;
}

int ambapp_leon4_n2x_get_params(struct drvmgr_dev *dev, struct drvmgr_bus_params *params)
{
	struct gr_cpci_leon4_n2x_priv *priv = dev->parent->dev->priv;

	/* Device name prefix pointer, skip /dev */
	params->dev_prefix = &priv->prefix[5];

	return 0;
}

void gr_cpci_leon4_n2x_print_dev(struct drvmgr_dev *dev, int options)
{
	struct gr_cpci_leon4_n2x_priv *priv = dev->priv;
	struct pci_dev_info *devinfo = priv->devinfo;
	uint32_t bar0, bar0_size;

	/* Print */
	printf("--- GR-CPCI-LEON4-N2X [bus 0x%x, dev 0x%x, fun 0x%x] ---\n",
		PCI_DEV_EXPAND(priv->pcidev));

	bar0 = devinfo->resources[0].address;
	bar0_size = devinfo->resources[0].size;
	printf(" PCI BAR[0]: 0x%lx - 0x%lx\n", bar0, bar0 + bar0_size - 1);
	printf(" IRQ REGS:        0x%x\n", (unsigned int)priv->irq);
	printf(" IRQ:             %d\n", devinfo->irq);
	printf(" PCI REVISION:    %d\n", devinfo->rev);
	printf(" FREQ:            %d Hz\n", priv->amba_freq_hz);
	printf(" IMASK:           0x%08x\n", priv->irq->mask[0]);
	printf(" IPEND:           0x%08x\n", priv->irq->ipend);

	/* Print amba config */
	if (options & GR_LEON4_N2X_OPTIONS_AMBA)
		ambapp_print(&priv->abus, 10);
}

void gr_leon4_n2x_print(int options)
{
	struct pci_drv_info *drv = &gr_cpci_leon4_n2x_info;
	struct drvmgr_dev *dev;

	dev = drv->general.dev;
	while(dev) {
		gr_cpci_leon4_n2x_print_dev(dev, options);
		dev = dev->next_in_drv;
	}
}
