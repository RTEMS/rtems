/*
 *  GRSPW/GRSPW2 SpaceWire Kernel Library Interface
 *
 *  COPYRIGHT (c) 2011
 *  Cobham Gaisler AB
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __GRSPW_PKT_H__
#define __GRSPW_PKT_H__

struct grspw_pkt;

/* Maximum number of GRSPW devices supported by driver */
#define GRSPW_MAX 32

/* Weak overridable variable the user can use to define the worker-task
 * priority (0..255) or to disable (-1) the creation of the worker-task
 * and the message queue to save space */
extern int grspw_work_task_priority;

#ifndef GRSPW_PKT_FLAGS
#define GRSPW_PKT_FLAGS
/*** TX Packet flags ***/

/* Enable IRQ generation */
#define TXPKT_FLAG_IE 0x0040

/* Enable Header CRC generation (if CRC is available in HW)
 * Header CRC will be appended (one byte at end of header)
 */
#define TXPKT_FLAG_HCRC 0x0100

/* Enable Data CRC generation (if CRC is available in HW)
 * Data CRC will be appended (one byte at end of packet)
 */
#define TXPKT_FLAG_DCRC 0x0200

/* Control how many bytes the beginning of the Header 
 * the CRC should not be calculated for */
#define TXPKT_FLAG_NOCRC_MASK 0x0000000f
#define TXPKT_FLAG_NOCRC_LEN0 0x00000000
#define TXPKT_FLAG_NOCRC_LEN1 0x00000001
#define TXPKT_FLAG_NOCRC_LEN2 0x00000002
#define TXPKT_FLAG_NOCRC_LEN3 0x00000003
#define TXPKT_FLAG_NOCRC_LEN4 0x00000004
#define TXPKT_FLAG_NOCRC_LEN5 0x00000005
#define TXPKT_FLAG_NOCRC_LEN6 0x00000006
#define TXPKT_FLAG_NOCRC_LEN7 0x00000007
#define TXPKT_FLAG_NOCRC_LEN8 0x00000008
#define TXPKT_FLAG_NOCRC_LEN9 0x00000009
#define TXPKT_FLAG_NOCRC_LENa 0x0000000a
#define TXPKT_FLAG_NOCRC_LENb 0x0000000b
#define TXPKT_FLAG_NOCRC_LENc 0x0000000c
#define TXPKT_FLAG_NOCRC_LENd 0x0000000d
#define TXPKT_FLAG_NOCRC_LENe 0x0000000e
#define TXPKT_FLAG_NOCRC_LENf 0x0000000f

#define TXPKT_FLAG_INPUT_MASK (TXPKT_FLAG_NOCRC_MASK | TXPKT_FLAG_IE | \
				TXPKT_FLAG_HCRC | TXPKT_FLAG_DCRC)

/* Marks if packet was transmitted or not */
#define TXPKT_FLAG_TX 0x4000

/* Link Error */
#define TXPKT_FLAG_LINKERR 0x8000

#define TXPKT_FLAG_OUTPUT_MASK (TXPKT_FLAG_TX | TXPKT_FLAG_LINKERR)

/*** RX Packet Flags ***/

/* Enable IRQ generation */
#define RXPKT_FLAG_IE 0x0010

#define RXPKT_FLAG_INPUT_MASK (RXPKT_FLAG_IE)

/* Packet was truncated */
#define RXPKT_FLAG_TRUNK 0x0800
/* Data CRC error (only valid if RMAP CRC is enabled) */
#define RXPKT_FLAG_DCRC 0x0400
/* Header CRC error (only valid if RMAP CRC is enabled) */
#define RXPKT_FLAG_HCRC 0x0200
/* Error in End-of-Packet */
#define RXPKT_FLAG_EEOP 0x0100
/* Marks if packet was recevied or not */
#define RXPKT_FLAG_RX 0x8000

#define RXPKT_FLAG_OUTPUT_MASK (RXPKT_FLAG_TRUNK | RXPKT_FLAG_DCRC | \
				RXPKT_FLAG_HCRC | RXPKT_FLAG_EEOP)

/*** General packet flag options ***/

/* Translate Hdr and/or Payload address */
#define PKT_FLAG_TR_DATA 0x1000
#define PKT_FLAG_TR_HDR 0x2000
/* All General options */
#define PKT_FLAG_MASK 0x3000

