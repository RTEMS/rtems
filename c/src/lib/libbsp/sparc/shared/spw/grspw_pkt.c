/*
 * Cobham Gaisler GRSPW/GRSPW2 SpaceWire Kernel Library Interface for RTEMS.
 *
 * This driver can be used to implement a standard I/O system "char"-driver
 * or used directly. NOTE SMP support has not been tested.
 *
 * COPYRIGHT (c) 2011
 * Cobham Gaisler AB
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
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
#include <bsp/grspw_pkt.h>

/* This driver has been prepared for SMP operation however never tested 
 * on a SMP system - use on your own risk.
 */
#ifdef RTEMS_HAS_SMP

#include <rtems/score/smplock.h> /* spin-lock */

/* SPIN_LOCK() and SPIN_UNLOCK() NOT_IMPLEMENTED_BY_RTEMS. Use _IRQ version
 * to implement.
 */
#define SPIN_DECLARE(name) SMP_lock_spinlock_simple_Control name
#define SPIN_INIT(lock) _SMP_lock_spinlock_simple_Initialize(lock)
#define SPIN_LOCK(lock, level) SPIN_LOCK_IRQ(lock, level)
#define SPIN_LOCK_IRQ(lock, level) (level) = _SMP_lock_spinlock_simple_Obtain(lock)
#define SPIN_UNLOCK(lock, level) SPIN_UNLOCK_IRQ(lock, level)
#define SPIN_UNLOCK_IRQ(lock, level) _SMP_lock_spinlock_simple_Release(lock, level)
#define IRQFLAGS_TYPE ISR_Level

#else

#define SPIN_DECLARE(name)
#define SPIN_INIT(lock)
#define SPIN_LOCK(lock, level)
#define SPIN_LOCK_IRQ(lock, level) rtems_interrupt_disable(level)
#define SPIN_UNLOCK(lock, level)
#define SPIN_UNLOCK_IRQ(lock, level) rtems_interrupt_enable(level)
#define IRQFLAGS_TYPE rtems_interrupt_level

#endif

/*#define STATIC*/
#define STATIC static

/*#define GRSPW_DBG(args...) printk(args)*/
#define GRSPW_DBG(args...)

struct grspw_dma_regs {
	volatile unsigned int ctrl;	/* DMA Channel Control */
	volatile unsigned int rxmax;	/* RX Max Packet Length */
	volatile unsigned int txdesc;	/* TX Descriptor Base/Current */
	volatile unsigned int rxdesc;	/* RX Descriptor Base/Current */
	volatile unsigned int addr;	/* Address Register */
	volatile unsigned int resv[3];
};

struct grspw_regs {
	volatile unsigned int ctrl;
	volatile unsigned int status;
	volatile unsigned int nodeaddr;
	volatile unsigned int clkdiv;
	volatile unsigned int destkey;
	volatile unsigned int time;
	volatile unsigned int timer;	/* Used only in GRSPW1 */
	volatile unsigned int resv1;

	/* DMA Registers, ctrl.NCH determines number of ports, 
	 * up to 4 channels are supported
	 */
	struct grspw_dma_regs dma[4];

	volatile unsigned int icctrl;
	volatile unsigned int icrx;
	volatile unsigned int icack;
	volatile unsigned int ictimeout;
	volatile unsigned int ictickomask;
	volatile unsigned int icaamask;
	volatile unsigned int icrlpresc;
	volatile unsigned int icrlisr;
	volatile unsigned int icrlintack;
	volatile unsigned int resv2;
	volatile unsigned int icisr;
	volatile unsigned int resv3;
};

/* GRSPW - Control Register - 0x00 */
#define GRSPW_CTRL_RA_BIT	31
#define GRSPW_CTRL_RX_BIT	30
#define GRSPW_CTRL_RC_BIT	29
#define GRSPW_CTRL_NCH_BIT	27
#define GRSPW_CTRL_PO_BIT	26
#define GRSPW_CTRL_ID_BIT	24
#define GRSPW_CTRL_LE_BIT	22
#define GRSPW_CTRL_PS_BIT	21
#define GRSPW_CTRL_NP_BIT	20
#define GRSPW_CTRL_RD_BIT	17
#define GRSPW_CTRL_RE_BIT	16
#define GRSPW_CTRL_TF_BIT	12
#define GRSPW_CTRL_TR_BIT	11
#define GRSPW_CTRL_TT_BIT	10
#define GRSPW_CTRL_LI_BIT	9
#define GRSPW_CTRL_TQ_BIT	8
#define GRSPW_CTRL_RS_BIT	6
#define GRSPW_CTRL_PM_BIT	5
#define GRSPW_CTRL_TI_BIT	4
#define GRSPW_CTRL_IE_BIT	3
#define GRSPW_CTRL_AS_BIT	2
#define GRSPW_CTRL_LS_BIT	1
#define GRSPW_CTRL_LD_BIT	0

#define GRSPW_CTRL_RA	(1<<GRSPW_CTRL_RA_BIT)
#define GRSPW_CTRL_RX	(1<<GRSPW_CTRL_RX_BIT)
#define GRSPW_CTRL_RC	(1<<GRSPW_CTRL_RC_BIT)
#define GRSPW_CTRL_NCH	(0x3<<GRSPW_CTRL_NCH_BIT)
#define GRSPW_CTRL_PO	(1<<GRSPW_CTRL_PO_BIT)
#define GRSPW_CTRL_ID	(1<<GRSPW_CTRL_ID_BIT)
#define GRSPW_CTRL_LE	(1<<GRSPW_CTRL_LE_BIT)
#define GRSPW_CTRL_PS	(1<<GRSPW_CTRL_PS_BIT)
#define GRSPW_CTRL_NP	(1<<GRSPW_CTRL_NP_BIT)
#define GRSPW_CTRL_RD	(1<<GRSPW_CTRL_RD_BIT)
#define GRSPW_CTRL_RE	(1<<GRSPW_CTRL_RE_BIT)
#define GRSPW_CTRL_TF	(1<<GRSPW_CTRL_TF_BIT)
#define GRSPW_CTRL_TR	(1<<GRSPW_CTRL_TR_BIT)
#define GRSPW_CTRL_TT	(1<<GRSPW_CTRL_TT_BIT)
#define GRSPW_CTRL_LI	(1<<GRSPW_CTRL_LI_BIT)
#define GRSPW_CTRL_TQ	(1<<GRSPW_CTRL_TQ_BIT)
#define GRSPW_CTRL_RS	(1<<GRSPW_CTRL_RS_BIT)
#define GRSPW_CTRL_PM	(1<<GRSPW_CTRL_PM_BIT)
#define GRSPW_CTRL_TI	(1<<GRSPW_CTRL_TI_BIT)
#define GRSPW_CTRL_IE	(1<<GRSPW_CTRL_IE_BIT)
#define GRSPW_CTRL_AS	(1<<GRSPW_CTRL_AS_BIT)
#define GRSPW_CTRL_LS	(1<<GRSPW_CTRL_LS_BIT)
#define GRSPW_CTRL_LD	(1<<GRSPW_CTRL_LD_BIT)

#define GRSPW_CTRL_IRQSRC_MASK \
	(GRSPW_CTRL_LI | GRSPW_CTRL_TQ)
#define GRSPW_ICCTRL_IRQSRC_MASK \
	(GRSPW_ICCTRL_TQ | GRSPW_ICCTRL_AQ | GRSPW_ICCTRL_IQ)


/* GRSPW - Status Register - 0x04 */
#define GRSPW_STS_LS_BIT	21
#define GRSPW_STS_AP_BIT	9
#define GRSPW_STS_EE_BIT	8
#define GRSPW_STS_IA_BIT	7
#define GRSPW_STS_WE_BIT	6	/* GRSPW1 */
#define GRSPW_STS_PE_BIT	4
#define GRSPW_STS_DE_BIT	3
#define GRSPW_STS_ER_BIT	2
#define GRSPW_STS_CE_BIT	1
#define GRSPW_STS_TO_BIT	0

#define GRSPW_STS_LS	(0x7<<GRSPW_STS_LS_BIT)
#define GRSPW_STS_AP	(1<<GRSPW_STS_AP_BIT)
#define GRSPW_STS_EE	(1<<GRSPW_STS_EE_BIT)
#define GRSPW_STS_IA	(1<<GRSPW_STS_IA_BIT)
#define GRSPW_STS_WE	(1<<GRSPW_STS_WE_BIT)	/* GRSPW1 */
#define GRSPW_STS_PE	(1<<GRSPW_STS_PE_BIT)
#define GRSPW_STS_DE	(1<<GRSPW_STS_DE_BIT)
#define GRSPW_STS_ER	(1<<GRSPW_STS_ER_BIT)
#define GRSPW_STS_CE	(1<<GRSPW_STS_CE_BIT)
#define GRSPW_STS_TO	(1<<GRSPW_STS_TO_BIT)

/* GRSPW - Default Address Register - 0x08 */
#define GRSPW_DEF_ADDR_BIT	0
#define GRSPW_DEF_MASK_BIT	8
#define GRSPW_DEF_ADDR	(0xff<<GRSPW_DEF_ADDR_BIT)
#define GRSPW_DEF_MASK	(0xff<<GRSPW_DEF_MASK_BIT)

/* GRSPW - Clock Divisor Register - 0x0C */
#define GRSPW_CLKDIV_START_BIT	8
#define GRSPW_CLKDIV_RUN_BIT	0
#define GRSPW_CLKDIV_START	(0xff<<GRSPW_CLKDIV_START_BIT)
#define GRSPW_CLKDIV_RUN	(0xff<<GRSPW_CLKDIV_RUN_BIT)
#define GRSPW_CLKDIV_MASK	(GRSPW_CLKDIV_START|GRSPW_CLKDIV_RUN)

/* GRSPW - Destination key Register - 0x10 */
#define GRSPW_DK_DESTKEY_BIT	0
#define GRSPW_DK_DESTKEY	(0xff<<GRSPW_DK_DESTKEY_BIT)

/* GRSPW - Time Register - 0x14 */
#define GRSPW_TIME_CTRL_BIT	6
#define GRSPW_TIME_CNT_BIT	0
#define GRSPW_TIME_CTRL		(0x3<<GRSPW_TIME_CTRL_BIT)
#define GRSPW_TIME_TCNT		(0x3f<<GRSPW_TIME_CNT_BIT)

/* GRSPW - DMA Control Register - 0x20*N */
#define GRSPW_DMACTRL_LE_BIT	16
#define GRSPW_DMACTRL_SP_BIT	15
#define GRSPW_DMACTRL_SA_BIT	14
#define GRSPW_DMACTRL_EN_BIT	13
#define GRSPW_DMACTRL_NS_BIT	12
#define GRSPW_DMACTRL_RD_BIT	11
#define GRSPW_DMACTRL_RX_BIT	10
#define GRSPW_DMACTRL_AT_BIT	9
#define GRSPW_DMACTRL_RA_BIT	8
#define GRSPW_DMACTRL_TA_BIT	7
#define GRSPW_DMACTRL_PR_BIT	6
#define GRSPW_DMACTRL_PS_BIT	5
#define GRSPW_DMACTRL_AI_BIT	4
#define GRSPW_DMACTRL_RI_BIT	3
#define GRSPW_DMACTRL_TI_BIT	2
#define GRSPW_DMACTRL_RE_BIT	1
#define GRSPW_DMACTRL_TE_BIT	0

#define GRSPW_DMACTRL_LE	(1<<GRSPW_DMACTRL_LE_BIT)
#define GRSPW_DMACTRL_SP	(1<<GRSPW_DMACTRL_SP_BIT)
#define GRSPW_DMACTRL_SA	(1<<GRSPW_DMACTRL_SA_BIT)
#define GRSPW_DMACTRL_EN	(1<<GRSPW_DMACTRL_EN_BIT)
#define GRSPW_DMACTRL_NS	(1<<GRSPW_DMACTRL_NS_BIT)
#define GRSPW_DMACTRL_RD	(1<<GRSPW_DMACTRL_RD_BIT)
#define GRSPW_DMACTRL_RX	(1<<GRSPW_DMACTRL_RX_BIT)
#define GRSPW_DMACTRL_AT	(1<<GRSPW_DMACTRL_AT_BIT)
#define GRSPW_DMACTRL_RA	(1<<GRSPW_DMACTRL_RA_BIT)
#define GRSPW_DMACTRL_TA	(1<<GRSPW_DMACTRL_TA_BIT)
#define GRSPW_DMACTRL_PR	(1<<GRSPW_DMACTRL_PR_BIT)
#define GRSPW_DMACTRL_PS	(1<<GRSPW_DMACTRL_PS_BIT)
#define GRSPW_DMACTRL_AI	(1<<GRSPW_DMACTRL_AI_BIT)
#define GRSPW_DMACTRL_RI	(1<<GRSPW_DMACTRL_RI_BIT)
#define GRSPW_DMACTRL_TI	(1<<GRSPW_DMACTRL_TI_BIT)
#define GRSPW_DMACTRL_RE	(1<<GRSPW_DMACTRL_RE_BIT)
#define GRSPW_DMACTRL_TE	(1<<GRSPW_DMACTRL_TE_BIT)

/* GRSPW - DMA Channel Max Packet Length Register - (0x20*N + 0x04) */
#define GRSPW_DMARXLEN_MAX_BIT	0
#define GRSPW_DMARXLEN_MAX	(0xffffff<<GRSPW_DMARXLEN_MAX_BIT)

/* GRSPW - DMA Channel Address Register - (0x20*N + 0x10) */
#define GRSPW_DMAADR_ADDR_BIT	0
#define GRSPW_DMAADR_MASK_BIT	8
#define GRSPW_DMAADR_ADDR	(0xff<<GRSPW_DMAADR_ADDR_BIT)
#define GRSPW_DMAADR_MASK	(0xff<<GRSPW_DMAADR_MASK_BIT)

