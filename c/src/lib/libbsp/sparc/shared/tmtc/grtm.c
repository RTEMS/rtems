/* GRTM CCSDS Telemetry Encoder driver
 *
 * COPYRIGHT (c) 2007.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <bsp.h>
#include <rtems/libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <rtems/bspIo.h>

#include <drvmgr/drvmgr.h>
#include <ambapp.h>
#include <drvmgr/ambapp_bus.h>
#include <bsp/grtm.h>

#ifndef IRQ_GLOBAL_PREPARE
 #define IRQ_GLOBAL_PREPARE(level) rtems_interrupt_level level
#endif

#ifndef IRQ_GLOBAL_DISABLE
 #define IRQ_GLOBAL_DISABLE(level) rtems_interrupt_disable(level)
#endif

#ifndef IRQ_GLOBAL_ENABLE
 #define IRQ_GLOBAL_ENABLE(level) rtems_interrupt_enable(level)
#endif

/*
#define DEBUG
#define DEBUGFUNCS
*/

#include <bsp/debug_defs.h>

/* GRTM register map */
struct grtm_regs {
	volatile unsigned int	dma_ctrl;	/* DMA Control Register (0x00) */
	volatile unsigned int	dma_status;	/* DMA Status Register (0x04) */
	volatile unsigned int	dma_len;	/* DMA Length Register (0x08) */	
	volatile unsigned int	dma_bd;		/* DMA Descriptor Pointer Register (0x0c) */

	volatile unsigned int	dma_cfg;	/* DMA Configuration Register (0x10) */
	volatile unsigned int	revision;	/* GRTM Revision Register (0x14) */

	int unused0[(0x80-0x18)/4];

	volatile unsigned int	ctrl;		/* TM Control Register (0x80) */
	volatile unsigned int	status;		/* TM Status Register (0x84) */
	volatile unsigned int	cfg;		/* TM Configuration Register (0x88) */
	volatile unsigned int	size;		/* TM Size Register (0x8c) */

	volatile unsigned int	phy;		/* TM Physical Layer Register (0x90) */
	volatile unsigned int	code;		/* TM Coding Sub-Layer Register (0x94) */
	volatile unsigned int	asmr;		/* TM Attached Synchronization Marker Register (0x98) */

	int unused1;

	volatile unsigned int	all_frm;	/* TM All Frames Generation Register (0xa0) */
	volatile unsigned int	mst_frm;	/* TM Master Channel Frame Generation Register (0xa4) */
	volatile unsigned int	idle_frm;	/* TM Idle Frame Generation Register (0xa8) */

	int unused2[(0xc0-0xac)/4];

	volatile unsigned int	fsh[4];		/* TM FSH/Insert Zone Registers (0xc0..0xcc) */

	volatile unsigned int	ocf;		/* TM Operational Control Field Register (0xd0) */
};

/* DMA Control Register (0x00) */
#define GRTM_DMA_CTRL_EN_BIT	0
#define GRTM_DMA_CTRL_IE_BIT	1
#define GRTM_DMA_CTRL_TXRST_BIT	2
#define GRTM_DMA_CTRL_RST_BIT	3
#define GRTM_DMA_CTRL_TFIE_BIT	4

#define GRTM_DMA_CTRL_EN	(1<<GRTM_DMA_CTRL_EN_BIT)
#define GRTM_DMA_CTRL_IE	(1<<GRTM_DMA_CTRL_IE_BIT)
#define GRTM_DMA_CTRL_TXRST	(1<<GRTM_DMA_CTRL_TXRST_BIT)
#define GRTM_DMA_CTRL_RST	(1<<GRTM_DMA_CTRL_RST_BIT)
#define GRTM_DMA_CTRL_TFIE	(1<<GRTM_DMA_CTRL_TFIE_BIT)

/* DMA Status Register (0x04) */
#define GRTM_DMA_STS_TE_BIT	0
#define GRTM_DMA_STS_TI_BIT	1
#define GRTM_DMA_STS_TA_BIT	2
#define GRTM_DMA_STS_TFF_BIT	3
#define GRTM_DMA_STS_TFS_BIT	4

#define GRTM_DMA_STS_TE		(1<<GRTM_DMA_STS_TE_BIT)
#define GRTM_DMA_STS_TI		(1<<GRTM_DMA_STS_TI_BIT)
#define GRTM_DMA_STS_TA		(1<<GRTM_DMA_STS_TA_BIT)
#define GRTM_DMA_STS_TFF	(1<<GRTM_DMA_STS_TFF_BIT)
#define GRTM_DMA_STS_TFS	(1<<GRTM_DMA_STS_TFS_BIT)
#define GRTM_DMA_STS_ALL	0x1f

/* DMA Length Register (0x08) */
#define GRTM_DMA_LEN_LEN_BIT	0
#define GRTM_DMA_LEN_LIM_BIT	16

#define GRTM_DMA_LEN_LEN	(0x7ff<<GRTM_DMA_LEN_LEN_BIT)
#define GRTM_DMA_LEN_LIM	(0x3ff<<GRTM_DMA_LEN_LIM_BIT)

/* DMA Descriptor Pointer Register (0x0c) */
#define GRTM_DMA_BD_INDEX_BIT	0
#define GRTM_DMA_BD_BASE_BIT	10

#define GRTM_DMA_BD_INDEX	(0x3ff<<GRTM_DMA_BD_INDEX_BIT)
#define GRTM_DMA_BD_BASE	(0xfffffc<<GRTM_DMA_BD_BASE_BIT)

/* DMA Configuration Register (0x10) */
#define GRTM_DMA_CFG_BLKSZ_BIT	0
#define GRTM_DMA_CFG_FIFOSZ_BIT	16

#define GRTM_DMA_CFG_BLKSZ	(0xffff<<GRTM_DMA_CFG_BLKSZ_BIT)
#define GRTM_DMA_CFG_FIFOSZ	(0xffff<<GRTM_DMA_CFG_FIFOSZ_BIT)

/* TM Control Register (0x80) */
#define GRTM_CTRL_EN_BIT	0

#define GRTM_CTRL_EN		(1<<GRTM_CTRL_EN_BIT)

/* TM Status Register (0x84) - Unused */

/* TM Configuration Register (0x88) */
#define GRTM_CFG_SC_BIT		0
#define GRTM_CFG_SP_BIT		1
#define GRTM_CFG_CE_BIT		2
#define GRTM_CFG_NRZ_BIT	3
#define GRTM_CFG_PSR_BIT	4
#define GRTM_CFG_TE_BIT		5
#define GRTM_CFG_RSDEP_BIT	6
#define GRTM_CFG_RS_BIT		9
#define GRTM_CFG_AASM_BIT	11
#define GRTM_CFG_FECF_BIT	12
#define GRTM_CFG_OCF_BIT	13
#define GRTM_CFG_EVC_BIT	14
#define GRTM_CFG_IDLE_BIT	15
#define GRTM_CFG_FSH_BIT	16
#define GRTM_CFG_MCG_BIT	17
#define GRTM_CFG_IZ_BIT		18
#define GRTM_CFG_FHEC_BIT	19
#define GRTM_CFG_AOS_BIT	20
#define GRTM_CFG_CIF_BIT	21
#define GRTM_CFG_OCFB_BIT	22