#endif
/* GRSPW RX/TX Packet structure.
 *
 * - For RX the 'hdr' and 'hlen' fields are not used, they are not written
 *   by driver.
 *
 * - The 'pkt_id' field is untouched by driver, it is intended for packet
 *   numbering or user-custom data.
 *
 * - The last packet in a list must have 'next' set to NULL.
 *
 * - data and hdr pointers are written without modification to hardware,
 *   this means that caller must do address translation to hardware
 *   address itself.
 *
 * - the 'flags' field are interpreted differently depending on transfer
 *   type (RX/TX). See XXPKT_FLAG_* options above.
 */
struct grspw_pkt {
	struct grspw_pkt *next;	/* Next packet in list. NULL if last packet */
	unsigned int pkt_id;	/* User assigned ID (not touched by driver) */
	unsigned short flags;	/* RX/TX Options and status */
	unsigned char reserved;	/* Reserved, must be zero */
	unsigned char hlen;	/* Length of Header Buffer (only TX) */
	unsigned int dlen;	/* Length of Data Buffer */
	void *data;	/* 4-byte or byte aligned depends on HW */
	void *hdr;	/* 4-byte or byte aligned depends on HW (only TX) */
};

/* GRSPW SpaceWire Packet List */
struct grspw_list {
	struct grspw_pkt *head;
	struct grspw_pkt *tail;
};

/* SpaceWire Link State */
typedef enum {
	SPW_LS_ERRRST = 0,
	SPW_LS_ERRWAIT = 1,
	SPW_LS_READY = 2,
	SPW_LS_CONNECTING = 3,
	SPW_LS_STARTED = 4,
	SPW_LS_RUN = 5
} spw_link_state_t;

/* Address Configuration */
struct grspw_addr_config {
	/* Ignore address field and put all received packets to first
	 * DMA channel.
	 */
	int promiscuous;

	/* Default Node Address and Mask */
	unsigned char def_addr;
	unsigned char def_mask;
	/* DMA Channel custom Node Address and Mask */
	struct {
		char node_en;			/* Enable Separate Addr */
		unsigned char node_addr;	/* Node address */
		unsigned char node_mask;	/* Node address mask */
	} dma_nacfg[4];
};

/* Hardware Support in GRSPW Core */
struct grspw_hw_sup {
	char	rmap;		/* If RMAP in HW is available */
	char	rmap_crc;	/* If RMAP CRC is available */
	char	rx_unalign;	/* RX unaligned (byte boundary) access allowed*/
	char	nports;		/* Number of Ports (1 or 2) */
	char	ndma_chans;	/* Number of DMA Channels (1..4) */
	char	strip_adr;	/* Hardware can strip ADR from packet data */
	char	strip_pid;	/* Hardware can strip PID from packet data */
	int	hw_version;	/* GRSPW Hardware Version */
	char	reserved[2];
	char	irq;		/* SpW Distributed Interrupt available if 1 */
	char	irq_num;	/* Number of interrupts that can be generated */
	char	itmr_width;	/* SpW Intr. ISR timers bit width. 0=no timer */
};

struct grspw_core_stats {
	int irq_cnt;
	int err_credit;
	int err_eeop;
	int err_addr;
	int err_parity;
	int err_disconnect;
	int err_escape;
	int err_wsync; /* only in GRSPW1 */
};

/* grspw_link_ctrl() options */
#define LINKOPTS_ENABLE		0x0000
#define LINKOPTS_DISABLE	0x0001
#define LINKOPTS_START		0x0002
#define LINKOPTS_AUTOSTART	0x0004
#define LINKOPTS_DIS_ONERR	0x0008	/* Disable DMA transmitter on link error
					 * Controls LE bit in DMACTRL register.
					 */
#define LINKOPTS_DIS_ON_CE	0x0020000/* Disable Link on Credit error */
#define LINKOPTS_DIS_ON_ER	0x0040000/* Disable Link on Escape error */
#define LINKOPTS_DIS_ON_DE	0x0080000/* Disable Link on Disconnect error */
#define LINKOPTS_DIS_ON_PE	0x0100000/* Disable Link on Parity error */
#define LINKOPTS_DIS_ON_WE	0x0400000/* Disable Link on write synchonization
					  * error (GRSPW1 only)
					  */
#define LINKOPTS_DIS_ON_EE	0x1000000/* Disable Link on Early EOP/EEP error*/

/*#define LINKOPTS_TICK_OUT_IRQ	0x0100*//* Enable Tick-out IRQ */
#define LINKOPTS_EIRQ		0x0200	/* Enable Error Link IRQ */

#define LINKOPTS_MASK		0x15e020f/* All above options */
#define LINKOPTS_MASK_DIS_ON	0x15e0000/* All disable link on error options
					  * On a certain error the link disable
					  * bit will be written and the work
					  * task will call dma_stop() for all
					  * channels.
					  */