/* GRSPW - Interrupt code receive register - 0xa4 */
#define GRSPW_ICCTRL_INUM_BIT	27
#define GRSPW_ICCTRL_IA_BIT	24
#define GRSPW_ICCTRL_LE_BIT	23
#define GRSPW_ICCTRL_PR_BIT	22
#define GRSPW_ICCTRL_DQ_BIT	21 /* never used */
#define GRSPW_ICCTRL_TQ_BIT	20
#define GRSPW_ICCTRL_AQ_BIT	19
#define GRSPW_ICCTRL_IQ_BIT	18
#define GRSPW_ICCTRL_IR_BIT	17
#define GRSPW_ICCTRL_IT_BIT	16
#define GRSPW_ICCTRL_NUMI_BIT	13
#define GRSPW_ICCTRL_BIRQ_BIT	8
#define GRSPW_ICCTRL_ID_BIT	7
#define GRSPW_ICCTRL_II_BIT	6
#define GRSPW_ICCTRL_TXIRQ_BIT	0
#define GRSPW_ICCTRL_INUM	(0x3f << GRSPW_ICCTRL_INUM_BIT)
#define GRSPW_ICCTRL_IA		(1 << GRSPW_ICCTRL_IA_BIT)
#define GRSPW_ICCTRL_LE		(1 << GRSPW_ICCTRL_LE_BIT)
#define GRSPW_ICCTRL_PR		(1 << GRSPW_ICCTRL_PR_BIT)
#define GRSPW_ICCTRL_DQ		(1 << GRSPW_ICCTRL_DQ_BIT)
#define GRSPW_ICCTRL_TQ		(1 << GRSPW_ICCTRL_TQ_BIT)
#define GRSPW_ICCTRL_AQ		(1 << GRSPW_ICCTRL_AQ_BIT)
#define GRSPW_ICCTRL_IQ		(1 << GRSPW_ICCTRL_IQ_BIT)
#define GRSPW_ICCTRL_IR		(1 << GRSPW_ICCTRL_IR_BIT)
#define GRSPW_ICCTRL_IT		(1 << GRSPW_ICCTRL_IT_BIT)
#define GRSPW_ICCTRL_NUMI	(0x7 << GRSPW_ICCTRL_NUMI_BIT)
#define GRSPW_ICCTRL_BIRQ	(0x1f << GRSPW_ICCTRL_BIRQ_BIT)
#define GRSPW_ICCTRL_ID		(1 << GRSPW_ICCTRL_ID_BIT)
#define GRSPW_ICCTRL_II		(1 << GRSPW_ICCTRL_II_BIT)
#define GRSPW_ICCTRL_TXIRQ	(0x3f << GRSPW_ICCTRL_TXIRQ_BIT)

/* RX Buffer Descriptor */
struct grspw_rxbd {
   volatile unsigned int ctrl;
   volatile unsigned int addr;
};

/* TX Buffer Descriptor */
struct grspw_txbd {
   volatile unsigned int ctrl;
   volatile unsigned int haddr;
   volatile unsigned int dlen;
   volatile unsigned int daddr;
};

/* GRSPW - DMA RXBD Ctrl */
#define GRSPW_RXBD_LEN_BIT 0
#define GRSPW_RXBD_LEN	(0x1ffffff<<GRSPW_RXBD_LEN_BIT)
#define GRSPW_RXBD_EN	(1<<25)
#define GRSPW_RXBD_WR	(1<<26)
#define GRSPW_RXBD_IE	(1<<27)
#define GRSPW_RXBD_EP	(1<<28)
#define GRSPW_RXBD_HC	(1<<29)
#define GRSPW_RXBD_DC	(1<<30)
#define GRSPW_RXBD_TR	(1<<31)

#define GRSPW_TXBD_HLEN	(0xff<<0)
#define GRSPW_TXBD_NCL	(0xf<<8)
#define GRSPW_TXBD_EN	(1<<12)
#define GRSPW_TXBD_WR	(1<<13)
#define GRSPW_TXBD_IE	(1<<14)
#define GRSPW_TXBD_LE	(1<<15)
#define GRSPW_TXBD_HC	(1<<16)
#define GRSPW_TXBD_DC	(1<<17)

#define GRSPW_DMAADR_MASK_BIT	8
#define GRSPW_DMAADR_ADDR	(0xff<<GRSPW_DMAADR_ADDR_BIT)
#define GRSPW_DMAADR_MASK	(0xff<<GRSPW_DMAADR_MASK_BIT)


/* GRSPW Error Condition */
#define GRSPW_STAT_ERROR	(GRSPW_STS_EE | GRSPW_STS_IA | GRSPW_STS_WE | GRSPW_STS_PE | GRSPW_STS_DE | GRSPW_STS_ER | GRSPW_STS_CE)
#define GRSPW_DMA_STATUS_ERROR	(GRSPW_DMACTRL_RA | GRSPW_DMACTRL_TA)
/* GRSPW Link configuration options */
#define GRSPW_LINK_CFG		(GRSPW_CTRL_LI | GRSPW_CTRL_LD | GRSPW_CTRL_LS | GRSPW_CTRL_AS)
#define GRSPW_LINKSTATE(status)	((status & GRSPW_CTRL_LS) >> GRSPW_CTRL_LS_BIT)

/* Software Defaults */
#define DEFAULT_RXMAX 1024	/* 1 KBytes Max RX Packet Size */

/* GRSPW Constants */
#define GRSPW_TXBD_NR 64	/* Maximum number of TX Descriptors */
#define GRSPW_RXBD_NR 128	/* Maximum number of RX Descriptors */
#define BDTAB_SIZE 0x400	/* BD Table Size (RX or TX) */
#define BDTAB_ALIGN 0x400	/* BD Table Alignment Requirement */

/* Memory and HW Registers Access routines. All 32-bit access routines */
#define BD_WRITE(addr, val) (*(volatile unsigned int *)(addr) = (unsigned int)(val))
/*#define BD_READ(addr) (*(volatile unsigned int *)(addr))*/
#define BD_READ(addr) leon_r32_no_cache((unsigned long)(addr))
#define REG_WRITE(addr, val) (*(volatile unsigned int *)(addr) = (unsigned int)(val))
#define REG_READ(addr) (*(volatile unsigned int *)(addr))

struct grspw_ring {
	struct grspw_ring *next;	/* Next Descriptor */
	union {
		struct grspw_txbd *tx;	/* Descriptor Address */
		struct grspw_rxbd *rx;	/* Descriptor Address */
	} bd;
	struct grspw_pkt *pkt;		/* Packet description associated.NULL if none*/	
};

/* An entry in the TX descriptor Ring */
struct grspw_txring {
	struct grspw_txring *next;	/* Next Descriptor */
	struct grspw_txbd *bd;		/* Descriptor Address */
	struct grspw_pkt *pkt;		/* Packet description associated.NULL if none*/
};

/* An entry in the RX descriptor Ring */
struct grspw_rxring {
	struct grspw_rxring *next;	/* Next Descriptor */
	struct grspw_rxbd *bd;		/* Descriptor Address */
	struct grspw_pkt *pkt;		/* Packet description associated.NULL if none*/
};


struct grspw_dma_priv {
	struct grspw_priv *core;	/* GRSPW Core */
	struct grspw_dma_regs *regs;	/* DMA Channel Registers */
	int index;			/* DMA Channel Index @ GRSPW core */
	int open;			/* DMA Channel opened by user */
	int started;			/* DMA Channel activity (start|stop) */
	rtems_id sem_dma;		/* DMA Channel Semaphore */
	struct grspw_dma_stats stats;	/* DMA Channel Statistics */
	struct grspw_dma_config cfg;	/* DMA Channel Configuration */

	/*** RX ***/

	/* RX Descriptor Ring */
	struct grspw_rxbd *rx_bds;		/* Descriptor Address */
	struct grspw_rxbd *rx_bds_hwa;		/* Descriptor HW Address */
	struct grspw_rxring *rx_ring_base;
	struct grspw_rxring *rx_ring_head;	/* Next descriptor to enable */
	struct grspw_rxring *rx_ring_tail;	/* Oldest enabled Descriptor */
	int rx_irq_en_cnt_curr;
	struct {
		int waiting;
		int ready_cnt;
		int op;
		int recv_cnt;
		rtems_id sem_wait;		/* RX Semaphore used to implement RX blocking */
	} rx_wait;

	/* Queue of Packets READY to be scheduled */
	struct grspw_list ready;
	int ready_cnt;

	/* Scheduled RX Packets Queue */
	struct grspw_list rx_sched;
	int rx_sched_cnt;

	/* Queue of Packets that has been RECIEVED */
	struct grspw_list recv;
	int recv_cnt;


	/*** TX ***/

	/* TX Descriptor Ring */
	struct grspw_txbd *tx_bds;		/* Descriptor Address */
	struct grspw_txbd *tx_bds_hwa;		/* Descriptor HW Address */
	struct grspw_txring *tx_ring_base;
	struct grspw_txring *tx_ring_head;
	struct grspw_txring *tx_ring_tail;
	int tx_irq_en_cnt_curr;
	struct {
		int waiting;
		int send_cnt;
		int op;
		int sent_cnt;
		rtems_id sem_wait;		/* TX Semaphore used to implement TX blocking */
	} tx_wait;

	/* Queue of Packets ready to be scheduled for transmission */
	struct grspw_list send;
	int send_cnt;

	/* Scheduled TX Packets Queue */
	struct grspw_list tx_sched;
	int tx_sched_cnt;

	/* Queue of Packets that has been SENT */
	struct grspw_list sent;
	int sent_cnt;
};

struct grspw_priv {
	char devname[8];		/* Device name "grspw%d" */
	struct drvmgr_dev *dev;		/* Device */
	struct grspw_regs *regs;	/* Virtual Address of APB Registers */
	int irq;			/* AMBA IRQ number of core */
	int index;			/* Index in order it was probed */
	int core_index;			/* Core Bus Index */
	int open;			/* If Device is alrady opened (=1) or not (=0) */
	void *data;			/* User private Data for this device instance, set by grspw_initialize_user */

	/* Features supported by Hardware */
	struct grspw_hw_sup hwsup;

	/* Pointer to an array of Maximally 4 DMA Channels */
	struct grspw_dma_priv *dma;

	/* Spin-lock ISR protection */
	SPIN_DECLARE(devlock);

	/* Descriptor Memory Area for TX & RX and all DMA channels */
	unsigned int bd_mem;
	unsigned int bd_mem_alloced;

	/*** Time Code Handling ***/
	void (*tcisr)(void *data, int timecode);
	void *tcisr_arg;

	/*** Interrupt-code Handling ***/
	spwpkt_ic_isr_t icisr;
	void *icisr_arg;

	/* Disable Link on SpW Link error */
	int dis_link_on_err;

	/* "Core Global" Statistics gathered, not dependent on DMA channel */
	struct grspw_core_stats stats;
};

int grspw_initialized = 0;
int grspw_count = 0;
struct workqueue_struct *grspw_workq = NULL;
rtems_id grspw_sem;
static struct grspw_priv *priv_tab[GRSPW_MAX];

/* callback to upper layer when devices are discovered/removed */
void *(*grspw_dev_add)(int) = NULL;
void (*grspw_dev_del)(int,void*) = NULL;

/* USER OVERRIDABLE - The work task priority. Set to -1 to disable creating
 * the work-task and work-queue to save space.
 */
int grspw_work_task_priority __attribute__((weak)) = 100;
int grspw_task_stop = 0;
rtems_id grspw_work_task;
rtems_id grspw_work_queue = 0;
#define WORK_NONE         0
#define WORK_SHUTDOWN     0x100
#define WORK_DMA(channel) (0x1 << (channel))
#define WORK_DMA_MASK     0xf /* max 4 channels */
#define WORK_CORE_BIT     16
#define WORK_CORE_MASK    0xffff
#define WORK_CORE(device) ((device) << WORK_CORE_BIT)

STATIC void grspw_hw_stop(struct grspw_priv *priv);
STATIC void grspw_hw_dma_stop(struct grspw_dma_priv *dma);
STATIC void grspw_dma_reset(struct grspw_dma_priv *dma);
STATIC void grspw_dma_stop_locked(struct grspw_dma_priv *dma);
STATIC void grspw_isr(void *data);