#define GRTM_CFG_SC		(1<<GRTM_CFG_SC_BIT)
#define GRTM_CFG_SP		(1<<GRTM_CFG_SP_BIT)
#define GRTM_CFG_CE		(1<<GRTM_CFG_CE_BIT)
#define GRTM_CFG_NRZ		(1<<GRTM_CFG_NRZ_BIT)
#define GRTM_CFG_PSR		(1<<GRTM_CFG_PSR_BIT)
#define GRTM_CFG_TE		(1<<GRTM_CFG_TE_BIT)
#define GRTM_CFG_RSDEP		(0x7<<GRTM_CFG_RSDEP_BIT)
#define GRTM_CFG_RS		(0x3<<GRTM_CFG_RS_BIT)
#define GRTM_CFG_AASM		(1<<GRTM_CFG_AASM_BIT)
#define GRTM_CFG_FECF		(1<<GRTM_CFG_FECF_BIT)
#define GRTM_CFG_OCF		(1<<GRTM_CFG_OCF_BIT)
#define GRTM_CFG_EVC		(1<<GRTM_CFG_EVC_BIT)
#define GRTM_CFG_IDLE		(1<<GRTM_CFG_IDLE_BIT)
#define GRTM_CFG_FSH		(1<<GRTM_CFG_FSH_BIT)
#define GRTM_CFG_MCG		(1<<GRTM_CFG_MCG_BIT)
#define GRTM_CFG_IZ		(1<<GRTM_CFG_IZ_BIT)
#define GRTM_CFG_FHEC		(1<<GRTM_CFG_FHEC_BIT)
#define GRTM_CFG_AOS		(1<<GRTM_CFG_AOS_BIT)
#define GRTM_CFG_CIF		(1<<GRTM_CFG_CIF_BIT)
#define GRTM_CFG_OCFB		(1<<GRTM_CFG_OCFB_BIT)

/* TM Size Register (0x8c) */
#define GRTM_SIZE_BLKSZ_BIT	0
#define GRTM_SIZE_FIFOSZ_BIT	8
#define GRTM_SIZE_LEN_BIT	20

#define GRTM_SIZE_BLKSZ		(0xff<<GRTM_SIZE_BLKSZ_BIT)
#define GRTM_SIZE_FIFOSZ	(0xfff<<GRTM_SIZE_FIFOSZ_BIT)
#define GRTM_SIZE_LEN		(0xfff<<GRTM_SIZE_LEN_BIT)

/* TM Physical Layer Register (0x90) */
#define GRTM_PHY_SUB_BIT	0
#define GRTM_PHY_SCF_BIT	15
#define GRTM_PHY_SYM_BIT	16
#define GRTM_PHY_SF_BIT		31

#define GRTM_PHY_SUB		(0x7fff<<GRTM_PHY_SUB_BIT)
#define GRTM_PHY_SCF		(1<<GRTM_PHY_SCF_BIT)
#define GRTM_PHY_SYM		(0x7fff<<GRTM_PHY_SYM_BIT)
#define GRTM_PHY_SF		(1<<GRTM_PHY_SF_BIT)

/* TM Coding Sub-Layer Register (0x94) */
#define GRTM_CODE_SC_BIT	0
#define GRTM_CODE_SP_BIT	1
#define GRTM_CODE_CERATE_BIT	2
#define GRTM_CODE_CE_BIT	5
#define GRTM_CODE_NRZ_BIT	6
#define GRTM_CODE_PSR_BIT	7
#define GRTM_CODE_RS8_BIT	11
#define GRTM_CODE_RSDEP_BIT	12
#define GRTM_CODE_RS_BIT	15
#define GRTM_CODE_AASM_BIT	16
#define GRTM_CODE_CSEL_BIT	17

#define GRTM_CODE_SC		(1<<GRTM_CODE_SC_BIT)
#define GRTM_CODE_SP		(1<<GRTM_CODE_SP_BIT)
#define GRTM_CODE_CERATE	(0x7<<GRTM_CODE_CERATE_BIT)
#define GRTM_CODE_CE		(1<<GRTM_CODE_CE_BIT)
#define GRTM_CODE_NRZ		(1<<GRTM_CODE_NRZ_BIT)
#define GRTM_CODE_PSR		(1<<GRTM_CODE_PSR_BIT)
#define GRTM_CODE_RS8		(1<<GRTM_CODE_RS8_BIT)
#define GRTM_CODE_RSDEP		(0x7<<GRTM_CODE_RSDEP_BIT)
#define GRTM_CODE_RS		(1<<GRTM_CODE_RS_BIT)
#define GRTM_CODE_AASM		(1<<GRTM_CODE_AASM_BIT)
#define GRTM_CODE_CSEL		(0x3<<GRTM_CODE_CSEL_BIT)

/* TM Attached Synchronization Marker Register (0x98) */
#define GRTM_ASM_BIT		0

#define GRTM_ASM		0xffffffff

/* TM All Frames Generation Register (0xa0) */
#define GRTM_ALL_LEN_BIT	0
#define GRTM_ALL_VER_BIT	12
#define GRTM_ALL_FHEC_BIT	14
#define GRTM_ALL_FECF_BIT	15
#define GRTM_ALL_IZ_BIT		16
#define GRTM_ALL_IZLEN_BIT	17

#define GRTM_ALL_LEN		(0x7ff<<GRTM_ALL_LEN_BIT)
#define GRTM_ALL_VER		(0x3<<GRTM_ALL_VER_BIT)
#define GRTM_ALL_FHEC		(1<<GRTM_ALL_FHEC_BIT)
#define GRTM_ALL_FECF		(1<<GRTM_ALL_FECF_BIT)
#define GRTM_ALL_IZ		(1<<GRTM_ALL_IZ_BIT)
#define GRTM_ALL_IZLEN		(0x1f<<GRTM_ALL_IZLEN_BIT)

/* TM Master Channel Frame Generation Register (0xa4) */
#define GRTM_MST_OW_BIT		0
#define GRTM_MST_OCF_BIT	1
#define GRTM_MST_FSH_BIT	2
#define GRTM_MST_MC_BIT		3
#define GRTM_MST_MCCNTR_BIT	24

#define GRTM_MST_OW		(1<<GRTM_MST_OW_BIT)
#define GRTM_MST_OCF		(1<<GRTM_MST_OCF_BIT)
#define GRTM_MST_FSH		(1<<GRTM_MST_FSH_BIT)
#define GRTM_MST_MC		(0xff<<GRTM_MST_MC_BIT)

/* TM Idle Frame Generation Register (0xa8) */
#define GRTM_IDLE_SCID_BIT	0
#define GRTM_IDLE_VCID_BIT	10
#define GRTM_IDLE_MC_BIT	16
#define GRTM_IDLE_VCC_BIT	17
#define GRTM_IDLE_FSH_BIT	18
#define GRTM_IDLE_EVC_BIT	19
#define GRTM_IDLE_OCF_BIT	20
#define GRTM_IDLE_IDLE_BIT	21
#define GRTM_IDLE_MCCNTR_BIT	24

#define GRTM_IDLE_SCID		(0x3ff<<GRTM_IDLE_SCID_BIT)
#define GRTM_IDLE_VCID		(0x3f<<GRTM_IDLE_VCID_BIT)
#define GRTM_IDLE_MC		(1<<GRTM_IDLE_MC_BIT)
#define GRTM_IDLE_VCC		(1<<GRTM_IDLE_VCC_BIT)
#define GRTM_IDLE_FSH		(1<<GRTM_IDLE_FSH_BIT)
#define GRTM_IDLE_EVC		(1<<GRTM_IDLE_EVC_BIT)
#define GRTM_IDLE_OCF		(1<<GRTM_IDLE_OCF_BIT)
#define GRTM_IDLE_IDLE		(1<<GRTM_IDLE_IDLE_BIT)
#define GRTM_IDLE_MCCNTR	(0xff<<GRTM_IDLE_MCCNTR_BIT)

/* TM FSH/Insert Zone Registers (0xc0..0xcc) */
#define GRTM_FSH_DATA_BIT	0

#define GRTM_FSH_DATA		0xffffffff


/* TM Operational Control Field Register (0xd0) */
#define GRTM_OCF_CLCW_BIT	0

#define GRTM_OCF_CLCW		0xffffffff