#define LINKSTS_CE		0x002	/* Credit error */
#define LINKSTS_ER		0x004	/* Escape error */
#define LINKSTS_DE		0x008	/* Disconnect error */
#define LINKSTS_PE		0x010	/* Parity error */
#define LINKSTS_WE		0x040	/* Write synchonization error (GRSPW1 only) */
#define LINKSTS_IA		0x080	/* Invalid address */
#define LINKSTS_EE		0x100	/* Early EOP/EEP */
#define LINKSTS_MASK		0x1de

/* grspw_tc_ctrl() options */
#define TCOPTS_EN_RXIRQ	0x0001	/* Tick-Out IRQ */
#define TCOPTS_EN_TX	0x0004
#define TCOPTS_EN_RX	0x0008

/* grspw_ic_ctrl() options:
 * Corresponds code duplicatingly to GRSPW_CTRL_XX_BIT defines
 */
#define ICOPTS_INTNUM		(0x1f << 27)
#define ICOPTS_EN_SPWIRQ_ON_EE	(1 << 24)
#define ICOPTS_EN_SPWIRQ_ON_IA	(1 << 23)
#define ICOPTS_EN_PRIO		(1 << 22)
#define ICOPTS_EN_TIMEOUTIRQ	(1 << 20)
#define ICOPTS_EN_ACKIRQ	(1 << 19)
#define ICOPTS_EN_TICKOUTIRQ	(1 << 18)
#define ICOPTS_EN_RX		(1 << 17)
#define ICOPTS_EN_TX		(1 << 16)
#define ICOPTS_BASEIRQ		(0x1f << 8)
#define ICOPTS_EN_FLAGFILTER	(1 << 0) /* NOTE: Not in icctrl. CTRL.bit12 */

/* grspw_ic_rlisr() and grspw_ic_rlintack()  */
#define ICRELOAD_EN		(1 << 31)
#define ICRELOAD_MASK		0x7fffffff

/* grspw_rmap_ctrl() options */
#define RMAPOPTS_EN_RMAP	0x0001
#define RMAPOPTS_EN_BUF		0x0002

/* grspw_dma_config.flags options */
#define DMAFLAG_NO_SPILL	0x0001	/* See HW doc DMA-CTRL NS bit */
#define DMAFLAG_RESV1		0x0002	/* HAS NO EFFECT */
#define DMAFLAG_STRIP_ADR	0x0004	/* See HW doc DMA-CTRL SA bit */
#define DMAFLAG_STRIP_PID	0x0008	/* See HW doc DMA-CTRL SP bit */
#define DMAFLAG_RESV2		0x0010	/* HAS NO EFFECT */
#define DMAFLAG_MASK	(DMAFLAG_NO_SPILL|DMAFLAG_STRIP_ADR|DMAFLAG_STRIP_PID)
/* grspw_dma_config.flags misc options (not shifted internally) */
#define DMAFLAG2_TXIE	0x00100000	/* See HW doc DMA-CTRL TI bit. 
					 * Used to enable TX DMA interrupt
					 * when tx_irq_en_cnt=0.
					 */
#define DMAFLAG2_RXIE	0x00200000	/* See HW doc DMA-CTRL RI bit.
					 * Used to enable RX DMA interrupt
					 * when rx_irq_en_cnt=0.
					 */
/* Defines how the ISR will disable RX/TX DMA interrupt source when a DMA RX/TX
 * interrupt has happended. DMA Error Interrupt always disables both RX/TX DMA
 * interrupt. By default both RX/TX IRQs are disabled when either a RX, TX or
 * both RX/TX DMA interrupt has been requested. The work-task, custom
 * application handler or custom ISR handler is responsible to re-enable
 * DMA interrupts.
 */
#define DMAFLAG2_IRQD_SRC  0x01000000	/* Disable triggering RX/TX source */
#define DMAFLAG2_IRQD_NONE 0x00c00000	/* Never disable RX/TX IRQ in ISR */
#define DMAFLAG2_IRQD_BOTH 0x00000000	/* Always disable both RX/TX sources */
#define DMAFLAG2_IRQD_MASK 0x01c00000	/* Mask of options */
#define DMAFLAG2_IRQD_BIT  22

#define DMAFLAG2_MASK	(DMAFLAG2_TXIE | DMAFLAG2_RXIE | DMAFLAG2_IRQD_MASK)

struct grspw_dma_config {
	int flags;		/* DMA config flags, see DMAFLAG1&2_* options */
	int rxmaxlen;		/* RX Max Packet Length */
	int rx_irq_en_cnt;	/* Enable RX IRQ every cnt descriptors */
	int tx_irq_en_cnt;	/* Enable TX IRQ every cnt descriptors */
};