void *grspw_open(int dev_no)
{
	struct grspw_priv *priv;
	unsigned int bdtabsize, hwa;
	int i;
	union drvmgr_key_value *value;

	if (grspw_initialized != 1 || (dev_no >= grspw_count))
		return NULL;

	priv = priv_tab[dev_no];

	/* Take GRSPW lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(grspw_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL)
		return NULL;

	if (priv->open) {
		priv = NULL;
		goto out;
	}

	/* Initialize Spin-lock for GRSPW Device. This is to protect
	 * CTRL and DMACTRL registers from ISR.
	 */
	SPIN_INIT(&priv->devlock);

	priv->tcisr = NULL;
	priv->tcisr_arg = NULL;
	priv->icisr = NULL;
	priv->icisr_arg = NULL;

	grspw_stats_clr(priv);

	/* Allocate TX & RX Descriptor memory area for all DMA
	 * channels. Max-size descriptor area is allocated (or user assigned):
	 *  - 128 RX descriptors per DMA Channel
	 *  - 64 TX descriptors per DMA Channel
 	 */
	bdtabsize = 2 * BDTAB_SIZE * priv->hwsup.ndma_chans;
	value = drvmgr_dev_key_get(priv->dev, "bdDmaArea", DRVMGR_KT_INT);
	if (value) {
		priv->bd_mem = value->i;
		priv->bd_mem_alloced = 0;
		if (priv->bd_mem & (BDTAB_ALIGN-1)) {
			GRSPW_DBG("GRSPW[%d]: user-def DMA-area not aligned",
			          priv->index);
			priv = NULL;
			goto out;
		}
	} else {
		priv->bd_mem_alloced = (unsigned int)malloc(bdtabsize + BDTAB_ALIGN - 1);
		if (priv->bd_mem_alloced == 0) {
			priv = NULL;
			goto out;
		}
		/* Align memory */
		priv->bd_mem = (priv->bd_mem_alloced + (BDTAB_ALIGN - 1)) &
		               ~(BDTAB_ALIGN-1);
	}

	/* Translate into DMA address that HW can use to access DMA
	 * descriptors
	 */
	drvmgr_translate_check(
		priv->dev,
		CPUMEM_TO_DMA,
		(void *)priv->bd_mem,
		(void **)&hwa,
		bdtabsize);

	GRSPW_DBG("GRSPW%d DMA descriptor table setup: (alloced:%p, bd_mem:%p, size: %d)\n",
		priv->index, priv->bd_mem_alloced, priv->bd_mem, bdtabsize + BDTAB_ALIGN - 1);
	for (i=0; i<priv->hwsup.ndma_chans; i++) {
		/* Do DMA Channel Init, other variables etc. are inited
		 * when respective DMA channel is opened.
		 *
		 * index & core are initialized by probe function.
		 */
		priv->dma[i].open = 0;
		priv->dma[i].rx_bds = (struct grspw_rxbd *)
			(priv->bd_mem + i*BDTAB_SIZE*2);
		priv->dma[i].rx_bds_hwa = (struct grspw_rxbd *)
			(hwa + BDTAB_SIZE*(2*i));
		priv->dma[i].tx_bds = (struct grspw_txbd *)
			(priv->bd_mem + BDTAB_SIZE*(2*i+1));
		priv->dma[i].tx_bds_hwa = (struct grspw_txbd *)
			(hwa + BDTAB_SIZE*(2*i+1));
		GRSPW_DBG("  DMA[%i]: RX %p - %p (%p - %p)   TX %p - %p (%p - %p)\n",
			i,
			priv->dma[i].rx_bds, (void *)priv->dma[i].rx_bds + BDTAB_SIZE - 1,
			priv->dma[i].rx_bds_hwa, (void *)priv->dma[i].rx_bds_hwa + BDTAB_SIZE - 1,
			priv->dma[i].tx_bds, (void *)priv->dma[i].tx_bds + BDTAB_SIZE - 1,
			priv->dma[i].tx_bds_hwa, (void *)priv->dma[i].tx_bds_hwa + BDTAB_SIZE - 1);
	}

	/* Basic initialization of hardware, clear some registers but
	 * keep Link/RMAP/Node-Address registers intact.
	 */
	grspw_hw_stop(priv);

	/* Register Interrupt handler and enable IRQ at IRQ ctrl */
	drvmgr_interrupt_register(priv->dev, 0, priv->devname, grspw_isr, priv);

	/* Take the device */
	priv->open = 1;
out:
	rtems_semaphore_release(grspw_sem);
	return priv;
}

void grspw_close(void *d)
{
	struct grspw_priv *priv = d;
	int i;

	/* Take GRSPW lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(grspw_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL)
		return;

	/* Stop Hardware from doing DMA, put HW into "startup-state",
	 * Stop hardware from generating IRQ.
	 */
	for (i=0; i<priv->hwsup.ndma_chans; i++)
		grspw_dma_close(&priv->dma[i]);
	grspw_hw_stop(priv);

	/* Mark not open */
	priv->open = 0;

	rtems_semaphore_release(grspw_sem);

	/* Check that all threads are out? */
}

void grspw_hw_support(void *d, struct grspw_hw_sup *hw)
{
	struct grspw_priv *priv = d;

	*hw = priv->hwsup;
}

void grspw_addr_ctrl(void *d, struct grspw_addr_config *cfg)
{
	struct grspw_priv *priv = d;
	struct grspw_regs *regs = priv->regs;
	unsigned int ctrl, nodeaddr;
	IRQFLAGS_TYPE irqflags;
	int i;

	if (!priv || !cfg)
		return;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	if (cfg->promiscuous != -1) {
		/* Set Configuration */
		ctrl = REG_READ(&regs->ctrl);
		if (cfg->promiscuous)
			ctrl |= GRSPW_CTRL_PM;
		else
			ctrl &= ~GRSPW_CTRL_PM;
		REG_WRITE(&regs->ctrl, ctrl);
		REG_WRITE(&regs->nodeaddr, (cfg->def_mask<<8) | cfg->def_addr);

		for (i=0; i<priv->hwsup.ndma_chans; i++) {
			ctrl = REG_READ(&regs->dma[i].ctrl);
			ctrl &= ~(GRSPW_DMACTRL_PS|GRSPW_DMACTRL_PR|GRSPW_DMA_STATUS_ERROR);
			if (cfg->dma_nacfg[i].node_en) {
				ctrl |= GRSPW_DMACTRL_EN;
				REG_WRITE(&regs->dma[i].addr,
				          (cfg->dma_nacfg[i].node_addr & 0xff) |
				          ((cfg->dma_nacfg[i].node_mask & 0xff)<<8));
			} else {
				ctrl &= ~GRSPW_DMACTRL_EN;
			}
			REG_WRITE(&regs->dma[i].ctrl, ctrl);
		}
	}

	/* Read Current Configuration */
	cfg->promiscuous = REG_READ(&regs->ctrl) & GRSPW_CTRL_PM;
	nodeaddr = REG_READ(&regs->nodeaddr);
	cfg->def_addr = (nodeaddr & GRSPW_DEF_ADDR) >> GRSPW_DEF_ADDR_BIT;
	cfg->def_mask = (nodeaddr & GRSPW_DEF_MASK) >> GRSPW_DEF_MASK_BIT;
	for (i=0; i<priv->hwsup.ndma_chans; i++) {
		cfg->dma_nacfg[i].node_en = REG_READ(&regs->dma[i].ctrl) &
						GRSPW_DMACTRL_EN;
		ctrl = REG_READ(&regs->dma[i].addr);
		cfg->dma_nacfg[i].node_addr = (ctrl & GRSPW_DMAADR_ADDR) >>
						GRSPW_DMAADR_ADDR_BIT;
		cfg->dma_nacfg[i].node_mask = (ctrl & GRSPW_DMAADR_MASK) >>
						GRSPW_DMAADR_MASK_BIT;
	}
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
	for (; i<4; i++) {
		cfg->dma_nacfg[i].node_en = 0;
		cfg->dma_nacfg[i].node_addr = 0;
		cfg->dma_nacfg[i].node_mask = 0;
	}
}

/* Return Current Status Register */
unsigned int grspw_link_status(void *d)
{
	struct grspw_priv *priv = d;

	return REG_READ(&priv->regs->status);
}

/* Return Current Link State */
spw_link_state_t grspw_link_state(void *d)
{
	struct grspw_priv *priv = d;
	unsigned int status = REG_READ(&priv->regs->status);

	return (status & GRSPW_STS_LS) >> GRSPW_STS_LS_BIT;
}

/* Enable Global IRQ only if some irq source is set */
static inline int grspw_is_irqsource_set(unsigned int ctrl, unsigned int icctrl)
{
	return (ctrl & GRSPW_CTRL_IRQSRC_MASK) ||
		(icctrl & GRSPW_ICCTRL_IRQSRC_MASK);
}


/* options and clkdiv [in/out]: set to -1 to only read current config */
void grspw_link_ctrl(void *d, int *options, int *clkdiv)
{
	struct grspw_priv *priv = d;
	struct grspw_regs *regs = priv->regs;
	unsigned int ctrl;
	IRQFLAGS_TYPE irqflags;

	/* Write? */
	if (clkdiv) {
		if (*clkdiv != -1)
			REG_WRITE(&regs->clkdiv, *clkdiv & GRSPW_CLKDIV_MASK);
		*clkdiv = REG_READ(&regs->clkdiv) & GRSPW_CLKDIV_MASK;
	}
	if (options) {
		SPIN_LOCK_IRQ(&priv->devlock, irqflags);
		ctrl = REG_READ(&regs->ctrl);
		if (*options != -1) {
			ctrl = (ctrl & ~GRSPW_LINK_CFG) |
				(*options & GRSPW_LINK_CFG);

			/* Enable Global IRQ only if some irq source is set */
			if (grspw_is_irqsource_set(ctrl, REG_READ(&regs->icctrl)))
				ctrl |= GRSPW_CTRL_IE;
			else
				ctrl &= ~GRSPW_CTRL_IE;

			REG_WRITE(&regs->ctrl, ctrl);
			priv->dis_link_on_err = (*options & LINKOPTS_DIS_ONERR) >> 3;
		}
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
		*options = (ctrl & GRSPW_LINK_CFG)|(priv->dis_link_on_err << 3);
	}
}

/* Generate Tick-In (increment Time Counter, Send Time Code) */
void grspw_tc_tx(void *d)
{
	struct grspw_priv *priv = d;
	struct grspw_regs *regs = priv->regs;
	IRQFLAGS_TYPE irqflags;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	REG_WRITE(&regs->ctrl, REG_READ(&regs->ctrl) | GRSPW_CTRL_TI);
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
}

void grspw_tc_ctrl(void *d, int *options)
{
	struct grspw_priv *priv = d;
	struct grspw_regs *regs = priv->regs;
	unsigned int ctrl;
	IRQFLAGS_TYPE irqflags;

	if (options == NULL)
		return;

	/* Write? */
	if (*options != -1) {
		SPIN_LOCK_IRQ(&priv->devlock, irqflags);
		ctrl = REG_READ(&regs->ctrl);
		ctrl &= ~(GRSPW_CTRL_TR|GRSPW_CTRL_TT|GRSPW_CTRL_TQ);
		ctrl |= (*options & 0xd) << GRSPW_CTRL_TQ_BIT;

		/* Enable Global IRQ only if some irq source is set */
		if (grspw_is_irqsource_set(ctrl, REG_READ(&regs->icctrl)))
			ctrl |= GRSPW_CTRL_IE;
		else
			ctrl &= ~GRSPW_CTRL_IE;

		REG_WRITE(&regs->ctrl, ctrl);
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
	} else
		ctrl = REG_READ(&regs->ctrl);
	*options = (ctrl >> GRSPW_CTRL_TQ_BIT) & 0xd;
}

/* Assign ISR Function to TimeCode RX IRQ */
void grspw_tc_isr(void *d, void (*tcisr)(void *data, int tc), void *data)
{
	struct grspw_priv *priv = d;

	priv->tcisr_arg = data;
	priv->tcisr = tcisr;
}

/* Read/Write TCTRL and TIMECNT. Write if not -1, always read current value
 * TCTRL   = bits 7 and 6
 * TIMECNT = bits 5 to 0
 */
void grspw_tc_time(void *d, int *time)
{
        struct grspw_priv *priv = d;
        struct grspw_regs *regs = priv->regs;

        if (time == NULL)
                return;
        if (*time != -1)
                REG_WRITE(&regs->time, *time & (GRSPW_TIME_TCNT | GRSPW_TIME_CTRL));
        *time = REG_READ(&regs->time) & (GRSPW_TIME_TCNT | GRSPW_TIME_CTRL);
}

/* Generate Tick-In for the given Interrupt-code and check for generation
 * error.
 *
 * Returns zero on success and non-zero on failure
 */
int grspw_ic_tickin(void *d, int ic)
{
	struct grspw_priv *priv = d;
	struct grspw_regs *regs = priv->regs;
	IRQFLAGS_TYPE irqflags;
	unsigned int icctrl, mask;

	/* Prepare before turning off IRQ */
	mask = 0x3f << GRSPW_ICCTRL_TXIRQ_BIT;
	ic = ((ic << GRSPW_ICCTRL_TXIRQ_BIT) & mask) |
	     GRSPW_ICCTRL_II | GRSPW_ICCTRL_ID;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	icctrl = REG_READ(&regs->icctrl);
	icctrl &= ~mask;
	icctrl |= ic;
	REG_WRITE(&regs->icctrl, icctrl); /* Generate SpW Interrupt Tick-In */
	/* the ID bit is valid after two clocks, so we not to wait here */
	icctrl = REG_READ(&regs->icctrl); /* Check SpW-Int generation error */
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return icctrl & GRSPW_ICCTRL_ID;
}

#define ICOPTS_CTRL_MASK ICOPTS_EN_FLAGFILTER
#define ICOPTS_ICCTRL_MASK						\
	(ICOPTS_INTNUM | ICOPTS_EN_SPWIRQ_ON_EE  | ICOPTS_EN_SPWIRQ_ON_IA | \
	 ICOPTS_EN_PRIO | ICOPTS_EN_TIMEOUTIRQ | ICOPTS_EN_ACKIRQ | \
	 ICOPTS_EN_TICKOUTIRQ | ICOPTS_EN_RX | ICOPTS_EN_TX | \
	 ICOPTS_BASEIRQ)

/* Control Interrupt-code settings of core
 * Write if not pointing to -1, always read current value
 *
 * TODO: A lot of code duplication with grspw_tc_ctrl
 */
void grspw_ic_ctrl(void *d, unsigned int *options)
{
	struct grspw_priv *priv = d;
	struct grspw_regs *regs = priv->regs;
	unsigned int ctrl;
	unsigned int icctrl;
	IRQFLAGS_TYPE irqflags;

	if (options == NULL)
		return;

	if (*options != -1) {
		SPIN_LOCK_IRQ(&priv->devlock, irqflags);

		ctrl = REG_READ(&regs->ctrl);
		ctrl &= ~GRSPW_CTRL_TF; /* Depends on one to one relation between
					 * irqopts bits and ctrl bits */
		ctrl |= (*options & ICOPTS_CTRL_MASK) <<
			(GRSPW_CTRL_TF_BIT - 0);

		icctrl = REG_READ(&regs->icctrl);
		icctrl &= ~ICOPTS_ICCTRL_MASK; /* Depends on one to one relation between
						* irqopts bits and icctrl bits */
		icctrl |= *options & ICOPTS_ICCTRL_MASK;

		/* Enable Global IRQ only if some irq source is set */
		if (grspw_is_irqsource_set(ctrl, icctrl))
			ctrl |= GRSPW_CTRL_IE;
		else
			ctrl &= ~GRSPW_CTRL_IE;

		REG_WRITE(&regs->ctrl, ctrl);
		REG_WRITE(&regs->icctrl, icctrl);
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
	}
	*options = ((REG_READ(&regs->ctrl) & ICOPTS_CTRL_MASK) |
		    (REG_READ(&regs->icctrl) & ICOPTS_ICCTRL_MASK));
}