/* GRTM Revision 0 */
#define GRTM_REV0_DMA_CTRL_TXRDY_BIT	5
#define GRTM_REV0_DMA_CTRL_TXRDY	(1<<GRTM_REV0_DMA_CTRL_TXRDY_BIT)

/* GRTM Revision 1 */
#define GRTM_REV1_DMA_STS_TXRDY_BIT	6
#define GRTM_REV1_DMA_STS_TXSTAT_BIT	7
#define GRTM_REV1_DMA_STS_TXRDY		(1<<GRTM_REV1_DMA_STS_TXRDY_BIT)
#define GRTM_REV1_DMA_STS_TXSTAT	(1<<GRTM_REV1_DMA_STS_TXSTAT_BIT)

#define GRTM_REV1_REV_SREV_BIT		0
#define GRTM_REV1_REV_MREV_BIT		8
#define GRTM_REV1_REV_TIRQ_BIT		16
#define GRTM_REV1_REV_SREV		(0xff<<GRTM_REV1_REV_SREV_BIT)
#define GRTM_REV1_REV_MREV		(0xff<<GRTM_REV1_REV_MREV_BIT)
#define GRTM_REV1_REV_TIRQ		(1<<GRTM_REV1_REV_TIRQ_BIT)


/* GRTM transmit descriptor (0x400 Alignment need) */
struct grtm_bd {
	volatile unsigned int	ctrl;
	unsigned int		address;
};

#define GRTM_BD_EN_BIT		0
#define GRTM_BD_WR_BIT		1
#define GRTM_BD_IE_BIT		2
#define GRTM_BD_FECFB_BIT	3
#define GRTM_BD_IZB_BIT		4
#define GRTM_BD_FHECB_BIT	5
#define GRTM_BD_OCFB_BIT	6
#define GRTM_BD_FSHB_BIT	7
#define GRTM_BD_MCB_BIT		8
#define GRTM_BD_VCE_BIT		9
#define GRTM_BD_TS_BIT		14
#define GRTM_BD_UE_BIT		15

#define GRTM_BD_EN		(1<<GRTM_BD_EN_BIT)
#define GRTM_BD_WR		(1<<GRTM_BD_WR_BIT)
#define GRTM_BD_IE		(1<<GRTM_BD_IE_BIT)
#define GRTM_BD_FECFB		(1<<GRTM_BD_FECFB_BIT)
#define GRTM_BD_IZB		(1<<GRTM_BD_IZB_BIT)
#define GRTM_BD_FHECB		(1<<GRTM_BD_FHECB_BIT)
#define GRTM_BD_OCFB		(1<<GRTM_BD_OCFB_BIT)
#define GRTM_BD_FSHB		(1<<GRTM_BD_FSHB_BIT)
#define GRTM_BD_MCB		(1<<GRTM_BD_MCB_BIT)
#define GRTM_BD_VCE		(1<<GRTM_BD_VCE_BIT)
#define GRTM_BD_TS		(1<<GRTM_BD_TS_BIT)
#define GRTM_BD_UE		(1<<GRTM_BD_UE_BIT)

/* Load register */

#define READ_REG(address)	(*(volatile unsigned int *)address)

/* Driver functions */
static rtems_device_driver grtm_initialize(rtems_device_major_number  major, rtems_device_minor_number  minor, void *arg);
static rtems_device_driver grtm_open(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grtm_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grtm_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grtm_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);
static rtems_device_driver grtm_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg);

#define GRTM_DRIVER_TABLE_ENTRY { grtm_initialize, grtm_open, grtm_close, grtm_read, grtm_write, grtm_ioctl }

static rtems_driver_address_table grtm_driver = GRTM_DRIVER_TABLE_ENTRY;

/* Structure that connects BD with SoftWare Frame */
struct grtm_ring {
	struct grtm_ring	*next;
	struct grtm_bd		*bd;
	struct grtm_frame	*frm;
};

struct grtm_priv {
	struct drvmgr_dev	*dev;		/* Driver manager device */
	char			devName[32];	/* Device Name */
	struct grtm_regs	*regs;
	int			irq;
	int			minor;
	int			subrev;		/* GRTM Revision */

	int			open;
	int			running;

	struct grtm_bd		*bds;
	void			*_bds;

	/* Interrupt generation */
	int			enable_cnt_curr;/* Down counter, when 0 the interrupt bit is set for next descriptor */
	volatile int		handling_transmission;	/* Tells ISR if user are active changing descriptors/queues */

	struct grtm_ring 	*_ring;		/* Root of ring */
	struct grtm_ring 	*ring;		/* Next ring to use for new frames to be transmitted */
	struct grtm_ring 	*ring_end;	/* Oldest activated ring used */

	/* Collections of frames Ready to sent/ Scheduled for transmission/Sent 
	 * frames waiting for the user to reclaim 
	 */
	struct grtm_list	ready;		/* Frames Waiting for free BDs */
	struct grtm_list	scheduled;	/* Frames in BDs beeing transmitted */
	struct grtm_list	sent;		/* Sent Frames waiting for user to reclaim and reuse */

	/* Number of frames in the lists */
	int			ready_cnt;	/* Number of ready frames */
	int			scheduled_cnt;	/* Number of scheduled frames */
	int			sent_cnt;	/* Number of sent frames */

	struct grtm_ioc_hw	hw_avail;	/* Hardware support available */
	struct grtm_ioc_config	config;
	struct grtm_ioc_stats	stats;

	rtems_id		sem_tx;
};

/* Prototypes */
static void *grtm_memalign(unsigned int boundary, unsigned int length, void *realbuf);
static void grtm_hw_reset(struct grtm_priv *pDev);
static void grtm_interrupt(void *arg);

/* Common Global Variables */
static rtems_id grtm_dev_sem;
static int grtm_driver_io_registered = 0;
static rtems_device_major_number grtm_driver_io_major = 0;

/******************* Driver manager interface ***********************/

/* Driver prototypes */
static int grtm_register_io(rtems_device_major_number *m);
static int grtm_device_init(struct grtm_priv *pDev);

static int grtm_init2(struct drvmgr_dev *dev);
static int grtm_init3(struct drvmgr_dev *dev);

static struct drvmgr_drv_ops grtm_ops = 
{
	{NULL, grtm_init2, grtm_init3, NULL},
	NULL,
	NULL
};

static struct amba_dev_id grtm_ids[] = 
{
	{VENDOR_GAISLER, GAISLER_GRTM},
	{0, 0}		/* Mark end of table */
};

static struct amba_drv_info grtm_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_GRTM_ID,	/* Driver ID */
		"GRTM_DRV",			/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&grtm_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		0,
	},
	&grtm_ids[0]
};

void grtm_register_drv (void)
{
	DBG("Registering GRTM driver\n");
	drvmgr_drv_register(&grtm_drv_info.general);
}

static int grtm_init2(struct drvmgr_dev *dev)
{
	struct grtm_priv *priv;

	DBG("GRTM[%d] on bus %s\n", dev->minor_drv, dev->parent->dev->name);
	priv = dev->priv = malloc(sizeof(struct grtm_priv));
	if ( !priv )
		return DRVMGR_NOMEM;
	memset(priv, 0, sizeof(*priv));
	priv->dev = dev;

	/* This core will not find other cores, so we wait for init2() */

	return DRVMGR_OK;
}