/* Statistics per DMA channel */
struct grspw_dma_stats {
	/* IRQ Statistics */
	int irq_cnt;		/* Number of DMA IRQs generated by channel */

	/* Descriptor Statistics */
	int tx_pkts;		/* Number of Transmitted packets */
	int tx_err_link;	/* Number of Transmitted packets with Link Error*/
	int rx_pkts;		/* Number of Received packets */
	int rx_err_trunk;	/* Number of Received Truncated packets */
	int rx_err_endpkt;	/* Number of Received packets with bad ending */

	/* Diagnostics to help developers sizing their number buffers to avoid
	 * out-of-buffers or other phenomenons.
	 */
	int send_cnt_min;	/* Minimum number of packets in TX SEND Q */
	int send_cnt_max;	/* Maximum number of packets in TX SEND Q */
	int tx_sched_cnt_min;	/* Minimum number of packets in TX SCHED Q */
	int tx_sched_cnt_max;	/* Maximum number of packets in TX SCHED Q */
	int sent_cnt_max;	/* Maximum number of packets in TX SENT Q */
	int tx_work_cnt;	/* Times the work thread processed TX BDs */
	int tx_work_enabled;	/* No. RX BDs enabled by work thread */

	int ready_cnt_min;	/* Minimum number of packets in RX READY Q */
	int ready_cnt_max;	/* Maximum number of packets in RX READY Q */
	int rx_sched_cnt_min;	/* Minimum number of packets in RX SCHED Q */
	int rx_sched_cnt_max;	/* Maximum number of packets in RX SCHED Q */
	int recv_cnt_max;	/* Maximum number of packets in RX RECV Q */
	int rx_work_cnt;	/* Times the work thread processed RX BDs */
	int rx_work_enabled;	/* No. RX BDs enabled by work thread */
};

/* ISR message sending call back. Compatible with rtems_message_queue_send().
 * The 'buf' parameter has a pointer to a WORK-TASK message defined by the
 * WORK_* macros below. The message indicates what GRSPW device operations
 * are pending, thus what caused the interrupt.
 *
 * \param data   defined by grspw_work_config.msgisr_arg, default a rtems_id.
 * \param buf    Pointer to a 32-bit message word
 * \param n      Always 4 (byte size of buf).
 */
typedef int (*grspw_msgqisr_t)(void *data, unsigned int *buf, unsigned int n);

/* Work message definitions, the int sent to *buf
 * Bits 31..24: reserved.
 * Bits 23..16: GRSPW device number message is associated with.
 * Bit  15:     reserved.
 * Bit  14:     work-task shall delete message queue on exit.
 * Bit  13:     work-task shall exit and delete itself.
 * Bit  12:     link error - shut down all DMA operations (stop DMA channels).
 * Bit  11..8:  Indicats DMA error on DMA channel 3..0.
 * Bit  7..0:   Indicats RX and/or TX packets completed on channel 3..0.
 */
#define WORK_NONE         0
#define WORK_SHUTDOWN     0x1000 /* Signal shut down */
#define WORK_QUIT_TASK    0x2000 /* Work task shall exit (delete itself) */
#define WORK_FREE_MSGQ    0x4000 /* Delete MsgQ (valid when WORK_QUIT_TASK) */
#define WORK_DMA(chan, rxtx) (((rxtx) & 0x3) << ((chan) * 2))
#define WORK_DMA_TX(chan) WORK_DMA(chan, 1)
#define WORK_DMA_RX(chan) WORK_DMA(chan, 2)
#define WORK_DMA_ER(chan) (0x1 << ((chan) + 8))
#define WORK_DMA_MASK     0xfff /* max 4 channels all work */
#define WORK_DMA_TX_MASK  0x055 /* max 4 channels TX work */
#define WORK_DMA_RX_MASK  0x0aa /* max 4 channels RX work */
#define WORK_DMA_ER_MASK  0xf00 /* max 4 channels Error work */
#define WORK_DMA_CHAN_MASK(chan) (WORK_DMA_ER(chan) | WORK_DMA(chan, 0x3))
#define WORK_CORE_BIT     16
#define WORK_CORE_MASK    0x00ff0000
#define WORK_CORE(device) ((device) << WORK_CORE_BIT)

/* Message Q used to send messages to work task */
struct grspw_work_config {
	grspw_msgqisr_t msgisr;
	void *msgisr_arg; /* example: rtems_id to Msg Q */
};