void grspw_ic_config(void *d, int rw, struct spwpkt_ic_config *cfg)
{
	struct grspw_priv *priv = d;
	struct grspw_regs *regs = priv->regs;

	if (!cfg)
		return;

	if (rw & 1) {
		REG_WRITE(&regs->ictickomask, cfg->tomask);
		REG_WRITE(&regs->icaamask, cfg->aamask);
		REG_WRITE(&regs->icrlpresc, cfg->scaler);
		REG_WRITE(&regs->icrlisr, cfg->isr_reload);
		REG_WRITE(&regs->icrlintack, cfg->ack_reload);
	}
	if (rw & 2) {
		cfg->tomask = REG_READ(&regs->ictickomask);
		cfg->aamask = REG_READ(&regs->icaamask);
		cfg->scaler = REG_READ(&regs->icrlpresc);
		cfg->isr_reload = REG_READ(&regs->icrlisr);
		cfg->ack_reload = REG_READ(&regs->icrlintack);
	}
}

/* Read or Write Interrupt-code status registers */
void grspw_ic_sts(void *d, unsigned int *rxirq, unsigned int *rxack, unsigned int *intto)
{
	struct grspw_priv *priv = d;
	struct grspw_regs *regs = priv->regs;

	/* No locking needed since the status bits are clear-on-write */

	if (rxirq) {
		if (*rxirq != 0)
			REG_WRITE(&regs->icrx, *rxirq);
		else
			*rxirq = REG_READ(&regs->icrx);
	}

	if (rxack) {
		if (*rxack != 0)
			REG_WRITE(&regs->icack, *rxack);
		else
			*rxack = REG_READ(&regs->icack);
	}

	if (intto) {
		if (*intto != 0)
			REG_WRITE(&regs->ictimeout, *intto);
		else
			*intto = REG_READ(&regs->ictimeout);
	}
}

/* Assign handler function to Interrupt-code tick out IRQ */
void grspw_ic_isr(void *d, spwpkt_ic_isr_t handler, void *data)
{
	struct grspw_priv *priv = d;

	priv->icisr_arg = data;
	priv->icisr = handler;
}

/* Set (not -1) and/or read RMAP options. */
int grspw_rmap_ctrl(void *d, int *options, int *dstkey)
{
	struct grspw_priv *priv = d;
	struct grspw_regs *regs = priv->regs;
	unsigned int ctrl;
	IRQFLAGS_TYPE irqflags;

	if (dstkey) {
		if (*dstkey != -1)
			REG_WRITE(&regs->destkey, *dstkey & GRSPW_DK_DESTKEY);
		*dstkey = REG_READ(&regs->destkey) & GRSPW_DK_DESTKEY;
	}
	if (options) {
		if (*options != -1) {
			if ((*options & RMAPOPTS_EN_RMAP) && !priv->hwsup.rmap)
				return -1;


			SPIN_LOCK_IRQ(&priv->devlock, irqflags);
			ctrl = REG_READ(&regs->ctrl);
			ctrl &= ~(GRSPW_CTRL_RE|GRSPW_CTRL_RD);
			ctrl |= (*options & 0x3) << GRSPW_CTRL_RE_BIT;
			REG_WRITE(&regs->ctrl, ctrl);
			SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
		}
		*options = (REG_READ(&regs->ctrl) >> GRSPW_CTRL_RE_BIT) & 0x3;
	}

	return 0;
}

void grspw_rmap_support(void *d, char *rmap, char *rmap_crc)
{
	struct grspw_priv *priv = d;

	if (rmap)
		*rmap = priv->hwsup.rmap;
	if (rmap_crc)
		*rmap_crc = priv->hwsup.rmap_crc;
}

/* Select port, if 
 * -1=The current selected port is returned
 * 0=Port 0
 * 1=Port 1
 * Others=Both Port0 and Port1
 */
int grspw_port_ctrl(void *d, int *port)
{
	struct grspw_priv *priv = d;
	struct grspw_regs *regs = priv->regs;
	unsigned int ctrl;
	IRQFLAGS_TYPE irqflags;

	if (port == NULL)
		return -1;

	if ((*port == 1) || (*port == 0)) {
		/* Select port user selected */
		if ((*port == 1) && (priv->hwsup.nports < 2))
			return -1; /* Changing to Port 1, but only one port available */
		SPIN_LOCK_IRQ(&priv->devlock, irqflags);
		ctrl = REG_READ(&regs->ctrl);
		ctrl &= ~(GRSPW_CTRL_NP | GRSPW_CTRL_PS);
		ctrl |= (*port & 1) << GRSPW_CTRL_PS_BIT;
		REG_WRITE(&regs->ctrl, ctrl);
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
	} else if (*port > 1) {
		/* Select both ports */
		SPIN_LOCK_IRQ(&priv->devlock, irqflags);
		REG_WRITE(&regs->ctrl, REG_READ(&regs->ctrl) | GRSPW_CTRL_NP);
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
	}

	/* Get current settings */
	ctrl = REG_READ(&regs->ctrl);
	if (ctrl & GRSPW_CTRL_NP) {
		/* Any port, selected by hardware */
		if (priv->hwsup.nports > 1)
			*port = 3;
		else
			*port = 0; /* Port0 the only port available */
	} else {
		*port = (ctrl & GRSPW_CTRL_PS) >> GRSPW_CTRL_PS_BIT;
	}

	return 0;
}

/* Returns Number ports available in hardware */
int grspw_port_count(void *d)
{
	struct grspw_priv *priv = d;

	return priv->hwsup.nports;
}

/* Current active port: 0 or 1 */
int grspw_port_active(void *d)
{
	struct grspw_priv *priv = d;
	unsigned int status;

	status = REG_READ(&priv->regs->status);

	return (status & GRSPW_STS_AP) >> GRSPW_STS_AP_BIT;
}

void grspw_stats_read(void *d, struct grspw_core_stats *sts)
{
	struct grspw_priv *priv = d;

	if (sts == NULL)
		return;
	memcpy(sts, &priv->stats, sizeof(priv->stats));
}

void grspw_stats_clr(void *d)
{
	struct grspw_priv *priv = d;

	/* Clear most of the statistics */	
	memset(&priv->stats, 0, sizeof(priv->stats));
}

/*** DMA Interface ***/

/* Initialize the RX and TX Descriptor Ring, empty of packets */
STATIC void grspw_bdrings_init(struct grspw_dma_priv *dma)
{
	struct grspw_ring *r;
	int i;

	/* Empty BD rings */
	dma->rx_ring_head = dma->rx_ring_base;
	dma->rx_ring_tail = dma->rx_ring_base;
	dma->tx_ring_head = dma->tx_ring_base;
	dma->tx_ring_tail = dma->tx_ring_base;

	/* Init RX Descriptors */
	r = (struct grspw_ring *)dma->rx_ring_base;
	for (i=0; i<GRSPW_RXBD_NR; i++) {

		/* Init Ring Entry */
		r[i].next = &r[i+1];
		r[i].bd.rx = &dma->rx_bds[i];
		r[i].pkt = NULL;

		/* Init HW Descriptor */
		BD_WRITE(&r[i].bd.rx->ctrl, 0);
		BD_WRITE(&r[i].bd.rx->addr, 0);
	}
	r[GRSPW_RXBD_NR-1].next = &r[0];

	/* Init TX Descriptors */
	r = (struct grspw_ring *)dma->tx_ring_base;
	for (i=0; i<GRSPW_TXBD_NR; i++) {

		/* Init Ring Entry */
		r[i].next = &r[i+1];
		r[i].bd.tx = &dma->tx_bds[i];
		r[i].pkt = NULL;

		/* Init HW Descriptor */
		BD_WRITE(&r[i].bd.tx->ctrl, 0);
		BD_WRITE(&r[i].bd.tx->haddr, 0);
		BD_WRITE(&r[i].bd.tx->dlen, 0);
		BD_WRITE(&r[i].bd.tx->daddr, 0);
	}
	r[GRSPW_TXBD_NR-1].next = &r[0];
}

/* Try to populate descriptor ring with as many as possible READY unused packet
 * buffers. The packets assigned with to a descriptor are put in the end of 
 * the scheduled list.
 *
 * The number of Packets scheduled is returned.
 *
 *  - READY List -> RX-SCHED List
 *  - Descriptors are initialized and enabled for reception
 */
STATIC int grspw_rx_schedule_ready(struct grspw_dma_priv *dma)
{
	int cnt;
	unsigned int ctrl, dmactrl;
	void *hwaddr;
	struct grspw_rxring *curr_bd;
	struct grspw_pkt *curr_pkt, *last_pkt;
	struct grspw_list lst;
	IRQFLAGS_TYPE irqflags;

	/* Is Ready Q empty? */
	if (grspw_list_is_empty(&dma->ready))
		return 0;

	cnt = 0;
	lst.head = curr_pkt = dma->ready.head;
	curr_bd = dma->rx_ring_head;
	while (!curr_bd->pkt) {

		/* Assign Packet to descriptor */
		curr_bd->pkt = curr_pkt;

		/* Prepare descriptor address. */
		hwaddr = curr_pkt->data;
		if (curr_pkt->flags & PKT_FLAG_TR_DATA) {
			drvmgr_translate(dma->core->dev, CPUMEM_TO_DMA,
					 hwaddr, &hwaddr);
			if (curr_pkt->data == hwaddr) /* translation needed? */
				curr_pkt->flags &= ~PKT_FLAG_TR_DATA;
		}
		BD_WRITE(&curr_bd->bd->addr, hwaddr);

		ctrl = GRSPW_RXBD_EN;
		if (curr_bd->next == dma->rx_ring_base) {
			/* Wrap around (only needed when smaller descriptor
			 * table)
			 */
			ctrl |= GRSPW_RXBD_WR;
		}

		/* Is this Packet going to be an interrupt Packet? */
		if ((--dma->rx_irq_en_cnt_curr) <= 0) {
			if (dma->cfg.rx_irq_en_cnt == 0) {
				/* IRQ is disabled. A big number to avoid
				 * equal to zero too often
				 */
				dma->rx_irq_en_cnt_curr = 0x3fffffff;
			} else {
				dma->rx_irq_en_cnt_curr = dma->cfg.rx_irq_en_cnt;
				ctrl |= GRSPW_RXBD_IE;
			}
		}

		if (curr_pkt->flags & RXPKT_FLAG_IE)
			ctrl |= GRSPW_RXBD_IE;

		/* Enable descriptor */
		BD_WRITE(&curr_bd->bd->ctrl, ctrl);

		last_pkt = curr_pkt;
		curr_bd = curr_bd->next;
		cnt++;

		/* Get Next Packet from Ready Queue */
		if (curr_pkt == dma->ready.tail) {
			/* Handled all in ready queue. */
			curr_pkt = NULL;
			break;
		}
		curr_pkt = curr_pkt->next;
	}

	/* Has Packets been scheduled? */
	if (cnt > 0) {
		/* Prepare list for insertion/deleation */
		lst.tail = last_pkt;

		/* Remove scheduled packets from ready queue */
		grspw_list_remove_head_list(&dma->ready, &lst);
		dma->ready_cnt -= cnt;
		if (dma->stats.ready_cnt_min > dma->ready_cnt)
			dma->stats.ready_cnt_min = dma->ready_cnt;

		/* Insert scheduled packets into scheduled queue */
		grspw_list_append_list(&dma->rx_sched, &lst);
		dma->rx_sched_cnt += cnt;
		if (dma->stats.rx_sched_cnt_max < dma->rx_sched_cnt)
			dma->stats.rx_sched_cnt_max = dma->rx_sched_cnt;

		/* Update TX ring posistion */
		dma->rx_ring_head = curr_bd;

		/* Make hardware aware of the newly enabled descriptors 
		 * We must protect from ISR which writes RI|TI
		 */
		SPIN_LOCK_IRQ(&dma->core->devlock, irqflags);
		dmactrl = REG_READ(&dma->regs->ctrl);
		dmactrl &= ~(GRSPW_DMACTRL_PS|GRSPW_DMACTRL_PR|GRSPW_DMA_STATUS_ERROR);
		dmactrl |= GRSPW_DMACTRL_RE | GRSPW_DMACTRL_RD;
		REG_WRITE(&dma->regs->ctrl, dmactrl);
		SPIN_UNLOCK_IRQ(&dma->core->devlock, irqflags);
	}

	return cnt;
}

/* Scans the RX desciptor table for scheduled Packet that has been received,
 * and moves these Packet from the head of the scheduled queue to the
 * tail of the recv queue.
 *
 * Also, for all packets the status is updated.
 *
 *  - SCHED List -> SENT List
 *
 * Return Value
 * Number of packets moved
 */