static int grtm_init3(struct drvmgr_dev *dev)
{
	struct grtm_priv *priv;
	char prefix[32];
	rtems_status_code status;

	priv = dev->priv;

	/* Do initialization */

	if ( grtm_driver_io_registered == 0) {
		/* Register the I/O driver only once for all cores */
		if ( grtm_register_io(&grtm_driver_io_major) ) {
			/* Failed to register I/O driver */
			dev->priv = NULL;
			return DRVMGR_FAIL;
		}

		grtm_driver_io_registered = 1;
	}

	/* I/O system registered and initialized 
	 * Now we take care of device initialization.
	 */
	if ( grtm_device_init(priv) ) {
		return DRVMGR_FAIL;
	}

	/* Get Filesystem name prefix */
	prefix[0] = '\0';
	if ( drvmgr_get_dev_prefix(dev, prefix) ) {
		/* Failed to get prefix, make sure of a unique FS name
		 * by using the driver minor.
		 */
		sprintf(priv->devName, "/dev/grtm%d", dev->minor_drv);
	} else {
		/* Got special prefix, this means we have a bus prefix
		 * And we should use our "bus minor"
		 */
		sprintf(priv->devName, "/dev/%sgrtm%d", prefix, dev->minor_bus);
	}

	/* Register Device */
	status = rtems_io_register_name(priv->devName, grtm_driver_io_major, dev->minor_drv);
	if (status != RTEMS_SUCCESSFUL) {
		return DRVMGR_FAIL;
	}

	return DRVMGR_OK;
}

/******************* Driver Implementation ***********************/

static int grtm_register_io(rtems_device_major_number *m)
{
	rtems_status_code r;

	if ((r = rtems_io_register_driver(0, &grtm_driver, m)) == RTEMS_SUCCESSFUL) {
		DBG("GRTM driver successfully registered, major: %d\n", *m);
	} else {
		switch(r) {
		case RTEMS_TOO_MANY:
			printk("GRTM rtems_io_register_driver failed: RTEMS_TOO_MANY\n");
			return -1;
		case RTEMS_INVALID_NUMBER:  
			printk("GRTM rtems_io_register_driver failed: RTEMS_INVALID_NUMBER\n");
			return -1;
		case RTEMS_RESOURCE_IN_USE:
			printk("GRTM rtems_io_register_driver failed: RTEMS_RESOURCE_IN_USE\n");
			return -1;
		default:
			printk("GRTM rtems_io_register_driver failed\n");
			return -1;
		}
	}
	return 0;
}

static int grtm_device_init(struct grtm_priv *pDev)
{
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	union drvmgr_key_value *value;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)pDev->dev->businfo;
	if ( ambadev == NULL ) {
		return -1;
	}
	pnpinfo = &ambadev->info;
	pDev->irq = pnpinfo->irq;
	pDev->regs = (struct grtm_regs *)pnpinfo->apb_slv->start;
	pDev->minor = pDev->dev->minor_drv;
	pDev->open = 0;
	pDev->running = 0;

	/* Create Binary RX Semaphore with count = 0 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'R', 'M', '0' + pDev->minor),
		0,
		RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|RTEMS_NO_INHERIT_PRIORITY|\
		RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING, 
		0,
		&pDev->sem_tx) != RTEMS_SUCCESSFUL ) {
		return -1;
	}

	/* Allocate Memory for Buffer Descriptor Table, or let user provide a custom
	 * address.
	 */
	value = drvmgr_dev_key_get(pDev->dev, "bdTabAdr", DRVMGR_KT_POINTER);
	if ( value ) {
		pDev->bds = (struct grtm_bd *)value->ptr;
		pDev->_bds = (void *)value->ptr;	
	} else {
		pDev->bds = (struct grtm_bd *)grtm_memalign(0x400, 0x400, &pDev->_bds);
	}
	if ( !pDev->bds ) {
		DBG("GRTM: Failed to allocate descriptor table\n");
		return -1;
	}
	memset(pDev->bds, 0, 0x400);

	pDev->_ring = malloc(sizeof(struct grtm_ring) * 128);
	if ( !pDev->_ring ) {
		return -1;
	}

	/* Reset Hardware before attaching IRQ handler */
	grtm_hw_reset(pDev);

	/* Read SUB revision number, ignore  */
	pDev->subrev = (READ_REG(&pDev->regs->revision) & GRTM_REV1_REV_SREV)
			>> GRTM_REV1_REV_SREV_BIT;

	return 0;
}


static inline void grtm_list_clr(struct grtm_list *list)
{
	list->head = NULL;
	list->tail = NULL;
}

static void grtm_hw_reset(struct grtm_priv *pDev)
{
	/* Reset Core */
	pDev->regs->dma_ctrl = GRTM_DMA_CTRL_RST;
}

static void grtm_hw_get_implementation(struct grtm_priv *pDev, struct grtm_ioc_hw *hwcfg)
{
	unsigned int cfg = READ_REG(&pDev->regs->cfg);

	hwcfg->cs	= (cfg & GRTM_CFG_SC)	? 1:0;
	hwcfg->sp	= (cfg & GRTM_CFG_SP)	? 1:0;
	hwcfg->ce	= (cfg & GRTM_CFG_CE)	? 1:0;
	hwcfg->nrz	= (cfg & GRTM_CFG_NRZ)	? 1:0;
	hwcfg->psr	= (cfg & GRTM_CFG_PSR)	? 1:0;
	hwcfg->te	= (cfg & GRTM_CFG_TE)	? 1:0;
	hwcfg->rsdep	= (cfg & GRTM_CFG_RSDEP)>>GRTM_CFG_RSDEP_BIT;
	hwcfg->rs	= (cfg & GRTM_CFG_RS)>>GRTM_CFG_RS_BIT;
	hwcfg->aasm	= (cfg & GRTM_CFG_AASM)	? 1:0;
	hwcfg->fecf	= (cfg & GRTM_CFG_FECF)	? 1:0;
	hwcfg->ocf	= (cfg & GRTM_CFG_OCF)	? 1:0;
	hwcfg->evc	= (cfg & GRTM_CFG_EVC)	? 1:0;
	hwcfg->idle	= (cfg & GRTM_CFG_IDLE)	? 1:0;
	hwcfg->fsh	= (cfg & GRTM_CFG_FSH)	? 1:0;
	hwcfg->mcg	= (cfg & GRTM_CFG_MCG)	? 1:0;
	hwcfg->iz	= (cfg & GRTM_CFG_IZ)	? 1:0;
	hwcfg->fhec	= (cfg & GRTM_CFG_FHEC)	? 1:0;
	hwcfg->aos	= (cfg & GRTM_CFG_AOS)	? 1:0;
	hwcfg->cif	= (cfg & GRTM_CFG_CIF)	? 1:0;
	hwcfg->ocfb	= (cfg & GRTM_CFG_OCFB)	? 1:0;

	cfg = READ_REG(&pDev->regs->dma_cfg);
	hwcfg->blk_size	= (cfg & GRTM_DMA_CFG_BLKSZ) >> GRTM_DMA_CFG_BLKSZ_BIT;
	hwcfg->fifo_size= (cfg & GRTM_DMA_CFG_FIFOSZ) >> GRTM_DMA_CFG_FIFOSZ_BIT;
}


/* TODO: Implement proper default calculation from hardware configuration */
static void grtm_hw_get_default_modes(struct grtm_ioc_config *cfg, struct grtm_ioc_hw *hwcfg)
{
	cfg->mode = GRTM_MODE_TM;
	cfg->frame_length = 223;
	cfg->limit = 0; /* Make driver auto configure it on START, user may override with non-zero value */
	cfg->as_marker = 0x1ACFFC1D;

	/* Physical */
	cfg->phy_subrate = 1;
	cfg->phy_symbolrate = 1;
	cfg->phy_opts = 0;

	/* Coding Layer */
	cfg->code_rsdep = 1;
	cfg->code_ce_rate = 0;
	cfg->code_csel = 0;
	cfg->code_opts = 0;

	/* All Frame Generation */
	cfg->all_izlen = 0;
	cfg->all_opts = GRTM_IOC_ALL_FECF;

	/* Master Channel Frame Generation */
	if ( hwcfg->mcg ) {
		cfg->mf_opts = GRTM_IOC_MF_MC;
	} else {
		cfg->mf_opts = 0;
	}

	/* Idle Frame Generation */
	cfg->idle_scid = 0;
	cfg->idle_vcid = 0;
	if ( hwcfg->idle ) {
		cfg->idle_opts = GRTM_IOC_IDLE_EN;
	} else {
		cfg->idle_opts = 0;
	}

	/* Interrupt options */
	cfg->blocking = 0;	/* non-blocking mode is default */
	cfg->enable_cnt = 16;	/* generate interrupt every 16 descriptor */
	cfg->isr_desc_proc = 1;	/* Let interrupt handler do descriptor processing */
	cfg->timeout = RTEMS_NO_TIMEOUT;
	
}