extern void grspw_initialize_user(
	/* Callback every time a GRSPW device is found. Args: DeviceIndex */
	void *(*devfound)(int),
	/* Callback every time a GRSPW device is removed. Args:
	 * int   = DeviceIndex
	 * void* = Return Value from devfound()
	 */
	void (*devremove)(int,void*)
	);

/* Creates a MsgQ (optional) and spawns a worker task associated with the
 * message Q. The task can also be associated with a custom msgQ if *msgQ.
 * is non-zero.
 *
 * \param prio     Task priority, set to -1 for default.
 * \param stack    Task stack size, set to 0 for default.
 * \param msgQ     pMsgQ=NULL: illegal,
 *                 pMsqQ==0: create new MsgQ with task and place in *pMsgQ,
 *                 *pmsqQ!=0: pointer to MsgQ used for task.
 * \param msgMax   Maximum number of messages, set to 0 for default.
 * \return         0 on failure, task id on success.
 */
extern rtems_id grspw_work_spawn(int prio, int stack, rtems_id *pMsgQ, int msgMax);

/* Free task associated with message queue and optionally also the message
 * queue itself. The message queue is deleted by the work task and is therefore
 * delayed until it the work task resumes its execution.
 */
extern rtems_status_code grspw_work_free(rtems_id msgQ, int freeMsgQ);

/* Configure a GRSPW device Work task and Message Q set up.
 * This affects messages to:
 *  - DMA AHB error interrupt handling (mandatory)
 *  - Link status interrupt handling (optional)
 *  - RX DMA, defaults to common msgQ (configured per DMA channel) 
 */
extern void grspw_work_cfg(void *d, struct grspw_work_config *wc);

/* Work-task function, called only from the work task. The function is provided
 * as a way for the user to create its own work tasks.
 * The argument determines which message queue the task shall read its
 * work jobs from.
 *
 * The messages are always 32-bit words and follows the format defined by the
 * WORK_* macros above.
 */
extern void grspw_work_func(rtems_id msgQ);

enum grspw_worktask_ev {
	WORKTASK_EV_NONE = 0,
	WORKTASK_EV_QUIT = 1,
	WORKTASK_EV_SHUTDOWN = 2,
	WORKTASK_EV_DMA_STOP = 3,
};

/* Weak function to let user override. Function called every time one of the
 * events above is handled by the work-task. The message 'msg' is the current
 * message being processed by the work-task.
 * The user can for example add custom code to invoke on a DMA error, link
 * error or monitor when the work-task exits after a call to grspw_work_free().
 */
extern void grspw_work_event(enum grspw_worktask_ev ev, unsigned int msg);

#ifdef RTEMS_SMP
/* Set ISR interrupt affinity. The LEON IRQCtrl requires that the cpumask shall
 * always have one bit set.
 */
extern int grspw_isr_affinity(void *d, const cpu_set_t *cpus);
#endif

extern int grspw_dev_count(void);
extern void *grspw_open(int dev_no);
extern int grspw_close(void *d);
extern void grspw_hw_support(void *d, struct grspw_hw_sup *hw);
extern void grspw_stats_read(void *d, struct grspw_core_stats *sts);
extern void grspw_stats_clr(void *d);

/* Set and Read current node address configuration. The dma_nacfg[N] field
 * represents the configuration for DMA Channel N.
 *
 * Set cfg->promiscous to -1 in order to only read current configuration.
 */
extern void grspw_addr_ctrl(void *d, struct grspw_addr_config *cfg);

/*** Link Control interface ***/
/* Read Link State */
extern spw_link_state_t grspw_link_state(void *d);
/* options [in/out]: set to -1 to only read current config
 *
 * CLKDIV register contain:
 *  bits 7..0  : Clock Div RUN (only run-state)
 *  bits 15..8 : Clock Div During Startup (all link states except run-state)
 */
extern void grspw_link_ctrl(void *d, int *options, int *stscfg, int *clkdiv);
/* Read the current value of the status register */
extern unsigned int grspw_link_status(void *d);
/* Clear bits in the status register */
extern void grspw_link_status_clr(void *d, unsigned int clearmask);

/*** Time Code Interface ***/
/* Generate Tick-In (increment Time Counter, Send Time Code) */
extern void grspw_tc_tx(void *d);
/* Control Timcode settings of core */
extern void grspw_tc_ctrl(void *d, int *options);
/* Assign ISR Function to TimeCode RX IRQ */
extern void grspw_tc_isr(void *d, void (*tcisr)(void *data, int tc), void *data);
/* Read/Write TCTRL and TIMECNT. Write if not -1, always read current value
 * TCTRL   = bits 7 and 6
 * TIMECNT = bits 5 to 0
 */