STATIC int grspw_rx_process_scheduled(struct grspw_dma_priv *dma)
{
	struct grspw_rxring *curr;
	struct grspw_pkt *last_pkt;
	int recv_pkt_cnt = 0;
	unsigned int ctrl;
	struct grspw_list lst;

	curr = dma->rx_ring_tail;

	/* Step into RX ring to find if packets have been scheduled for 
	 * reception.
	 */
	if (!curr->pkt)
		return 0; /* No scheduled packets, thus no received, abort */

	/* There has been Packets scheduled ==> scheduled Packets may have been
	 * received and needs to be collected into RECV List.
	 *
	 * A temporary list "lst" with all received packets is created.
	 */
	lst.head = curr->pkt;

	/* Loop until first enabled "unrecveived" SpW Packet is found.
	 * An unused descriptor is indicated by an unassigned pkt field.
	 */
	while (curr->pkt && !((ctrl=BD_READ(&curr->bd->ctrl)) & GRSPW_RXBD_EN)) {
		/* Handle one received Packet */

		/* Remember last handled Packet so that insertion/removal from
		 * Packet lists go fast.
		 */
		last_pkt = curr->pkt;

		/* Get Length of Packet in bytes, and reception options */
		last_pkt->dlen = (ctrl & GRSPW_RXBD_LEN) >> GRSPW_RXBD_LEN_BIT;

		/* Set flags to indicate error(s) and CRC information,
		 * and Mark Received.
		 */
		last_pkt->flags = (last_pkt->flags & ~RXPKT_FLAG_OUTPUT_MASK) |
		                  ((ctrl >> 20) & RXPKT_FLAG_OUTPUT_MASK) |
		                  RXPKT_FLAG_RX;

		/* Packet was Truncated? */
		if (ctrl & GRSPW_RXBD_TR)
			dma->stats.rx_err_trunk++;

		/* Error End-Of-Packet? */
		if (ctrl & GRSPW_RXBD_EP)
			dma->stats.rx_err_endpkt++;
		curr->pkt = NULL; /* Mark descriptor unused */

		/* Increment */
		curr = curr->next;
		recv_pkt_cnt++;
	}

	/* 1. Remove all handled packets from scheduled queue
	 * 2. Put all handled packets into recv queue
	 */
	if (recv_pkt_cnt > 0) {

		/* Update Stats, Number of Received Packets */
		dma->stats.rx_pkts += recv_pkt_cnt;

		/* Save RX ring posistion */
		dma->rx_ring_tail = curr;

		/* Prepare list for insertion/deleation */
		lst.tail = last_pkt;

		/* Remove received Packets from RX-SCHED queue */
		grspw_list_remove_head_list(&dma->rx_sched, &lst);
		dma->rx_sched_cnt -= recv_pkt_cnt;
		if (dma->stats.rx_sched_cnt_min > dma->rx_sched_cnt)
			dma->stats.rx_sched_cnt_min = dma->rx_sched_cnt;

		/* Insert received Packets into RECV queue */
		grspw_list_append_list(&dma->recv, &lst);
		dma->recv_cnt += recv_pkt_cnt;
		if (dma->stats.recv_cnt_max < dma->recv_cnt)
			dma->stats.recv_cnt_max = dma->recv_cnt;
	}

	return recv_pkt_cnt;
}

/* Try to populate descriptor ring with as many SEND packets as possible. The
 * packets assigned with to a descriptor are put in the end of 
 * the scheduled list.
 *
 * The number of Packets scheduled is returned.
 *
 *  - SEND List -> TX-SCHED List
 *  - Descriptors are initialized and enabled for transmission
 */
STATIC int grspw_tx_schedule_send(struct grspw_dma_priv *dma)
{
	int cnt;
	unsigned int ctrl, dmactrl;
	void *hwaddr;
	struct grspw_txring *curr_bd;
	struct grspw_pkt *curr_pkt, *last_pkt;
	struct grspw_list lst;
	IRQFLAGS_TYPE irqflags;

	/* Is Ready Q empty? */
	if (grspw_list_is_empty(&dma->send))
		return 0;

	cnt = 0;
	lst.head = curr_pkt = dma->send.head;
	curr_bd = dma->tx_ring_head;
	while (!curr_bd->pkt) {

		/* Assign Packet to descriptor */
		curr_bd->pkt = curr_pkt;

		/* Set up header transmission */
		if (curr_pkt->hdr && curr_pkt->hlen) {
			hwaddr = curr_pkt->hdr;
			if (curr_pkt->flags & PKT_FLAG_TR_HDR) {
				drvmgr_translate(dma->core->dev, CPUMEM_TO_DMA,
						 hwaddr, &hwaddr);
				/* translation needed? */
				if (curr_pkt->hdr == hwaddr)
					curr_pkt->flags &= ~PKT_FLAG_TR_HDR;
			}
			BD_WRITE(&curr_bd->bd->haddr, hwaddr);
			ctrl = GRSPW_TXBD_EN | curr_pkt->hlen;
		} else {
			ctrl = GRSPW_TXBD_EN;
		}
		/* Enable IRQ generation and CRC options as specified
		 * by user.
		 */
		ctrl |= (curr_pkt->flags & TXPKT_FLAG_INPUT_MASK) << 8;

		if (curr_bd->next == dma->tx_ring_base) {
			/* Wrap around (only needed when smaller descriptor table) */
			ctrl |= GRSPW_TXBD_WR;
		}

		/* Is this Packet going to be an interrupt Packet? */
		if ((--dma->tx_irq_en_cnt_curr) <= 0) {
			if (dma->cfg.tx_irq_en_cnt == 0) {
				/* IRQ is disabled.
				 * A big number to avoid equal to zero too often 
				 */
				dma->tx_irq_en_cnt_curr = 0x3fffffff;
			} else {
				dma->tx_irq_en_cnt_curr = dma->cfg.tx_irq_en_cnt;
				ctrl |= GRSPW_TXBD_IE;
			}
		}

		/* Prepare descriptor address. Parts of CTRL is written to
		 * DLEN for debug-only (CTRL is cleared by HW).
		 */
		if (curr_pkt->data && curr_pkt->dlen) {
			hwaddr = curr_pkt->data;
			if (curr_pkt->flags & PKT_FLAG_TR_DATA) {
				drvmgr_translate(dma->core->dev, CPUMEM_TO_DMA,
						 hwaddr, &hwaddr);
				/* translation needed? */
				if (curr_pkt->data == hwaddr)
					curr_pkt->flags &= ~PKT_FLAG_TR_DATA;
			}
			BD_WRITE(&curr_bd->bd->daddr, hwaddr);
			BD_WRITE(&curr_bd->bd->dlen, curr_pkt->dlen |
			                             ((ctrl & 0x3f000) << 12));
		} else {
			BD_WRITE(&curr_bd->bd->daddr, 0);
			BD_WRITE(&curr_bd->bd->dlen, ((ctrl & 0x3f000) << 12));
		}

		/* Enable descriptor */
		BD_WRITE(&curr_bd->bd->ctrl, ctrl);

		last_pkt = curr_pkt;
		curr_bd = curr_bd->next;
		cnt++;

		/* Get Next Packet from Ready Queue */
		if (curr_pkt == dma->send.tail) {
			/* Handled all in ready queue. */
			curr_pkt = NULL;
			break;
		}
		curr_pkt = curr_pkt->next;
	}

	/* Have Packets been scheduled? */
	if (cnt > 0) {
		/* Prepare list for insertion/deleation */
		lst.tail = last_pkt;

		/* Remove scheduled packets from ready queue */
		grspw_list_remove_head_list(&dma->send, &lst);
		dma->send_cnt -= cnt;
		if (dma->stats.send_cnt_min > dma->send_cnt)
			dma->stats.send_cnt_min = dma->send_cnt;

		/* Insert scheduled packets into scheduled queue */
		grspw_list_append_list(&dma->tx_sched, &lst);
		dma->tx_sched_cnt += cnt;
		if (dma->stats.tx_sched_cnt_max < dma->tx_sched_cnt)
			dma->stats.tx_sched_cnt_max = dma->tx_sched_cnt;

		/* Update TX ring posistion */
		dma->tx_ring_head = curr_bd;

		/* Make hardware aware of the newly enabled descriptors */
		SPIN_LOCK_IRQ(&dma->core->devlock, irqflags);
		dmactrl = REG_READ(&dma->regs->ctrl);
		dmactrl &= ~(GRSPW_DMACTRL_PS|GRSPW_DMACTRL_PR|GRSPW_DMA_STATUS_ERROR);
		dmactrl |= GRSPW_DMACTRL_TE;
		REG_WRITE(&dma->regs->ctrl, dmactrl);
		SPIN_UNLOCK_IRQ(&dma->core->devlock, irqflags);
	}
	return cnt;
}

/* Scans the TX desciptor table for transmitted packets, and moves these 
 * packets from the head of the scheduled queue to the tail of the sent queue.
 *
 * Also, for all packets the status is updated.
 *
 *  - SCHED List -> SENT List
 *
 * Return Value
 * Number of packet moved
 */
STATIC int grspw_tx_process_scheduled(struct grspw_dma_priv *dma)
{
	struct grspw_txring *curr;
	struct grspw_pkt *last_pkt;
	int sent_pkt_cnt = 0;
	unsigned int ctrl;
	struct grspw_list lst;

	curr = dma->tx_ring_tail;

	/* Step into TX ring to find if packets have been scheduled for 
	 * transmission.
	 */
	if (!curr->pkt)
		return 0; /* No scheduled packets, thus no sent, abort */

	/* There has been Packets scheduled ==> scheduled Packets may have been
	 * transmitted and needs to be collected into SENT List.
	 *
	 * A temporary list "lst" with all sent packets is created.
	 */
	lst.head = curr->pkt;

	/* Loop until first enabled "un-transmitted" SpW Packet is found.
	 * An unused descriptor is indicated by an unassigned pkt field.
	 */
	while (curr->pkt && !((ctrl=BD_READ(&curr->bd->ctrl)) & GRSPW_TXBD_EN)) {
		/* Handle one sent Packet */

		/* Remember last handled Packet so that insertion/removal from
		 * packet lists go fast.
		 */
		last_pkt = curr->pkt;

		/* Set flags to indicate error(s) and Mark Sent.
		 */
		last_pkt->flags = (last_pkt->flags & ~TXPKT_FLAG_OUTPUT_MASK) |
					(ctrl & TXPKT_FLAG_LINKERR) |
					TXPKT_FLAG_TX;

		/* Sent packet experienced link error? */
		if (ctrl & GRSPW_TXBD_LE)
			dma->stats.tx_err_link++;

		curr->pkt = NULL; /* Mark descriptor unused */

		/* Increment */
		curr = curr->next;
		sent_pkt_cnt++;
	}

	/* 1. Remove all handled packets from TX-SCHED queue
	 * 2. Put all handled packets into SENT queue
	 */
	if (sent_pkt_cnt > 0) {
		/* Update Stats, Number of Transmitted Packets */
		dma->stats.tx_pkts += sent_pkt_cnt;

		/* Save TX ring posistion */
		dma->tx_ring_tail = curr;

		/* Prepare list for insertion/deleation */
		lst.tail = last_pkt;

		/* Remove sent packets from TX-SCHED queue */
		grspw_list_remove_head_list(&dma->tx_sched, &lst);
		dma->tx_sched_cnt -= sent_pkt_cnt;
		if (dma->stats.tx_sched_cnt_min > dma->tx_sched_cnt)
			dma->stats.tx_sched_cnt_min = dma->tx_sched_cnt;

		/* Insert received packets into SENT queue */
		grspw_list_append_list(&dma->sent, &lst);
		dma->sent_cnt += sent_pkt_cnt;
		if (dma->stats.sent_cnt_max < dma->sent_cnt)
			dma->stats.sent_cnt_max = dma->sent_cnt;
	}

	return sent_pkt_cnt;
}