static void *grtm_memalign(unsigned int boundary, unsigned int length, void *realbuf)
{
	*(int *)realbuf = (int)malloc(length+boundary);
	DBG("GRTM: Alloced %d (0x%x) bytes, requested: %d\n",length+boundary,length+boundary,length);
	return (void *)(((*(unsigned int *)realbuf)+boundary) & ~(boundary-1));
}

static int grtm_hw_set_config(struct grtm_priv *pDev, struct grtm_ioc_config *cfg, struct grtm_ioc_hw *hwcfg)
{
	struct grtm_regs *regs = pDev->regs;
	unsigned int tmp;
	unsigned int limit;

	if ( cfg->limit == 0 ) {
		/* Calculate Limit */
		if ( cfg->frame_length > hwcfg->blk_size ) {
			limit = hwcfg->blk_size*2;
		} else {
			limit = cfg->frame_length;
		}
	} else {
		/* Use user configured limit */
		limit = cfg->limit;
	}

	/* Frame Length and Limit */
	regs->dma_len =	(((limit-1) << GRTM_DMA_LEN_LIM_BIT) & GRTM_DMA_LEN_LIM)|
			(((cfg->frame_length-1) << GRTM_DMA_LEN_LEN_BIT) & GRTM_DMA_LEN_LEN);

	/* Physical layer options */
	tmp =	(cfg->phy_opts & (GRTM_IOC_PHY_SCF|GRTM_IOC_PHY_SF)) | 
		(((cfg->phy_symbolrate-1)<<GRTM_PHY_SYM_BIT) & GRTM_PHY_SYM) | (((cfg->phy_subrate-1)<<GRTM_PHY_SUB_BIT) & GRTM_PHY_SUB);
	regs->phy = tmp;

	/* Coding Sub-layer Options */
	tmp =	(cfg->code_opts & GRTM_IOC_CODE_ALL) | ((cfg->code_csel<<GRTM_CODE_CSEL_BIT) & GRTM_CODE_CSEL) |
		(((cfg->code_rsdep-1)<<GRTM_CODE_RSDEP_BIT) & GRTM_CODE_RSDEP) | ((cfg->code_ce_rate<<GRTM_CODE_CERATE_BIT) & GRTM_CODE_CERATE);
	regs->code = tmp;

	/* Attached synchronization marker register */
	regs->asmr = cfg->as_marker;

	/* All Frames Generation */
	tmp =	((cfg->all_opts & GRTM_IOC_ALL_ALL)<<14) | 
		((cfg->all_izlen<<GRTM_ALL_IZLEN_BIT) & GRTM_ALL_IZLEN) |
		((cfg->mode<<GRTM_ALL_VER_BIT) & GRTM_ALL_VER);
	regs->all_frm = tmp;

	/* Master Frame Generation */
	regs->mst_frm = cfg->mf_opts & GRTM_IOC_MF_ALL;

	/* Idle frame Generation */
	tmp =	((cfg->idle_opts & GRTM_IOC_IDLE_ALL) << 16) |
		((cfg->idle_vcid << GRTM_IDLE_VCID_BIT) & GRTM_IDLE_VCID) |
		((cfg->idle_scid << GRTM_IDLE_SCID_BIT) & GRTM_IDLE_SCID);
	regs->idle_frm = tmp;

	return 0;
}

static int grtm_start(struct grtm_priv *pDev)
{
	struct grtm_regs *regs = pDev->regs;
	int i;
	struct grtm_ioc_config *cfg = &pDev->config;
	unsigned int txrdy;

	/* Clear Descriptors */
	memset(pDev->bds,0,0x400);
	
	/* Clear stats */
	memset(&pDev->stats,0,sizeof(struct grtm_ioc_stats));
	
	/* Init Descriptor Ring */
	memset(pDev->_ring,0,sizeof(struct grtm_ring)*128);
	for(i=0;i<127;i++){
		pDev->_ring[i].next = &pDev->_ring[i+1];
		pDev->_ring[i].bd = &pDev->bds[i];
		pDev->_ring[i].frm = NULL;
	}
	pDev->_ring[127].next = &pDev->_ring[0];
	pDev->_ring[127].bd = &pDev->bds[127];
	pDev->_ring[127].frm = NULL;

	pDev->ring = &pDev->_ring[0];
	pDev->ring_end = &pDev->_ring[0];

	/* Clear Scheduled, Ready and Sent list */
	grtm_list_clr(&pDev->ready);
	grtm_list_clr(&pDev->scheduled);
	grtm_list_clr(&pDev->sent);

	/* Software init */
	pDev->handling_transmission = 0;
	
	/* Reset the transmitter */
	regs->dma_ctrl = GRTM_DMA_CTRL_TXRST;
	regs->dma_ctrl = 0;	/* Leave Reset */

	/* Clear old interrupts */
	regs->dma_status = GRTM_DMA_STS_ALL;

	/* Set Descriptor Pointer Base register to point to first descriptor */
	drvmgr_translate_check(pDev->dev, CPUMEM_TO_DMA, (void *)pDev->bds,
				(void **)&regs->dma_bd, 0x400);

	/* Set hardware options as defined by config */
	if ( grtm_hw_set_config(pDev, cfg, &pDev->hw_avail) ) {
		return RTEMS_IO_ERROR;
	}

	/* Enable TM Transmitter */
	regs->ctrl = GRTM_CTRL_EN;

	/* Wait for TXRDY to be cleared */
	i=1000;
	while( i > 0 ) {
		asm volatile ("nop"::);
		i--;
	}

	/* Check transmitter startup OK */
	i = 1000000;
	do {
		/* Location of TXRDY Bit is different for different revisions */
		if ( pDev->subrev == 0 ) {
			txrdy = READ_REG(&regs->dma_ctrl) &
				GRTM_REV0_DMA_CTRL_TXRDY;
		} else {
			txrdy = READ_REG(&regs->dma_status) &
				GRTM_REV1_DMA_STS_TXRDY;
		}
		if (txrdy != 0)
			break;

		asm volatile ("nop"::);
	} while ( --i > 0 );
	if ( i == 0 ) {
		/* Reset Failed */
		DBG("GRTM: start: Reseting transmitter failed (%d)\n",i);
		return RTEMS_IO_ERROR;
	}
	DBG("GRTM: reset time %d\n",i);

	/* Everything is configured, the TM transmitter is started
	 * and idle frames has been sent.
	 */

	/* Mark running before enabling the DMA transmitter */
	pDev->running = 1;

	/* Enable interrupts (Error and DMA TX) */
	regs->dma_ctrl = GRTM_DMA_CTRL_IE;

	DBG("GRTM: STARTED\n");

	return RTEMS_SUCCESSFUL;
}

static void grtm_stop(struct grtm_priv *pDev)
{
	struct grtm_regs *regs = pDev->regs;

	/* Disable the transmitter & Interrupts */
	regs->dma_ctrl = 0;
	
	/* Clear any pending interrupt  */
	regs->dma_status = GRTM_DMA_STS_ALL;

	DBG("GRTM: STOPPED\n");

	/* Flush semaphore in case a thread is stuck waiting for TX Interrupts */
	rtems_semaphore_flush(pDev->sem_tx);
}