extern void grspw_tc_time(void *d, int *time);

/*** Interrupt-code Interface ***/
struct spwpkt_ic_config {
	unsigned int tomask;
	unsigned int aamask;
	unsigned int scaler;
	unsigned int isr_reload;
	unsigned int ack_reload;
};
/* Function Interrupt-Code ISR callback prototype. Called when respective
 * interrupt handling option has been enabled by grspw_ic_ctrl(), the
 * arguments rxirq, rxack and intto are read from the registers of the
 * GRSPW core read by the GRSPW ISR, they are individually valid only when
 * repective handling been turned on.
 *
 * data    - Custom data provided by user
 * rxirq   - Interrupt-Code Recevie register of the GRSPW core read by ISR
 *           (only defined if IQ bit enabled through grspw_ic_ctrl())
 * rxack   - Interrupt-Ack-Code Recevie register of the GRSPW core read by ISR
 *           (only defined if AQ bit enabled through grspw_ic_ctrl())
 * intto   - Interrupt Tick-out Recevie register of the GRSPW core read by ISR
 *           (only defined if TQ bit enabled through grspw_ic_ctrl()) 
 */
typedef void (*spwpkt_ic_isr_t)(void *data, unsigned int rxirq,
				unsigned int rxack, unsigned int intto);
/* Control Interrupt-code settings of core
 * Write if 'options' not pointing to -1, always read current value
 */
extern void grspw_ic_ctrl(void *d, unsigned int *options);
/* Write (rw&1 == 1) configuration parameters to registers and/or,
 * Read  (rw&2 == 1) configuration parameters from registers, in that sequence.
 */
extern void grspw_ic_config(void *d, int rw, struct spwpkt_ic_config *cfg);
/* Read or Write Interrupt-code status registers.
 * If pointer argument *ptr == 0 then only read, if *ptr != 0 then only write.
 * If *ptr is NULL no operation.
 */
extern void grspw_ic_sts(void *d, unsigned int *rxirq, unsigned int *rxack,
			unsigned int *intto);
/* Generate Tick-In for the given Interrupt-code
 * Returns zero on success and non-zero on failure
 *
 * Interrupt code bits (ic):
 * Bit 5 - ACK if 1
 * Bits 4-0 Interrupt-code number
 */
extern int grspw_ic_tickin(void *d, int ic);
/* Assign handler function to Interrupt-code timeout IRQ */
extern void grspw_ic_isr(void *d, spwpkt_ic_isr_t handler, void *data);

/*** RMAP Control Interface ***/
/* Set (not -1) and/or read RMAP options. */
extern int grspw_rmap_ctrl(void *d, int *options, int *dstkey);
extern void grspw_rmap_support(void *d, char *rmap, char *rmap_crc);

/*** SpW Port Control Interface ***/

/* Select port, if
 * -1=The current selected port is returned
 * 0=Port 0
 * 1=Port 1
 * Other positive values=Both Port0 and Port1
 */
extern int grspw_port_ctrl(void *d, int *port);
/* Returns Number ports available in hardware */
extern int grspw_port_count(void *d);
/* Returns the current active port */
extern int grspw_port_active(void *d);

/*** DMA Interface ***/
extern void *grspw_dma_open(void *d, int chan_no);
extern int grspw_dma_close(void *c);

extern int grspw_dma_start(void *c);
extern void grspw_dma_stop(void *c);

/* Enable interrupt manually */
extern unsigned int grspw_dma_enable_int(void *c, int rxtx, int force);

/* Return Current DMA Control & Status Register */
extern unsigned int grspw_dma_ctrlsts(void *c);

/* Schedule List of packets for transmission at some point in
 * future.
 *
 * 1. Move transmitted packets to SENT List (SCHED->SENT)
 * 2. Add the requested packets to the SEND List (USER->SEND)
 * 3. Schedule as many packets as possible for transmission (SEND->SCHED)
 *
 * Call this function with pkts=NULL to just do step 1 and 3. This may be
 * required in Polling-mode.
 *
 * The above steps 1 and 3 may be skipped by setting 'opts':
 *  bit0 = 1: Skip Step 1.
 *  bit1 = 1: Skip Step 3.
 * Skipping both step 1 and 3 may be usefull when IRQ is enabled, then
 * the work queue will be totaly responsible for handling descriptors.
 *
 * The fastest solution in retreiving sent TX packets and sending new frames
 * is to call:
 *  A. grspw_dma_tx_reclaim(opts=0)
 *  B. grspw_dma_tx_send(opts=1)
 *
 * NOTE: the TXPKT_FLAG_TX flag must not be set.
 *
 * Return Code
 *  -1   Error
 *  0    Successfully added pkts to send/sched list
 *  1    DMA stopped. No operation.
 */