void *grspw_dma_open(void *d, int chan_no)
{
	struct grspw_priv *priv = d;
	struct grspw_dma_priv *dma;
	int size;

	if ((chan_no < 0) && (priv->hwsup.ndma_chans <= chan_no))
		return NULL;

	dma = &priv->dma[chan_no];

	/* Take GRSPW lock */
	if (rtems_semaphore_obtain(grspw_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL)
		return NULL;

	if (dma->open) {
		dma = NULL;
		goto out;
	}

	dma->started = 0;

	/* Set Default Configuration:
	 *
	 *  - MAX RX Packet Length = 
	 *  - Disable IRQ generation
	 *  -
	 */
	dma->cfg.rxmaxlen = DEFAULT_RXMAX;
	dma->cfg.rx_irq_en_cnt = 0;
	dma->cfg.tx_irq_en_cnt = 0;
	dma->cfg.flags = DMAFLAG_NO_SPILL;

	/* DMA Channel Semaphore created with count = 1 */
	if (rtems_semaphore_create(
	    rtems_build_name('S', 'D', '0' + priv->index, '0' + chan_no), 1,
	    RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | \
	    RTEMS_NO_INHERIT_PRIORITY | RTEMS_LOCAL | \
	    RTEMS_NO_PRIORITY_CEILING, 0, &dma->sem_dma) != RTEMS_SUCCESSFUL) {
		dma = NULL;
		goto out;
	}

	/* Allocate memory for the two descriptor rings */
	size = sizeof(struct grspw_ring) * (GRSPW_RXBD_NR + GRSPW_TXBD_NR);
	dma->rx_ring_base = (struct grspw_rxring *)malloc(size);
	dma->tx_ring_base = (struct grspw_txring *)&dma->rx_ring_base[GRSPW_RXBD_NR];
	if (dma->rx_ring_base == NULL) {
		dma = NULL;
		goto out;
	}

	/* Create DMA RX and TX Channel sempahore with count = 0 */
	if (rtems_semaphore_create(
	    rtems_build_name('S', 'R', '0' + priv->index, '0' + chan_no), 0,
	    RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | \
	    RTEMS_NO_INHERIT_PRIORITY | RTEMS_LOCAL | \
	    RTEMS_NO_PRIORITY_CEILING, 0, &dma->rx_wait.sem_wait) != RTEMS_SUCCESSFUL) {
		dma = NULL;
		goto out;
	}
	if (rtems_semaphore_create(
	    rtems_build_name('S', 'T', '0' + priv->index, '0' + chan_no), 0,
	    RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | \
	    RTEMS_NO_INHERIT_PRIORITY | RTEMS_LOCAL | \
	    RTEMS_NO_PRIORITY_CEILING, 0, &dma->tx_wait.sem_wait) != RTEMS_SUCCESSFUL) {
		dma = NULL;
		goto out;
	}

	/* Reset software structures */
	grspw_dma_reset(dma);

	/* Take the device */
	dma->open = 1;
out:
	/* Return GRSPW Lock */
	rtems_semaphore_release(grspw_sem);

	return dma;
}

/* Initialize Software Structures:
 *  - Clear all Queues 
 *  - init BD ring 
 *  - init IRQ counter
 *  - clear statistics counters
 *  - init wait structures and semaphores
 */
STATIC void grspw_dma_reset(struct grspw_dma_priv *dma)
{
	/* Empty RX and TX queues */
	grspw_list_clr(&dma->ready);
	grspw_list_clr(&dma->rx_sched);
	grspw_list_clr(&dma->recv);
	grspw_list_clr(&dma->send);
	grspw_list_clr(&dma->tx_sched);
	grspw_list_clr(&dma->sent);
	dma->ready_cnt = 0;
	dma->rx_sched_cnt = 0;
	dma->recv_cnt = 0;
	dma->send_cnt = 0;
	dma->tx_sched_cnt = 0;
	dma->sent_cnt = 0;

	dma->rx_irq_en_cnt_curr = 0;
	dma->tx_irq_en_cnt_curr = 0;

	grspw_bdrings_init(dma);

	dma->rx_wait.waiting = 0;
	dma->tx_wait.waiting = 0;

	grspw_dma_stats_clr(dma);
}

void grspw_dma_close(void *c)
{
	struct grspw_dma_priv *dma = c;

	if (!dma->open)
		return;

	/* Take device lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(dma->sem_dma, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL)
		return;

	grspw_dma_stop_locked(dma);

	/* Free resources */
	rtems_semaphore_delete(dma->rx_wait.sem_wait);
	rtems_semaphore_delete(dma->tx_wait.sem_wait);
	rtems_semaphore_delete(dma->sem_dma); /* Release and delete lock */

	/* Free memory */
	if (dma->rx_ring_base)
		free(dma->rx_ring_base);
	dma->rx_ring_base = NULL;
	dma->tx_ring_base = NULL;

	dma->open = 0;
}

/* Schedule List of packets for transmission at some point in
 * future.
 *
 * 1. Move transmitted packets to SENT List (SCHED->SENT)
 * 2. Add the requested packets to the SEND List (USER->SEND)
 * 3. Schedule as many packets as possible (SEND->SCHED)
 */
int grspw_dma_tx_send(void *c, int opts, struct grspw_list *pkts, int count)
{
	struct grspw_dma_priv *dma = c;
	int ret;

	/* Take DMA channel lock */
	if (rtems_semaphore_obtain(dma->sem_dma, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL)
		return -1;

	if (dma->started == 0) {
		ret = 1; /* signal DMA has been stopped */
		goto out;
	}
	ret = 0;

	/* 1. Move transmitted packets to SENT List (SCHED->SENT) */
	if ((opts & 1) == 0)
		grspw_tx_process_scheduled(dma);

	/* 2. Add the requested packets to the SEND List (USER->SEND) */
	if (pkts) {
		grspw_list_append_list(&dma->send, pkts);
		dma->send_cnt += count;
		if (dma->stats.send_cnt_max < dma->send_cnt)
			dma->stats.send_cnt_max = dma->send_cnt;
	}

	/* 3. Schedule as many packets as possible (SEND->SCHED) */
	if ((opts & 2) == 0)
		grspw_tx_schedule_send(dma);

out:
	/* Unlock DMA channel */
	rtems_semaphore_release(dma->sem_dma);

	return ret;
}

int grspw_dma_tx_reclaim(void *c, int opts, struct grspw_list *pkts, int *count)
{
	struct grspw_dma_priv *dma = c;
	struct grspw_pkt *pkt, *lastpkt;
	int cnt, started;

	/* Take DMA channel lock */
	if (rtems_semaphore_obtain(dma->sem_dma, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL)
		return -1;

	/* 1. Move transmitted packets to SENT List (SCHED->SENT) */
	started = dma->started;
	if ((started > 0) && ((opts & 1) == 0))
		grspw_tx_process_scheduled(dma);

	/* Move all/count SENT packet to the callers list (SENT->USER) */
	if (pkts) {
		if ((count == NULL) || (*count == -1) ||
		    (*count >= dma->sent_cnt)) {
			/* Move all SENT Packets */
			*pkts = dma->sent;
			grspw_list_clr(&dma->sent);
			if (count)
				*count = dma->sent_cnt;
			dma->sent_cnt = 0;
		} else {
			/* Move a number of SENT Packets */
			pkts->head = pkt = lastpkt = dma->sent.head;
			cnt = 0;
			while (cnt < *count) {
				lastpkt = pkt;
				pkt = pkt->next;
				cnt++;
			}
			if (cnt > 0) {
				pkts->tail = lastpkt;
				grspw_list_remove_head_list(&dma->sent, pkts);
				dma->sent_cnt -= cnt;
			} else {
				grspw_list_clr(pkts);
			}
		}
	} else if (count) {
		*count = 0;
	}

	/* 3. Schedule as many packets as possible (SEND->SCHED) */
	if ((started > 0 ) && ((opts & 2) == 0))
		grspw_tx_schedule_send(dma);

	/* Unlock DMA channel */
	rtems_semaphore_release(dma->sem_dma);

	return (~started) & 1; /* signal DMA has been stopped */
}

void grspw_dma_tx_count(void *c, int *send, int *sched, int *sent)
{
	struct grspw_dma_priv *dma = c;

	if (send)
		*send = dma->send_cnt;
	if (sched)
		*sched = dma->tx_sched_cnt;
	if (sent)
		*sent = dma->sent_cnt;
}

static inline int grspw_tx_wait_eval(struct grspw_dma_priv *dma)
{
	int send_val, sent_val;

	if (dma->tx_wait.send_cnt >= (dma->send_cnt + dma->tx_sched_cnt))
		send_val = 1;
	else
		send_val = 0;

	if (dma->tx_wait.sent_cnt <= dma->sent_cnt)
		sent_val = 1;
	else
		sent_val = 0;

	/* AND or OR ? */
	if (dma->tx_wait.op == 0)
		return send_val & sent_val; /* AND */
	else
		return send_val | sent_val; /* OR */
}

/* Block until send_cnt or fewer packets are Queued in "Send and Scheduled" Q,
 * op (AND or OR), sent_cnt or more packet "have been sent" (Sent Q) condition
 * is met.
 * If a link error occurs and the Stop on Link error is defined, this function
 * will also return to caller.
 */
int grspw_dma_tx_wait(void *c, int send_cnt, int op, int sent_cnt, int timeout)
{
	struct grspw_dma_priv *dma = c;
	int ret, rc;

	if (timeout == 0)
		timeout = RTEMS_NO_TIMEOUT;

check_condition:

	/* Take DMA channel lock */
	if (rtems_semaphore_obtain(dma->sem_dma, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL)
		return -1;

	/* Check so that no other thread is waiting, this driver only supports
	 * one waiter at a time.
	 */
	if (dma->tx_wait.waiting) {
		ret = -1;
		goto out;
	}

	/* Stop if link error or similar, abort */
	if (dma->started == 0) {
		ret = 1;
		goto out;
	}

	/* Set up Condition */
	dma->tx_wait.send_cnt = send_cnt;
	dma->tx_wait.op = op;
	dma->tx_wait.sent_cnt = sent_cnt;

	if (grspw_tx_wait_eval(dma) == 0) {
		/* Prepare Wait */
		dma->tx_wait.waiting = 1;

		/* Release DMA channel lock */
		rtems_semaphore_release(dma->sem_dma);

		/* Try to take Wait lock, if this fail link may have gone down
		 * or user stopped this DMA channel
		 */
		rc = rtems_semaphore_obtain(dma->tx_wait.sem_wait, RTEMS_WAIT,
						timeout);
		if (rc == RTEMS_TIMEOUT) {
			dma->tx_wait.waiting = 0;
			return 2;
		} else if (rc == RTEMS_UNSATISFIED ||
		           rc == RTEMS_OBJECT_WAS_DELETED) {
			dma->tx_wait.waiting = 0;
			return 1; /* sem was flushed/deleted, means DMA stop */
		} else if (rc != RTEMS_SUCCESSFUL)
		    	return -1;

		/* Check condition once more */
		goto check_condition;
	} else {
		/* No Wait needed */
		dma->tx_wait.waiting = 0;
	}

	ret = 0;
out:
	/* Unlock DMA channel */
	rtems_semaphore_release(dma->sem_dma);

	return ret;
}

int grspw_dma_rx_recv(void *c, int opts, struct grspw_list *pkts, int *count)
{
	struct grspw_dma_priv *dma = c;
	struct grspw_pkt *pkt, *lastpkt;
	int cnt, started;

	/* Take DMA channel lock */
	if (rtems_semaphore_obtain(dma->sem_dma, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL)
		return -1;

	/* 1. Move Scheduled packets to RECV List (SCHED->RECV) */
	started = dma->started;
	if (((opts & 1) == 0) && (started > 0))
		grspw_rx_process_scheduled(dma);

	/* Move all RECV packet to the callers list */
	if (pkts) {
		if ((count == NULL) || (*count == -1) ||
		    (*count >= dma->recv_cnt)) {
			/* Move all Received packets */
			*pkts = dma->recv;
			grspw_list_clr(&dma->recv);
			if ( count )
				*count = dma->recv_cnt;
			dma->recv_cnt = 0;
		} else {
			/* Move a number of RECV Packets */
			pkts->head = pkt = lastpkt = dma->recv.head;
			cnt = 0;
			while (cnt < *count) {
				lastpkt = pkt;
				pkt = pkt->next;
				cnt++;
			}
			if (cnt > 0) {
				pkts->tail = lastpkt;
				grspw_list_remove_head_list(&dma->recv, pkts);
				dma->recv_cnt -= cnt;
			} else {
				grspw_list_clr(pkts);
			}
		}
	} else if (count) {
		*count = 0;
	}

	/* 3. Schedule as many free packet buffers as possible (READY->SCHED) */
	if (((opts & 2) == 0) && (started > 0))
		grspw_rx_schedule_ready(dma);

	/* Unlock DMA channel */
	rtems_semaphore_release(dma->sem_dma);

	return (~started) & 1;
}

int grspw_dma_rx_prepare(void *c, int opts, struct grspw_list *pkts, int count)
{
	struct grspw_dma_priv *dma = c;
	int ret;

	/* Take DMA channel lock */
	if (rtems_semaphore_obtain(dma->sem_dma, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL)
		return -1;

	if (dma->started == 0) {
		ret = 1;
		goto out;
	}

	/* 1. Move Received packets to RECV List (SCHED->RECV) */
	if ((opts & 1) == 0)
		grspw_rx_process_scheduled(dma);

	/* 2. Add the "free/ready" packet buffers to the READY List (USER->READY) */
	if (pkts && (count > 0)) {
		grspw_list_append_list(&dma->ready, pkts);
		dma->ready_cnt += count;
		if (dma->stats.ready_cnt_max < dma->ready_cnt)
			dma->stats.ready_cnt_max = dma->ready_cnt;
	}

	/* 3. Schedule as many packets as possible (READY->SCHED) */
	if ((opts & 2) == 0)
		grspw_rx_schedule_ready(dma);

	ret = 0;
out:
	/* Unlock DMA channel */
	rtems_semaphore_release(dma->sem_dma);

	return ret;
}

void grspw_dma_rx_count(void *c, int *ready, int *sched, int *recv)
{
	struct grspw_dma_priv *dma = c;

	if (ready)
		*ready = dma->ready_cnt;
	if (sched)
		*sched = dma->rx_sched_cnt;
	if (recv)
		*recv = dma->recv_cnt;
}

static inline int grspw_rx_wait_eval(struct grspw_dma_priv *dma)
{
	int ready_val, recv_val;

	if (dma->rx_wait.ready_cnt >= (dma->ready_cnt + dma->rx_sched_cnt))
		ready_val = 1;
	else
		ready_val = 0;

	if (dma->rx_wait.recv_cnt <= dma->recv_cnt)
		recv_val = 1;
	else
		recv_val = 0;

	/* AND or OR ? */
	if (dma->rx_wait.op == 0)
		return ready_val & recv_val; /* AND */
	else
		return ready_val | recv_val; /* OR */
}

/* Block until recv_cnt or more packets are Queued in RECV Q, op (AND or OR), 
 * ready_cnt or fewer packet buffers are available in the "READY and Scheduled" Q,
 * condition is met.
 * If a link error occurs and the Stop on Link error is defined, this function
 * will also return to caller, however with an error.
 */
int grspw_dma_rx_wait(void *c, int recv_cnt, int op, int ready_cnt, int timeout)
{
	struct grspw_dma_priv *dma = c;
	int ret, rc;

	if (timeout == 0)
		timeout = RTEMS_NO_TIMEOUT;

check_condition:

	/* Take DMA channel lock */
	if (rtems_semaphore_obtain(dma->sem_dma, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL)
		return -1;

	/* Check so that no other thread is waiting, this driver only supports
	 * one waiter at a time.
	 */
	if (dma->rx_wait.waiting) {
		ret = -1;
		goto out;
	}

	/* Stop if link error or similar (MDA stopped) */
	if (dma->started == 0) {
		ret = 1;
		goto out;
	}

	/* Set up Condition */
	dma->rx_wait.recv_cnt = recv_cnt;
	dma->rx_wait.op = op;
	dma->rx_wait.ready_cnt = ready_cnt;

	if (grspw_rx_wait_eval(dma) == 0) {
		/* Prepare Wait */
		dma->rx_wait.waiting = 1;

		/* Release channel lock */
		rtems_semaphore_release(dma->sem_dma);

		/* Try to take Wait lock, if this fail link may have gone down
		 * or user stopped this DMA channel
		 */
		rc = rtems_semaphore_obtain(dma->rx_wait.sem_wait, RTEMS_WAIT,
		                           timeout);
		if (rc == RTEMS_TIMEOUT) {
			dma->rx_wait.waiting = 0;
			return 2;
		} else if (rc == RTEMS_UNSATISFIED ||
		           rc == RTEMS_OBJECT_WAS_DELETED) {
			dma->rx_wait.waiting = 0;
			return 1; /* sem was flushed/deleted, means DMA stop */
		} else if (rc != RTEMS_SUCCESSFUL)
		    	return -1;

		/* Check condition once more */
		goto check_condition;
	} else {
		/* No Wait needed */
		dma->rx_wait.waiting = 0;
	}
	ret = 0;

out:
	/* Unlock DMA channel */
	rtems_semaphore_release(dma->sem_dma);

	return ret;
}

int grspw_dma_config(void *c, struct grspw_dma_config *cfg)
{
	struct grspw_dma_priv *dma = c;

	if (dma->started || !cfg)
		return -1;

	if (cfg->flags & ~DMAFLAG_MASK)
		return -1;

	/* Update Configuration */
	memcpy(&dma->cfg, cfg, sizeof(*cfg));

	return 0;
}

void grspw_dma_config_read(void *c, struct grspw_dma_config *cfg)
{
	struct grspw_dma_priv *dma = c;

	/* Copy Current Configuration */
	memcpy(cfg, &dma->cfg, sizeof(*cfg));
}

void grspw_dma_stats_read(void *c, struct grspw_dma_stats *sts)
{
	struct grspw_dma_priv *dma = c;

	memcpy(sts, &dma->stats, sizeof(dma->stats));
}

void grspw_dma_stats_clr(void *c)
{
	struct grspw_dma_priv *dma = c;

	/* Clear most of the statistics */	
	memset(&dma->stats, 0, sizeof(dma->stats));

	/* Init proper default values so that comparisons will work the
	 * first time.
	 */
	dma->stats.send_cnt_min = 0x3fffffff;
	dma->stats.tx_sched_cnt_min = 0x3fffffff;
	dma->stats.ready_cnt_min = 0x3fffffff;
	dma->stats.rx_sched_cnt_min = 0x3fffffff;
}

int grspw_dma_start(void *c)
{
	struct grspw_dma_priv *dma = c;
	struct grspw_dma_regs *dregs = dma->regs;
	unsigned int ctrl;

	if (dma->started)
		return 0;

	/* Initialize Software Structures:
	 *  - Clear all Queues
	 *  - init BD ring 
	 *  - init IRQ counter
	 *  - clear statistics counters
	 *  - init wait structures and semaphores
	 */
	grspw_dma_reset(dma);

	/* RX&RD and TX is not enabled until user fills SEND and READY Queue
	 * with SpaceWire Packet buffers. So we do not have to worry about
	 * IRQs for this channel just yet. However other DMA channels
	 * may be active.
	 *
	 * Some functionality that is not changed during started mode is set up
	 * once and for all here:
	 *
	 *   - RX MAX Packet length
	 *   - TX Descriptor base address to first BD in TX ring (not enabled)
	 *   - RX Descriptor base address to first BD in RX ring (not enabled)
	 *   - IRQs (TX DMA, RX DMA, DMA ERROR)
	 *   - Strip PID
	 *   - Strip Address
	 *   - No Spill
	 *   - Receiver Enable
	 *   - disable on link error (LE)
	 *
	 * Note that the address register and the address enable bit in DMACTRL
	 * register must be left untouched, they are configured on a GRSPW
	 * core level.
	 *
	 * Note that the receiver is enabled here, but since descriptors are
	 * not enabled the GRSPW core may stop/pause RX (if NS bit set) until
	 * descriptors are enabled or it may ignore RX packets (NS=0) until
	 * descriptors are enabled (writing RD bit).
	 */
	REG_WRITE(&dregs->txdesc, dma->tx_bds_hwa);
	REG_WRITE(&dregs->rxdesc, dma->rx_bds_hwa);

	/* MAX Packet length */
	REG_WRITE(&dma->regs->rxmax, dma->cfg.rxmaxlen);

	ctrl =  GRSPW_DMACTRL_AI | GRSPW_DMACTRL_PS | GRSPW_DMACTRL_PR |
		GRSPW_DMACTRL_TA | GRSPW_DMACTRL_RA | GRSPW_DMACTRL_RE |
		(dma->cfg.flags & DMAFLAG_MASK) << GRSPW_DMACTRL_NS_BIT;
	if (dma->core->dis_link_on_err)
		ctrl |= GRSPW_DMACTRL_LE;
	if (dma->cfg.rx_irq_en_cnt != 0)
		ctrl |= GRSPW_DMACTRL_RI;
	if (dma->cfg.tx_irq_en_cnt != 0)
		ctrl |= GRSPW_DMACTRL_TI;
	REG_WRITE(&dregs->ctrl, ctrl);

	dma->started = 1; /* open up other DMA interfaces */

	return 0;
}

STATIC void grspw_dma_stop_locked(struct grspw_dma_priv *dma)
{
	IRQFLAGS_TYPE irqflags;

	if (dma->started == 0)
		return;
	dma->started = 0;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	grspw_hw_dma_stop(dma);
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	/* From here no more packets will be sent, however
	 * there may still exist scheduled packets that has been
	 * sent, and packets in the SEND Queue waiting for free
	 * descriptors. All packets are moved to the SENT Queue
	 * so that the user may get its buffers back, the user
	 * must look at the TXPKT_FLAG_TX in order to determine
	 * if the packet was sent or not.
	 */

	/* Retreive scheduled all sent packets */
	grspw_tx_process_scheduled(dma);

	/* Move un-sent packets in SEND and SCHED queue to the
	 * SENT Queue. (never marked sent)
	 */
	if (!grspw_list_is_empty(&dma->tx_sched)) {
		grspw_list_append_list(&dma->sent, &dma->tx_sched);
		grspw_list_clr(&dma->tx_sched);
		dma->sent_cnt += dma->tx_sched_cnt;
		dma->tx_sched_cnt = 0;
	}
	if (!grspw_list_is_empty(&dma->send)) {
		grspw_list_append_list(&dma->sent, &dma->send);
		grspw_list_clr(&dma->send);
		dma->sent_cnt += dma->send_cnt;
		dma->send_cnt = 0;
	}

	/* Similar for RX */
	grspw_rx_process_scheduled(dma);
	if (!grspw_list_is_empty(&dma->rx_sched)) {
		grspw_list_append_list(&dma->recv, &dma->rx_sched);
		grspw_list_clr(&dma->rx_sched);
		dma->recv_cnt += dma->rx_sched_cnt;
		dma->rx_sched_cnt = 0;
	}
	if (!grspw_list_is_empty(&dma->ready)) {
		grspw_list_append_list(&dma->recv, &dma->ready);
		grspw_list_clr(&dma->ready);
		dma->recv_cnt += dma->ready_cnt;
		dma->ready_cnt = 0;
	}

	/* Throw out blocked threads */
	rtems_semaphore_flush(dma->rx_wait.sem_wait);
	rtems_semaphore_flush(dma->tx_wait.sem_wait);
}

void grspw_dma_stop(void *c)
{
	struct grspw_dma_priv *dma = c;

	/* Take DMA Channel lock */
	if (rtems_semaphore_obtain(dma->sem_dma, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL)
		return;

	grspw_dma_stop_locked(dma);

	rtems_semaphore_release(dma->sem_dma);
}

/* Do general work, invoked indirectly from ISR */
static void grspw_work_shutdown_func(struct grspw_priv *priv)
{
	int i;

	/* Link is down for some reason, and the user has configured
	 * that we stop all DMA channels and throw out all blocked
	 * threads.
	 */
	for (i=0; i<priv->hwsup.ndma_chans; i++)
		grspw_dma_stop(&priv->dma[i]);
	grspw_hw_stop(priv);
}

/* Do DMA work on one channel, invoked indirectly from ISR */
static void grspw_work_dma_func(struct grspw_dma_priv *dma)
{
	int tx_cond_true, rx_cond_true;
	unsigned int ctrl;
	IRQFLAGS_TYPE irqflags;

	rx_cond_true = 0;
	tx_cond_true = 0;
	dma->stats.irq_cnt++;

	/* Take DMA channel lock */
	if (rtems_semaphore_obtain(dma->sem_dma, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
	    != RTEMS_SUCCESSFUL)
		return;

	/* Look at cause we were woken up and clear source */
	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	ctrl = REG_READ(&dma->regs->ctrl);

	/* Read/Write DMA error ? */
	if (ctrl & GRSPW_DMA_STATUS_ERROR) {
		/* DMA error -> Stop DMA channel (both RX and TX) */
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
		grspw_dma_stop_locked(dma);
	} else if (ctrl & (GRSPW_DMACTRL_PR | GRSPW_DMACTRL_PS)) {
		/* DMA has finished a TX/RX packet */
		ctrl &= ~GRSPW_DMACTRL_AT;
		if (dma->cfg.rx_irq_en_cnt != 0)
			ctrl |= GRSPW_DMACTRL_RI;
		if (dma->cfg.tx_irq_en_cnt != 0)
			ctrl |= GRSPW_DMACTRL_TI;
		REG_WRITE(&dma->regs->ctrl, ctrl);
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
		if (ctrl & GRSPW_DMACTRL_PR) {
			/* Do RX Work */
			dma->stats.rx_work_cnt++;
			grspw_rx_process_scheduled(dma);
			dma->stats.rx_work_enabled += grspw_rx_schedule_ready(dma);
			/* Check to see if condition for waking blocked USER
		 	 * task is fullfilled.
			 */
			if (dma->rx_wait.waiting) {
				rx_cond_true = grspw_rx_wait_eval(dma);
				if (rx_cond_true)
					dma->rx_wait.waiting = 0;
			}
		}
		if (ctrl & GRSPW_DMACTRL_PS) {
			/* Do TX Work */
			dma->stats.tx_work_cnt++;
			grspw_tx_process_scheduled(dma);
			dma->stats.tx_work_enabled += grspw_tx_schedule_send(dma);
			if (dma->tx_wait.waiting) {
				tx_cond_true = grspw_tx_wait_eval(dma);
				if (tx_cond_true)
					dma->tx_wait.waiting = 0;
			}
		}
	} else
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	/* Release lock */
	rtems_semaphore_release(dma->sem_dma);

	if (rx_cond_true)
		rtems_semaphore_release(dma->rx_wait.sem_wait);

	if (tx_cond_true)
		rtems_semaphore_release(dma->tx_wait.sem_wait);
}

/* Work task is receiving work for the work message queue posted from
 * the ISR.
 */
static void grspw_work_func(rtems_task_argument unused)
{
	rtems_status_code status;
	unsigned int message;
	size_t size;
	struct grspw_priv *priv;
	int i;

	while (grspw_task_stop == 0) {
		/* Wait for ISR to schedule work */
		status = rtems_message_queue_receive(
			grspw_work_queue, &message,
			&size, RTEMS_WAIT, RTEMS_NO_TIMEOUT);
		if (status != RTEMS_SUCCESSFUL)
			break;

		/* Handle work */
		priv = priv_tab[message >> WORK_CORE_BIT];
		if (message & WORK_SHUTDOWN)
			grspw_work_shutdown_func(priv);
		else if (message & WORK_DMA_MASK) {
			for (i = 0; i < 4; i++) {
				if (message & WORK_DMA(i))
					grspw_work_dma_func(&priv->dma[i]);
			}
		}
	}
	rtems_task_delete(RTEMS_SELF);
}

STATIC void grspw_isr(void *data)
{
	struct grspw_priv *priv = data;
	unsigned int dma_stat, stat, stat_clrmsk, ctrl, icctrl, timecode;
	unsigned int rxirq, rxack, intto;
	int i, handled = 0, message = WORK_NONE, call_user_int_isr;
#ifdef RTEMS_HAS_SMP
	IRQFLAGS_TYPE irqflags;
#endif

	/* Get Status from Hardware */
	stat = REG_READ(&priv->regs->status);
	stat_clrmsk = stat & (GRSPW_STS_TO | GRSPW_STAT_ERROR);

	/* Make sure to put the timecode handling first in order to get the
	 * smallest possible interrupt latency
	 */
	if ((stat & GRSPW_STS_TO) && (priv->tcisr != NULL)) {
		ctrl = REG_READ(&priv->regs->ctrl);
		if (ctrl & GRSPW_CTRL_TQ) {
			/* Timecode received. Let custom function handle this */
			timecode = REG_READ(&priv->regs->time) &
					(GRSPW_TIME_CTRL | GRSPW_TIME_TCNT);
			(priv->tcisr)(priv->tcisr_arg, timecode);
		}
	}

	/* Get Interrupt status from hardware */
	icctrl = REG_READ(&priv->regs->icctrl);
	if ((icctrl & GRSPW_ICCTRL_IRQSRC_MASK) && (priv->icisr != NULL)) {
	    	call_user_int_isr = 0;
		rxirq = rxack = intto = 0;

		if ((icctrl & GRSPW_ICCTRL_IQ) &&
		    (rxirq = REG_READ(&priv->regs->icrx)) != 0)
			call_user_int_isr = 1;

		if ((icctrl & GRSPW_ICCTRL_AQ) &&
		    (rxack = REG_READ(&priv->regs->icack)) != 0)
			call_user_int_isr = 1;

		if ((icctrl & GRSPW_ICCTRL_TQ) &&
		    (intto = REG_READ(&priv->regs->ictimeout)) != 0)
			call_user_int_isr = 1;			

		/* Let custom functions handle this POTENTIAL SPW interrupt. The
		 * user function is called even if no such IRQ has happened!
		 * User must make sure to clear all interrupts that have been
		 * handled from the three registers by writing a one.
		 */
		if (call_user_int_isr)
			priv->icisr(priv->icisr_arg, rxirq, rxack, intto);
	}

	/* An Error occured? */
	if (stat & GRSPW_STAT_ERROR) {
		/* Wake Global WorkQ */
		handled = 1;

		if (stat & GRSPW_STS_EE)
			priv->stats.err_eeop++;

		if (stat & GRSPW_STS_IA)
			priv->stats.err_addr++;

		if (stat & GRSPW_STS_PE)
			priv->stats.err_parity++;

		if (stat & GRSPW_STS_ER)
			priv->stats.err_escape++;

		if (stat & GRSPW_STS_CE)
			priv->stats.err_credit++;

		if (stat & GRSPW_STS_WE)
			priv->stats.err_wsync++;

		if (priv->dis_link_on_err) {
			/* Disable the link, no more transfers are expected
			 * on any DMA channel.
			 */
			SPIN_LOCK(&priv->devlock, irqflags);
			ctrl = REG_READ(&priv->regs->ctrl);
			REG_WRITE(&priv->regs->ctrl, GRSPW_CTRL_LD |
				(ctrl & ~(GRSPW_CTRL_IE|GRSPW_CTRL_LS)));
			SPIN_UNLOCK(&priv->devlock, irqflags);
			/* Signal to work-thread to stop DMA and clean up */
			message = WORK_SHUTDOWN;
		}
	}

	/* Clear Status Flags */
	if (stat_clrmsk) {
		handled = 1;
		REG_WRITE(&priv->regs->status, stat_clrmsk);
	}

	/* A DMA transfer or Error occured? In that case disable more IRQs
	 * from the DMA channel, then invoke the workQ.
	 *
	 * Also the GI interrupt flag may not be available for older
	 * designs where (was added together with mutiple DMA channels).
	 */
	SPIN_LOCK(&priv->devlock, irqflags);
	for (i=0; i<priv->hwsup.ndma_chans; i++) {
		dma_stat = REG_READ(&priv->regs->dma[i].ctrl);
		/* Check for Errors and if Packets been sent or received if
		 * respective IRQ are enabled
		 */
#ifdef HW_WITH_GI
		if ( dma_stat & (GRSPW_DMA_STATUS_ERROR | GRSPW_DMACTRL_GI) ) {
#else
		if ( (((dma_stat << 3) & (GRSPW_DMACTRL_PR | GRSPW_DMACTRL_PS))
		     | GRSPW_DMA_STATUS_ERROR) & dma_stat ) {
#endif
			/* Disable Further IRQs (until enabled again)
			 * from this DMA channel. Let the status
			 * bit remain so that they can be handled by
			 * work function.
			 */
			REG_WRITE(&priv->regs->dma[i].ctrl, dma_stat & 
				~(GRSPW_DMACTRL_RI|GRSPW_DMACTRL_TI|
				GRSPW_DMACTRL_PR|GRSPW_DMACTRL_PS|
				GRSPW_DMACTRL_RA|GRSPW_DMACTRL_TA|
				GRSPW_DMACTRL_AT));
			message |= WORK_DMA(i);
			handled = 1;
		}
	}
	SPIN_UNLOCK(&priv->devlock, irqflags);

	if (handled != 0)
		priv->stats.irq_cnt++;

	/* Schedule work by sending message to work thread */
	if ((message != WORK_NONE) && grspw_work_queue) {
		message |= WORK_CORE(priv->index);
		stat = rtems_message_queue_send(grspw_work_queue, &message, 4);
		if (stat != RTEMS_SUCCESSFUL)
			printk("grspw_isr(%d): message fail %d (0x%x)\n",
				priv->index, stat, message);
	}
}

STATIC void grspw_hw_dma_stop(struct grspw_dma_priv *dma)
{
	unsigned int ctrl;
	struct grspw_dma_regs *dregs = dma->regs;

	ctrl = REG_READ(&dregs->ctrl) & (GRSPW_DMACTRL_LE | GRSPW_DMACTRL_EN |
	       GRSPW_DMACTRL_SP | GRSPW_DMACTRL_SA | GRSPW_DMACTRL_NS);
	ctrl |=	GRSPW_DMACTRL_AT;
	REG_WRITE(&dregs->ctrl, ctrl);
}

STATIC void grspw_hw_dma_softreset(struct grspw_dma_priv *dma)
{
	unsigned int ctrl;
	struct grspw_dma_regs *dregs = dma->regs;

	ctrl = REG_READ(&dregs->ctrl) & (GRSPW_DMACTRL_LE | GRSPW_DMACTRL_EN);
	REG_WRITE(&dregs->ctrl, ctrl);

	REG_WRITE(&dregs->rxmax, DEFAULT_RXMAX);
	REG_WRITE(&dregs->txdesc, 0);
	REG_WRITE(&dregs->rxdesc, 0);
}

/* Hardware Action:
 *  - stop DMA
 *  - do not bring down the link (RMAP may be active)
 *  - RMAP settings untouched (RMAP may be active)
 *  - port select untouched (RMAP may be active)
 *  - timecodes are disabled
 *  - IRQ generation disabled
 *  - status not cleared (let user analyze it if requested later on)
 *  - Node address / First DMA channels Node address
 *    is untouched (RMAP may be active)
 */
STATIC void grspw_hw_stop(struct grspw_priv *priv)
{
	int i;
	unsigned int ctrl;
	IRQFLAGS_TYPE irqflags;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	for (i=0; i<priv->hwsup.ndma_chans; i++)
		grspw_hw_dma_stop(&priv->dma[i]);

	ctrl = REG_READ(&priv->regs->ctrl);
	REG_WRITE(&priv->regs->ctrl, ctrl & (
		GRSPW_CTRL_LD | GRSPW_CTRL_LS | GRSPW_CTRL_AS |
		GRSPW_CTRL_RE | GRSPW_CTRL_RD |
		GRSPW_CTRL_NP | GRSPW_CTRL_PS));

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
}

/* Soft reset of GRSPW core registers */
STATIC void grspw_hw_softreset(struct grspw_priv *priv)
{
	int i;
	unsigned int tmp;

	for (i=0; i<priv->hwsup.ndma_chans; i++)
		grspw_hw_dma_softreset(&priv->dma[i]);

	REG_WRITE(&priv->regs->status, 0xffffffff);
	REG_WRITE(&priv->regs->time, 0);
	/* Clear all but valuable reset values of ICCTRL */
	tmp = REG_READ(&priv->regs->icctrl);
	tmp &= GRSPW_ICCTRL_INUM | GRSPW_ICCTRL_BIRQ | GRSPW_ICCTRL_TXIRQ;
	tmp |= GRSPW_ICCTRL_ID;
	REG_WRITE(&priv->regs->icctrl, tmp);
	REG_WRITE(&priv->regs->icrx, 0xffffffff);
	REG_WRITE(&priv->regs->icack, 0xffffffff);
	REG_WRITE(&priv->regs->ictimeout, 0xffffffff);
}

int grspw_dev_count(void)
{
	return grspw_count;
}

void grspw_initialize_user(void *(*devfound)(int), void (*devremove)(int,void*))
{
	int i;
	struct grspw_priv *priv;

	/* Set new Device Found Handler */
	grspw_dev_add = devfound;
	grspw_dev_del = devremove;

	if (grspw_initialized == 1 && grspw_dev_add) {
		/* Call callback for every previously found device */
		for (i=0; i<grspw_count; i++) {
			priv = priv_tab[i];
			if (priv)
				priv->data = grspw_dev_add(i);
		}
	}
}

/******************* Driver manager interface ***********************/

/* Driver prototypes */
static int grspw_common_init(void);
static int grspw2_init3(struct drvmgr_dev *dev);

static struct drvmgr_drv_ops grspw2_ops =
{
	.init = {NULL,  NULL, grspw2_init3, NULL},
	.remove = NULL,
	.info = NULL
};

static struct amba_dev_id grspw2_ids[] =
{
	{VENDOR_GAISLER, GAISLER_SPW}, /* not yet supported */
	{VENDOR_GAISLER, GAISLER_SPW2},
	{VENDOR_GAISLER, GAISLER_SPW2_DMA},
	{0, 0}		/* Mark end of table */
};

static struct amba_drv_info grspw2_drv_info =
{
	{
		DRVMGR_OBJ_DRV,			/* Driver */
		NULL,				/* Next driver */
		NULL,				/* Device list */
		DRIVER_AMBAPP_GAISLER_GRSPW2_ID,/* Driver ID */
		"GRSPW_PKT_DRV",		/* Driver Name */
		DRVMGR_BUS_TYPE_AMBAPP,		/* Bus Type */
		&grspw2_ops,
		NULL,				/* Funcs */
		0,				/* No devices yet */
		sizeof(struct grspw_priv),	/* Let DrvMgr alloc priv */
	},
	&grspw2_ids[0]
};

void grspw2_register_drv (void)
{
	GRSPW_DBG("Registering GRSPW2 packet driver\n");
	drvmgr_drv_register(&grspw2_drv_info.general);
}

static int grspw2_init3(struct drvmgr_dev *dev)
{
	struct grspw_priv *priv;
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	int i, size;
	unsigned int ctrl, icctrl, numi;
	union drvmgr_key_value *value;

	GRSPW_DBG("GRSPW[%d] on bus %s\n", dev->minor_drv,
		dev->parent->dev->name);

	if (grspw_count > GRSPW_MAX)
		return DRVMGR_ENORES;

	priv = dev->priv;
	if (priv == NULL)
		return DRVMGR_NOMEM;
	priv->dev = dev;

	/* If first device init common part of driver */
	if (grspw_common_init())
		return DRVMGR_FAIL;

	/*** Now we take care of device initialization ***/

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)dev->businfo;
	if (ambadev == NULL)
		return -1;
	pnpinfo = &ambadev->info;
	priv->irq = pnpinfo->irq;
	priv->regs = (struct grspw_regs *)pnpinfo->apb_slv->start;

	/* Read Hardware Support from Control Register */
	ctrl = REG_READ(&priv->regs->ctrl);
	priv->hwsup.rmap = (ctrl & GRSPW_CTRL_RA) >> GRSPW_CTRL_RA_BIT;
	priv->hwsup.rmap_crc = (ctrl & GRSPW_CTRL_RC) >> GRSPW_CTRL_RC_BIT;
	priv->hwsup.rx_unalign = (ctrl & GRSPW_CTRL_RX) >> GRSPW_CTRL_RX_BIT;
	priv->hwsup.nports = 1 + ((ctrl & GRSPW_CTRL_PO) >> GRSPW_CTRL_PO_BIT);
	priv->hwsup.ndma_chans = 1 + ((ctrl & GRSPW_CTRL_NCH) >> GRSPW_CTRL_NCH_BIT);
	priv->hwsup.irq = ((ctrl & GRSPW_CTRL_ID) >> GRSPW_CTRL_ID_BIT);
	icctrl = REG_READ(&priv->regs->icctrl);
	numi = (icctrl & GRSPW_ICCTRL_NUMI) >> GRSPW_ICCTRL_NUMI_BIT;
	if (numi > 0)
		priv->hwsup.irq_num = 1 << (numi - 1);
	else 
		priv->hwsup.irq_num = 0;

	/* Construct hardware version identification */
	priv->hwsup.hw_version = pnpinfo->device << 16 | pnpinfo->apb_slv->ver;

	if ((pnpinfo->device == GAISLER_SPW2) ||
	    (pnpinfo->device == GAISLER_SPW2_DMA)) {
		priv->hwsup.strip_adr = 1; /* All GRSPW2 can strip Address */
		priv->hwsup.strip_pid = 1; /* All GRSPW2 can strip PID */
	} else {
		/* Autodetect GRSPW1 features? */
		priv->hwsup.strip_adr = 0;
		priv->hwsup.strip_pid = 0;
	}

	/* Probe width of SpaceWire Interrupt ISR timers. All have the same
	 * width... so only the first is probed, if no timer result will be
	 * zero.
	 */
	REG_WRITE(&priv->regs->icrlpresc, 0x7fffffff);
	ctrl = REG_READ(&priv->regs->icrlpresc);
	REG_WRITE(&priv->regs->icrlpresc, 0);
	priv->hwsup.itmr_width = 0;
	while (ctrl & 1) {
		priv->hwsup.itmr_width++;
		ctrl = ctrl >> 1;
	}

	/* Let user limit the number of DMA channels on this core to save
	 * space. Only the first nDMA channels will be available.
	 */
	value = drvmgr_dev_key_get(priv->dev, "nDMA", DRVMGR_KT_INT);
	if (value && (value->i < priv->hwsup.ndma_chans))
		priv->hwsup.ndma_chans = value->i;

	/* Allocate and init Memory for all DMA channels */
	size = sizeof(struct grspw_dma_priv) * priv->hwsup.ndma_chans;
	priv->dma = (struct grspw_dma_priv *) malloc(size);
	if (priv->dma == NULL)
		return DRVMGR_NOMEM;
	memset(priv->dma, 0, size);
	for (i=0; i<priv->hwsup.ndma_chans; i++) {
		priv->dma[i].core = priv;
		priv->dma[i].index = i;
		priv->dma[i].regs = &priv->regs->dma[i];
	}

	/* Startup Action:
	 *  - stop DMA
	 *  - do not bring down the link (RMAP may be active)
	 *  - RMAP settings untouched (RMAP may be active)
	 *  - port select untouched (RMAP may be active)
	 *  - timecodes are diabled
	 *  - IRQ generation disabled
	 *  - status cleared
	 *  - Node address / First DMA channels Node address
	 *    is untouched (RMAP may be active)
	 */
	grspw_hw_stop(priv);
	grspw_hw_softreset(priv);

	/* Register character device in registered region */
	priv->index = grspw_count;
	priv_tab[priv->index] = priv;
	grspw_count++;

	/* Device name */
	sprintf(priv->devname, "grspw%d", priv->index);

	/* Tell above layer about new device */
	if (grspw_dev_add)
		priv->data = grspw_dev_add(priv->index);

	return DRVMGR_OK;
}

/******************* Driver Implementation ***********************/

static int grspw_common_init(void)
{
	if (grspw_initialized == 1)
		return 0;
	if (grspw_initialized == -1)
		return -1;
	grspw_initialized = -1;

	/* Device Semaphore created with count = 1 */
	if (rtems_semaphore_create(rtems_build_name('S', 'G', 'L', 'S'), 1,
	    RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | \
	    RTEMS_NO_INHERIT_PRIORITY | RTEMS_LOCAL | \
	    RTEMS_NO_PRIORITY_CEILING, 0, &grspw_sem) != RTEMS_SUCCESSFUL)
		return -1;

	/* Work queue, Work thread. Not created if user disables it.
	 * user can disable it when interrupt is not used to save resources
	 */
	if (grspw_work_task_priority != -1) {
		if (rtems_message_queue_create(
		    rtems_build_name('S', 'G', 'L', 'Q'), 32, 4, RTEMS_FIFO,
		    &grspw_work_queue) != RTEMS_SUCCESSFUL)
			return -1;

		if (rtems_task_create(rtems_build_name('S', 'G', 'L', 'T'),
		    grspw_work_task_priority, RTEMS_MINIMUM_STACK_SIZE,
		    RTEMS_PREEMPT | RTEMS_NO_ASR, RTEMS_NO_FLOATING_POINT,
		    &grspw_work_task) != RTEMS_SUCCESSFUL)
			return -1;

		if (rtems_task_start(grspw_work_task, grspw_work_func, 0) !=
		    RTEMS_SUCCESSFUL)
			return -1;
	}

	grspw_initialized = 1;
	return 0;
}