static rtems_device_driver grtm_open(
	rtems_device_major_number major, 
	rtems_device_minor_number minor, 
	void *arg)
{
	struct grtm_priv *pDev;
	struct drvmgr_dev *dev;

	FUNCDBG();

	if ( drvmgr_get_dev(&grtm_drv_info.general, minor, &dev) ) {
		DBG("Wrong minor %d\n", minor);
		return RTEMS_INVALID_NUMBER;
	}
	pDev = (struct grtm_priv *)dev->priv;
	
	/* Wait until we get semaphore */
	if ( rtems_semaphore_obtain(grtm_dev_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT) != RTEMS_SUCCESSFUL ){
		return RTEMS_INTERNAL_ERROR;
	}

	/* Is device in use? */
	if ( pDev->open ){
		rtems_semaphore_release(grtm_dev_sem);
		return RTEMS_RESOURCE_IN_USE;
	}
	
	/* Mark device taken */
	pDev->open = 1;
	
	rtems_semaphore_release(grtm_dev_sem);
	
	DBG("grtm_open: OPENED minor %d (pDev: 0x%x)\n",pDev->minor,(unsigned int)pDev);
	
	/* Set defaults */
	pDev->config.timeout = RTEMS_NO_TIMEOUT;	/* no timeout (wait forever) */
	pDev->config.blocking = 0;			/* polling mode */
	
	pDev->running = 0;				/* not in running mode yet */

	memset(&pDev->config,0,sizeof(pDev->config));
	
	/* The core has been reset when we execute here, so it is possible
	 * to read out what HW is implemented from core.
	 */
	grtm_hw_get_implementation(pDev, &pDev->hw_avail);

	/* Get default modes */
	grtm_hw_get_default_modes(&pDev->config,&pDev->hw_avail);
	
	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grtm_close(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct grtm_priv *pDev;
	struct drvmgr_dev *dev;

	FUNCDBG();

	if ( drvmgr_get_dev(&grtm_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NUMBER;
	}
	pDev = (struct grtm_priv *)dev->priv;

	if ( pDev->running ){
		grtm_stop(pDev);
		pDev->running = 0;
	}
	
	/* Reset core */
	grtm_hw_reset(pDev);

	/* Clear descriptor area just for sure */
	memset(pDev->bds, 0, 0x400);
	
	/* Mark not open */
	pDev->open = 0;

	return RTEMS_SUCCESSFUL;
}

static rtems_device_driver grtm_read(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	FUNCDBG();
	return RTEMS_NOT_IMPLEMENTED;
}

static rtems_device_driver grtm_write(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	FUNCDBG();
	return RTEMS_NOT_IMPLEMENTED;
}

/* Scans the desciptor table for scheduled frames that has been sent, 
 * and moves these frames from the head of the scheduled queue to the
 * tail of the sent queue.
 *
 * Also, for all frames the status is updated.
 *
 * Return Value
 * Number of frames freed.
 */
static int grtm_free_sent(struct grtm_priv *pDev)
{
	struct grtm_ring *curr;
	struct grtm_frame *last_frm, *first_frm;
	int freed_frame_cnt=0;
	unsigned int ctrl;

	curr = pDev->ring_end;

	/* Step into TX ring to find sent frames */
	if ( !curr->frm ){
		/* No scheduled frames, abort */
		return 0;
	}

	/* There has been messages scheduled ==> scheduled messages may have been
	 * transmitted and needs to be collected.
	 */

	first_frm = curr->frm;

	/* Loop until first enabled unsent frame is found. 
	 * A unused descriptor is indicated by an unassigned frm field
	 */
	while ( curr->frm && !((ctrl=READ_REG(&curr->bd->ctrl)) & GRTM_BD_EN) ){
		/* Handle one sent Frame */
		
		/* Remember last handled frame so that insertion/removal from
		 * frames lists go fast.
		 */
		last_frm = curr->frm;
		
		/* 1. Set flags to indicate error(s) and other information */
		last_frm->flags |= GRTM_FLAGS_SENT; /* Mark sent */
		
		/* Update Stats */
		pDev->stats.frames_sent++;
    
		/* Did packet encounter link error? */
		if ( ctrl & GRTM_BD_UE ) {
			pDev->stats.err_underrun++;
			last_frm->flags |= GRRM_FLAGS_ERR;
		}

		curr->frm = NULL; /* Mark unused */

		/* Increment */
		curr = curr->next;
		freed_frame_cnt++;
	}

	/* 1. Remove all handled frames from scheduled queue
	 * 2. Put all handled frames into sent queue
	 */
	if ( freed_frame_cnt > 0 ){

		/* Save TX ring posistion */
		pDev->ring_end = curr;

		/* Remove all sent frames from scheduled list */
		if ( pDev->scheduled.tail == last_frm ){
			/* All scheduled frames sent... */
			pDev->scheduled.head = NULL;
			pDev->scheduled.tail = NULL;
		}else{
			pDev->scheduled.head = last_frm->next;
		}
		last_frm->next = NULL;

		/* Put all sent frames into "Sent queue" for user to
		 * collect, later on.
		 */
		if ( !pDev->sent.head ){
			/* Sent queue empty */
			pDev->sent.head = first_frm;
			pDev->sent.tail = last_frm;
		}else{
			pDev->sent.tail->next = first_frm;
			pDev->sent.tail = last_frm;
		}
	}
	return freed_frame_cnt;
}


/* Moves as many frames in the ready queue (as there are free descriptors for)
 * to the scheduled queue. The free descriptors are then assigned one frame
 * each and enabled for transmission.
 * 
 * Return Value
 * Returns number of frames moved from ready to scheduled queue 
 */
static int grtm_schedule_ready(struct grtm_priv *pDev, int ints_off)
{
	int cnt;
	unsigned int ctrl, dmactrl;
	struct grtm_ring *curr_bd;
	struct grtm_frame *curr_frm, *last_frm;
	IRQ_GLOBAL_PREPARE(oldLevel);

	if ( !pDev->ready.head ){
		return 0;
	}

	cnt=0;
	curr_frm = pDev->ready.head;
	curr_bd = pDev->ring;
	while( !curr_bd->frm ){
		/* Assign frame to descriptor */
		curr_bd->frm = curr_frm;

		/* Prepare descriptor address. Three cases:
		 *  - GRTM core on same bus as CPU ==> no translation (Address used by CPU = address used by GRTM)
		 *  - GRTM core on remote bus, and payload address given as used by CPU ==> Translation needed
		 *  - GRTM core on remote bus, and payload address given as used by GRTM ==> no translation  [ USER does custom translation]
		 */
		if ( curr_frm->flags & (GRTM_FLAGS_TRANSLATE|GRTM_FLAGS_TRANSLATE_AND_REMEMBER) ) {
			/* Do translation */
			drvmgr_translate(pDev->dev, CPUMEM_TO_DMA, (void *)curr_frm->payload, (void **)&curr_bd->bd->address);
			if ( curr_frm->flags & GRTM_FLAGS_TRANSLATE_AND_REMEMBER ) {
				if ( curr_frm->payload != (unsigned int *)curr_bd->bd->address ) {
					/* Translation needed */
					curr_frm->flags &= ~GRTM_FLAGS_TRANSLATE_AND_REMEMBER;
					curr_frm->flags |= GRTM_FLAGS_TRANSLATE;
				} else {
					/* No Trnaslation needed */
					curr_frm->flags &= ~(GRTM_FLAGS_TRANSLATE|GRTM_FLAGS_TRANSLATE_AND_REMEMBER);
				}
			}
		} else {
			/* Custom translation or no translation needed */
			curr_bd->bd->address = (unsigned int)curr_frm->payload;
		}

		ctrl = GRTM_BD_EN;
		if ( curr_bd->next == pDev->_ring ){
			ctrl |= GRTM_BD_WR; /* Wrap around */
		}
		/* Apply user options/flags */
		ctrl |= (curr_frm->flags & GRTM_FLAGS_MASK);

		/* Is this Frame going to be an interrupt Frame? */
		if ( (--pDev->enable_cnt_curr) <= 0 ){
			if ( pDev->config.enable_cnt == 0 ){
				pDev->enable_cnt_curr = 0x3fffffff;
			}else{
				pDev->enable_cnt_curr = pDev->config.enable_cnt;
				ctrl |= GRTM_BD_IE;
			}
		}

		/* Enable descriptor */
		curr_bd->bd->ctrl = ctrl;

		last_frm = curr_frm;
		curr_bd = curr_bd->next;
		cnt++;
		
		/* Get Next Frame from Ready Queue */
		if ( curr_frm == pDev->ready.tail ){
			/* Handled all in ready queue. */
			curr_frm = NULL;
			break;
		}
		curr_frm = curr_frm->next;
	}
	
	/* Has frames have been scheduled? */
	if ( cnt > 0 ){
		/* Make last frame mark end of chain, probably pointless... */
		last_frm->next = NULL;

		/* Insert scheduled packets into scheduled queue */
		if ( !pDev->scheduled.head ){
			/* empty scheduled queue */
			pDev->scheduled.head = pDev->ready.head;
			pDev->scheduled.tail = last_frm;
		}else{
			pDev->scheduled.tail->next = pDev->ready.head;
			pDev->scheduled.tail = last_frm;
		}

		/* Remove scheduled packets from ready queue */
		pDev->ready.head = curr_frm;
		if ( !curr_frm ){
			pDev->ready.tail = NULL;
		}

		/* Update TX ring posistion */
		pDev->ring = curr_bd;
		if ( !ints_off ) {
			IRQ_GLOBAL_DISABLE(oldLevel);
		}

		/* Make hardware aware of the newly enabled descriptors */
		dmactrl = READ_REG(&pDev->regs->dma_ctrl);
		dmactrl &= ~(GRTM_DMA_CTRL_TXRST | GRTM_DMA_CTRL_RST);
		dmactrl |= GRTM_DMA_CTRL_EN;
		pDev->regs->dma_ctrl = dmactrl;
		
		if ( !ints_off ) {
			IRQ_GLOBAL_ENABLE(oldLevel);
		}
	}
	return cnt;
}


static rtems_device_driver grtm_ioctl(rtems_device_major_number major, rtems_device_minor_number minor, void *arg)
{
	struct grtm_priv *pDev;
	struct drvmgr_dev *dev;
	rtems_libio_ioctl_args_t *ioarg = (rtems_libio_ioctl_args_t *)arg;
	unsigned int *data;
	int status;
	struct grtm_ioc_config *cfg;
	struct grtm_ioc_hw_status *hwregs;
	IRQ_GLOBAL_PREPARE(oldLevel);
	struct grtm_list *chain;
	struct grtm_frame *curr;
	struct grtm_ioc_hw *hwimpl;
	struct grtm_ioc_stats *stats;
	int num,ret;

	FUNCDBG();

	if ( drvmgr_get_dev(&grtm_drv_info.general, minor, &dev) ) {
		return RTEMS_INVALID_NUMBER;
	}
	pDev = (struct grtm_priv *)dev->priv;

	if (!ioarg)
		return RTEMS_INVALID_NAME;

	data = ioarg->buffer;
	ioarg->ioctl_return = 0;
	switch(ioarg->command) {
		case GRTM_IOC_START:
		if ( pDev->running ) {
			return RTEMS_RESOURCE_IN_USE; /* EBUSY */
		}
		if ( (status=grtm_start(pDev)) != RTEMS_SUCCESSFUL ){
			return status;
		}
		/* Register ISR & Enable interrupt */
		drvmgr_interrupt_register(dev, 0, "grtm", grtm_interrupt, pDev);

		/* Read and write are now open... */
		break;

		case GRTM_IOC_STOP:
		if ( !pDev->running ) {
			return RTEMS_RESOURCE_IN_USE;
		}

		/* Disable interrupts */
		drvmgr_interrupt_unregister(dev, 0, grtm_interrupt, pDev);
		grtm_stop(pDev);
		pDev->running = 0;
		break;

		case GRTM_IOC_ISSTARTED:
		if ( !pDev->running ) {
			return RTEMS_RESOURCE_IN_USE;
		}
		break;

		case GRTM_IOC_SET_BLOCKING_MODE:
		if ( (unsigned int)data > GRTM_BLKMODE_BLK ) {
			return RTEMS_INVALID_NAME;
		}
		DBG("GRTM: Set blocking mode: %d\n",(unsigned int)data);
		pDev->config.blocking = (unsigned int)data;
		break;

		case GRTM_IOC_SET_TIMEOUT:
		DBG("GRTM: Timeout: %d\n",(unsigned int)data);
		pDev->config.timeout = (rtems_interval)data;
		break;

		case GRTM_IOC_SET_CONFIG:
		cfg = (struct grtm_ioc_config *)data;
		if ( !cfg ) {
			return RTEMS_INVALID_NAME;
		}
		
		if ( pDev->running ) {
			return RTEMS_RESOURCE_IN_USE;
		}

		pDev->config = *cfg;
		break;

		case GRTM_IOC_GET_STATS:
		stats = (struct grtm_ioc_stats *)data;
		if ( !stats ) {
			return RTEMS_INVALID_NAME;
		}
		memcpy(stats,&pDev->stats,sizeof(struct grtm_ioc_stats));
		break;

		case GRTM_IOC_CLR_STATS:
		memset(&pDev->stats,0,sizeof(struct grtm_ioc_stats));
		break;

		case GRTM_IOC_GET_CONFIG:
		cfg = (struct grtm_ioc_config *)data;
		if ( !cfg ) {
			return RTEMS_INVALID_NAME;
		}

		*cfg = pDev->config;
		break;

		case GRTM_IOC_GET_OCFREG:
		if ( !pDev->hw_avail.ocf ) {
			/* Hardware does not implement the OCF register */
			return RTEMS_NOT_DEFINED;
		}
		if ( !data ) {
			return RTEMS_INVALID_NAME;
		}
		*(unsigned int **)data = (unsigned int *)&pDev->regs->ocf;
		break;

		case GRTM_IOC_GET_HW_IMPL:
		hwimpl = (struct grtm_ioc_hw *)data;
		if ( !hwimpl ) {
			return RTEMS_INVALID_NAME;
		}
		*hwimpl = pDev->hw_avail;
		break;

		case GRTM_IOC_GET_HW_STATUS:
		hwregs = (struct grtm_ioc_hw_status *)data;
		if ( !hwregs ) {
			return RTEMS_INVALID_NAME;
		}
		/* We disable interrupt in order to get a snapshot of the registers */
		IRQ_GLOBAL_DISABLE(oldLevel);
/* TODO: implement hwregs */
		IRQ_GLOBAL_ENABLE(oldLevel);
		break;

		/* Put a chain of frames at the back of the "Ready frames" queue. This 
		 * triggers the driver to put frames from the Ready queue into unused 
		 * available descriptors. (Ready -> Scheduled)
		 */

		case GRTM_IOC_SEND:
		if ( !pDev->running ){
			return RTEMS_RESOURCE_IN_USE;
		}
		num=0;

		/* Get pointer to frame chain wished be sent */
		chain = (struct grtm_list *)ioarg->buffer;
		if ( !chain ){
			/* No new frames to send ==> just trigger hardware
			 * to send previously made ready frames to be sent.
			 */
			pDev->handling_transmission = 1;
			goto trigger_transmission;
		}
		if ( !chain->tail || !chain->head ){
			return RTEMS_INVALID_NAME;
		}

		DBG("GRTM_SEND: head: 0x%x, tail: 0x%x\n",chain->head,chain->tail);

		/* Mark ready frames unsent by clearing GRTM_FLAGS_SENT of all frames */

		curr = chain->head;
		while(curr != chain->tail){
			curr->flags = curr->flags & ~(GRTM_FLAGS_SENT|GRRM_FLAGS_ERR);
			curr = curr->next;
			num++;
		}
		curr->flags = curr->flags & ~(GRTM_FLAGS_SENT|GRRM_FLAGS_ERR);
		num++;

		pDev->handling_transmission = 1;
		/* 1. Put frames into ready queue 
		 *    (New Frames->READY)
		 */
		if ( pDev->ready.head ){
			/* Frames already on ready queue (no free descriptors previously) ==>
			 * Put frames at end of ready queue
			 */
			pDev->ready.tail->next = chain->head;
			pDev->ready.tail = chain->tail;
			chain->tail->next = NULL;
		}else{
			/* All frames is put into the ready queue for later processing */
			pDev->ready.head = chain->head;
			pDev->ready.tail = chain->tail;
			chain->tail->next = NULL;
		}
		pDev->ready_cnt += num;	/* Added 'num' frames to ready queue */
trigger_transmission:
		/* 2. Free used descriptors and put the sent frame into the "Sent queue"  
		 *    (SCHEDULED->SENT)
		 */
		num = grtm_free_sent(pDev);
		pDev->scheduled_cnt -= num;
		pDev->sent_cnt += num;

		/* 3. Use all available free descriptors there are frames for
		 *    in the ready queue.
		 *    (READY->SCHEDULED)
		 */
		num = grtm_schedule_ready(pDev,0);
		pDev->ready_cnt -= num;
		pDev->scheduled_cnt += num;
	
		pDev->handling_transmission = 0;
		break;

		/* Take all available sent frames from the "Sent frames" queue.
		 * If no frames has been sent, the thread may get blocked if in blocking
		 * mode. The blocking mode is not available if driver is not in running mode.
		 *
		 * Note this ioctl may return success even if the driver is not in STARTED mode.
		 * This is because in case of a error (link error of similar) and the driver switch
		 * from START to STOP mode we must still be able to get our frames back.
		 * 
		 * Note in case the driver fails to send a frame for some reason (link error),
		 * the sent flag is set to 0 indicating a failure.
		 *
		 */
		case GRTM_IOC_RECLAIM:
		/* Get pointer to were to place reaped chain */
		chain = (struct grtm_list *)ioarg->buffer;
		if ( !chain ){
			return RTEMS_INVALID_NAME;
		}

		/* Lock out interrupt handler */
		pDev->handling_transmission = 1;

		do {
			/* Move sent frames from descriptors to Sent queue. This makes more 
			 * descriptors (BDs) available.
			 */
			num = grtm_free_sent(pDev);
			pDev->scheduled_cnt -= num;
			pDev->sent_cnt += num;
			

			if ( pDev->running ){
				/* Fill descriptors with as many frames from the ready list 
				 * as possible.
				 */
				num = grtm_schedule_ready(pDev,0);
				pDev->ready_cnt -= num;
				pDev->scheduled_cnt += num;
			}

			/* Are there any frames on the sent queue waiting to be 
			 * reclaimed?
			 */

			if ( !pDev->sent.head ){
				/* No frames to reclaim - no frame in sent queue.
				 * Instead we block thread until frames have been sent 
				 * if in blocking mode.
				 */
				if ( pDev->running && pDev->config.blocking ){
					ret = rtems_semaphore_obtain(pDev->sem_tx,RTEMS_WAIT,pDev->config.timeout);
					if ( ret == RTEMS_TIMEOUT ) {
						pDev->handling_transmission = 0;
						return RTEMS_TIMEOUT;
					} else if ( ret == RTEMS_SUCCESSFUL ) {
						/* There might be frames available, go check */
						continue;
					} else {
						/* any error (driver closed, internal error etc.) */
						pDev->handling_transmission = 0;
						return RTEMS_UNSATISFIED;
					}

				}else{
					/* non-blocking mode, we quit */
					chain->head = NULL;
					chain->tail = NULL;
					/* do not lock out interrupt handler any more */
					pDev->handling_transmission = 0;
					return RTEMS_TIMEOUT;
				}
			}else{
				/* Take all sent framess from sent queue to userspace queue */
				chain->head = pDev->sent.head;
				chain->tail = pDev->sent.tail;
				chain->tail->next = NULL; /* Just for sure */

				/* Mark no Sent */
				grtm_list_clr(&pDev->sent);
				pDev->sent_cnt = 0;

				DBG("TX_RECLAIM: head: 0x%x, tail: 0x%x\n",chain->head,chain->tail);
				break;
			}

		}while(1);
		
		/* do not lock out interrupt handler any more */
		pDev->handling_transmission = 0;
		break;

		default:
		return RTEMS_NOT_DEFINED;
	}
	return RTEMS_SUCCESSFUL;
}

static void grtm_interrupt(void *arg)
{
	struct grtm_priv *pDev = arg;
	struct grtm_regs *regs = pDev->regs;
	unsigned int status;
	int num;

	/* Clear interrupt by reading it */
	status = READ_REG(&regs->dma_status);
	
	/* Spurious Interrupt? */
	if ( !pDev->running )
		return;

	if ( status )
		regs->dma_status = status;

	if ( status & GRTM_DMA_STS_TFF ){
		pDev->stats.err_transfer_frame++;
	}

	if ( status & GRTM_DMA_STS_TA ){
		pDev->stats.err_ahb++;
	}

	if ( status & GRTM_DMA_STS_TE ){
		pDev->stats.err_tx++;
	}

	if ( status & GRTM_DMA_STS_TI ){
		
		if ( pDev->config.isr_desc_proc && !pDev->handling_transmission ) {
			/* Free used descriptors and put the sent frame into the "Sent queue"  
			 *   (SCHEDULED->SENT)
			 */
			num = grtm_free_sent(pDev);
			pDev->scheduled_cnt -= num;
			pDev->sent_cnt += num;

			/* Use all available free descriptors there are frames for
			 * in the ready queue.
			 *   (READY->SCHEDULED)
			 */
			num = grtm_schedule_ready(pDev,1);
			pDev->ready_cnt -= num;
			pDev->scheduled_cnt += num;

#if 0
			if ( (pDev->config.blocking==GRTM_BLKMODE_COMPLETE) && pDev->timeout ){
				/* Signal to thread only if enough data is available */
				if ( pDev->wait_for_frames > grtm_data_avail(pDev) ){
					/* Not enough data available */
					goto procceed_processing_interrupts;
				}

				/* Enough number of frames has been transmitted which means that
				 * the waiting thread should be woken up.
				 */
				rtems_semaphore_release(pDev->sem_tx);
			}
#endif
		}

		if ( pDev->config.blocking == GRTM_BLKMODE_BLK ) {
			/* Blocking mode */

#if 0
			/* Disable further Interrupts until handled by waiting task. */
			regs->dma_ctrl = READ_REG(&regs->dma_ctrl) & ~GRTM_DMA_CTRL_IE;
#endif
		
			/* Signal Semaphore to wake waiting thread in ioctl(SEND|RECLAIM) */
			rtems_semaphore_release(pDev->sem_tx);
		}

	}
#if 0
procceed_processing_interrupts:
	;
#endif
}

static rtems_device_driver grtm_initialize(
  rtems_device_major_number major, 
  rtems_device_minor_number unused,
  void *arg
  )
{
	/* Device Semaphore created with count = 1 */
	if ( rtems_semaphore_create(rtems_build_name('G', 'R', 'T', 'M'),
		1,
		RTEMS_FIFO|RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|RTEMS_NO_PRIORITY_CEILING,
		0,
		&grtm_dev_sem) != RTEMS_SUCCESSFUL ) {
		return RTEMS_INTERNAL_ERROR;
	}

	return RTEMS_SUCCESSFUL;
}