extern int grspw_dma_tx_send(void *c, int opts, struct grspw_list *pkts, int count);

/* Reclaim TX packet buffers that has previously been scheduled for transmission
 * with grspw_dma_tx_send().
 *
 * 1. Move transmitted packets to SENT List (SCHED->SENT)
 * 2. Move all SENT List to pkts list (SENT->USER)
 * 3. Schedule as many packets as possible for transmission (SEND->SCHED)
 *
 * The above steps 1 may be skipped by setting 'opts':
 *  bit0 = 1: Skip Step 1.
 *  bit1 = 1: Skip Step 3.
 *
 * The fastest solution in retreiving sent TX packets and sending new frames
 * is to call:
 *  A. grspw_dma_tx_reclaim(opts=2) (Skip step 3)
 *  B. grspw_dma_tx_send(opts=1) (Skip step 1)
 *
 * Return Code
 *  -1   Error
 *  0    Successful. pkts list filled with all packets from sent list
 *  1    Same as 0, but indicates that DMA stopped
 */
extern int grspw_dma_tx_reclaim(void *c, int opts, struct grspw_list *pkts, int *count);

/* Get current number of Packets in respective TX Queue. */
extern void grspw_dma_tx_count(void *c, int *send, int *sched, int *sent, int *hw);

#define GRSPW_OP_AND 0
#define GRSPW_OP_OR 1
/* Block until send_cnt or fewer packets are Queued in "Send and Scheduled" Q,
 * op (AND or OR), sent_cnt or more packet "have been sent" (Sent Q) condition
 * is met.
 * If a link error occurs and the Stop on Link error is defined, this function
 * will also return to caller.
 * The timeout argument is used to return after timeout ticks, regardless of
 * the other conditions. If timeout is zero, the function will wait forever
 * until the condition is satisfied.
 *
 * NOTE: if IRQ of TX descriptors are not enabled conditions are never
 *       checked, this may hang infinitely unless a timeout has been specified
 *
 * Return Code
 *  -1   Error
 *  0    Returing to caller because specified conditions are now fullfilled
 *  1    DMA stopped
 *  2    Timeout, conditions are not met
 *  3    Another task is already waiting. Service is Busy.
 */
extern int grspw_dma_tx_wait(void *c, int send_cnt, int op, int sent_cnt, int timeout);

/* Get received RX packet buffers that has previously been scheduled for 
 * reception with grspw_dma_rx_prepare().
 *
 * 1. Move Scheduled packets to RECV List (SCHED->RECV)
 * 2. Move all RECV packet to the callers list (RECV->USER)
 * 3. Schedule as many free packet buffers as possible (READY->SCHED)
 *
 * The above steps 1 may be skipped by setting 'opts':
 *  bit0 = 1: Skip Step 1.
 *  bit1 = 1: Skip Step 3.
 *
 * The fastest solution in retreiving received RX packets and preparing new
 * packet buffers for future receive, is to call:
 *  A. grspw_dma_rx_recv(opts=2, &recvlist) (Skip step 3)
 *  B. grspw_dma_rx_prepare(opts=1, &freelist) (Skip step 1)
 *
 * Return Code
 *  -1   Error
 *  0    Successfully filled pkts list with packets from recv list.
 *  1    DMA stopped
 */
extern int grspw_dma_rx_recv(void *c, int opts, struct grspw_list *pkts, int *count);

/* Add more RX packet buffers for future for reception. The received packets
 * can later be read out with grspw_dma_rx_recv().
 *
 * 1. Move Received packets to RECV List (SCHED->RECV)
 * 2. Add the "free/ready" packet buffers to the READY List (USER->READY)
 * 3. Schedule as many packets as possible (READY->SCHED)
 *
 * The above steps 1 may be skipped by setting 'opts':
 *  bit0 = 1: Skip Step 1.
 *  bit1 = 1: Skip Step 3.
 *
 * The fastest solution in retreiving received RX packets and preparing new
 * packet buffers for future receive, is to call:
 *  A. grspw_dma_rx_recv(opts=2, &recvlist) (Skip step 3)
 *  B. grspw_dma_rx_prepare(opts=1, &freelist) (Skip step 1)
 *
 * Return Code
 *  -1   Error
 *  0    Successfully added packet buffers from pkt list into the ready queue
 *  1    DMA stopped
 */
extern int grspw_dma_rx_prepare(void *c, int opts, struct grspw_list *pkts, int count);

/* Get current number of Packets in respective RX Queue. */
extern void grspw_dma_rx_count(void *c, int *ready, int *sched, int *recv, int *hw);

/* Block until recv_cnt or more packets are Queued in RECV Q, op (AND or OR), 
 * ready_cnt or fewer packet buffers are available in the "READY and Scheduled" Q,
 * condition is met.
 * If a link error occurs and the Stop on Link error is defined, this function
 * will also return to caller, however with an error.
 * The timeout argument is used to return after timeout ticks, regardless of
 * the other conditions. If timeout is zero, the function will wait forever
 * until the condition is satisfied.
 *
 * NOTE: if IRQ of RX descriptors are not enabled conditions are never
 *       checked, this may hang infinitely unless a timeout has been specified
 *
 * Return Code
 *  -1   Error
 *  0    Returing to caller because specified conditions are now fullfilled
 *  1    DMA stopped
 *  2    Timeout, conditions are not met
 *  3    Another task is already waiting. Service is Busy.
 */
extern int grspw_dma_rx_wait(void *c, int recv_cnt, int op, int ready_cnt, int timeout);

extern int grspw_dma_config(void *c, struct grspw_dma_config *cfg);
extern void grspw_dma_config_read(void *c, struct grspw_dma_config *cfg);

extern void grspw_dma_stats_read(void *c, struct grspw_dma_stats *sts);
extern void grspw_dma_stats_clr(void *c);

/* Register GRSPW packet driver to Driver Manager */
void grspw2_register_drv (void);

/*** GRSPW SpaceWire Packet List Handling Routines ***/

static inline void grspw_list_clr(struct grspw_list *list)
{
        list->head = NULL;
        list->tail = NULL;
}

static inline int grspw_list_is_empty(struct grspw_list *list)
{
        return (list->head == NULL);
}

/* Return Number of entries in list */
static inline int grspw_list_cnt(struct grspw_list *list)
{
	struct grspw_pkt *lastpkt = NULL, *pkt = list->head;
	int cnt = 0;
	while ( pkt ) {
		cnt++;
		lastpkt = pkt;
		pkt = pkt->next;
	}
	if ( lastpkt && (list->tail != lastpkt) )
		return -1;
	return cnt;
}

static inline void
grspw_list_append(struct grspw_list *list, struct grspw_pkt *pkt)
{
	pkt->next = NULL;
	if ( list->tail == NULL ) {
		list->head = pkt;
	} else {
		list->tail->next = pkt;
	}
	list->tail = pkt;
}

static inline void 
grspw_list_prepend(struct grspw_list *list, struct grspw_pkt *pkt)
{
	pkt->next = list->head;
	if ( list->head == NULL ) {
		list->tail = pkt;
	}
	list->head = pkt;
}

static inline void
grspw_list_append_list(struct grspw_list *list, struct grspw_list *alist)
{
	if (grspw_list_is_empty(alist)) {
		return;
	}
	alist->tail->next = NULL;
	if ( list->tail == NULL ) {
		list->head = alist->head;
	} else {
		list->tail->next = alist->head;
	}
	list->tail = alist->tail;
}

static inline void
grspw_list_prepend_list(struct grspw_list *list, struct grspw_list *alist)
{
	if (grspw_list_is_empty(alist)) {
		return;
	}
	if ( list->head == NULL ) {
		list->tail = alist->tail;
		alist->tail->next = NULL;
	} else {
		alist->tail->next = list->head;
	}
	list->head = alist->head;
}

/* Remove dlist (delete-list) from head of list */
static inline void
grspw_list_remove_head_list(struct grspw_list *list, struct grspw_list *dlist)
{
	if (grspw_list_is_empty(dlist)) {
		return;
	}
	list->head = dlist->tail->next;
	if ( list->head == NULL ) {
		list->tail = NULL;
	}
	dlist->tail->next = NULL;
}

/* Take A number of entries from head of list 'list' and put the entires
 * to rlist (result list).
 */
static inline int
grspw_list_take_head_list(struct grspw_list *list, struct grspw_list *rlist, int max)
{
	int cnt;
	struct grspw_pkt *pkt, *last;

	pkt = list->head;

	if ( (max < 1) || (pkt == NULL) ) {
		grspw_list_clr(rlist);
		return 0;
	}

	cnt = 0;
	rlist->head = pkt;
	last = pkt;
	while ((cnt < max) && pkt) {
		last = pkt;
		pkt = pkt->next;
		cnt++;
	}
	rlist->tail = last;
	grspw_list_remove_head_list(list, rlist);
	return cnt;
}

#endif
