/*  SPWTDP - SpaceWire Time Distribution Protocol. The driver provides
 *  device discovery and interrupt management.
 *
 *  COPYRIGHT (c) 2017.
 *  Cobham Gaisler AB
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef __SPWTDP_H__
#define __SPWTDP_H__

#ifdef __cplusplus
extern "C" {
#endif

#define SPWTDP_ERR_OK		0
#define SPWTDP_ERR_EINVAL	-1
#define SPWTDP_ERR_ERROR	-2
#define SPWTDP_ERR_NOINIT	-3

/* Maximum number of SPWTDP devices supported by driver */
#define SPWTDP_MAX 2

/* SPWTDP Register layout */
struct spwtdp_regs {
	volatile unsigned int conf[4];       /* 0x000-0x00C */
	volatile unsigned int stat[4];       /* 0x010-0x01C */
	volatile unsigned int cmd_ctrl;      /* 0x020 */
	volatile unsigned int cmd_et[5];     /* 0x024-0x034 */
	volatile unsigned int resv1[2];      /* 0x038-0x03C */
	volatile unsigned int dat_ctrl;      /* 0x040 */
	volatile unsigned int dat_et[5];     /* 0x044-0x054 */
	volatile unsigned int resv2[2];      /* 0x058-0x05C */
	volatile unsigned int ts_rx_ctrl;    /* 0x060 */
	volatile unsigned int ts_rx_et[5];   /* 0x064-0x074 */
	volatile unsigned int resv3[2];      /* 0x078-0x07C */
	volatile unsigned int ts_tx_ctrl;    /* 0x080 */
	volatile unsigned int ts_tx_et[5];   /* 0x084-0x094 */
	volatile unsigned int resv4[2];      /* 0x098 */
	volatile unsigned int lat_ctrl;      /* 0x0A0 */
	volatile unsigned int lat_et[5];     /* 0x0A4-0x0B4 */
	volatile unsigned int resv5[2];      /* 0x0B8-0x0BC */
	volatile unsigned int ien;           /* 0x0C0 */
	volatile unsigned int ists;          /* 0x0C4 */
	volatile unsigned int dlycnt;        /* 0x0C8 */
	volatile unsigned int dissync;       /* 0x0CC */
	volatile unsigned int resv6[12];     /* 0x0D0-0x0FC */
	volatile unsigned int edmask[4];     /* 0x100-0x10C */
	struct {
		volatile unsigned int ctrl;          /* 0x110, 0x130, 0x150, 0x170 */
		volatile unsigned int et[5];         /* 0x114-0x124, 0x134-0x144, 0x154-0x164, 0x174-0x184,  */
		volatile unsigned int resv0[2];      /* 0x128-0x12C, 0x148-0x14C, 0x168-0x16C, 0x188-0x18C,  */
	} edat[4];                           /* 0x110-0x18C */
	volatile unsigned int resv7[4];      /* 0x190-0x19C */
	volatile unsigned int pulse[8];      /* 0x1A0-0x1BC */
	volatile unsigned int resv8[16];     /* 0x1C0-0x1FC */
};

/*
 * Configuration register definitions 
 */
#define CONF0_JE	(0x1 << CONF0_JE_BIT)
#define CONF0_ST	(0x1 << CONF0_ST_BIT)
#define CONF0_EP	(0x1 << CONF0_EP_BIT)
#define CONF0_ET	(0x1 << CONF0_ET_BIT)
#define CONF0_SP	(0x1 << CONF0_SP_BIT)
#define CONF0_SE	(0x1 << CONF0_SE_BIT)
#define CONF0_LE	(0x1 << CONF0_LE_BIT)
#define CONF0_AE	(0x1 << CONF0_AE_BIT)
#define CONF0_MAP	(0x1f << CONF0_MAP_BIT)
#define CONF0_TD	(0x1 << CONF0_TD_BIT)
#define CONF0_MU	(0x1 << CONF0_MU_BIT)
#define CONF0_SEL	(0x3 << CONF0_SEL_BIT)
#define CONF0_ME	(0x1 << CONF0_ME_BIT)
#define CONF0_RE	(0x1 << CONF0_RE_BIT)
#define CONF0_TE	(0x1 << CONF0_TE_BIT)
#define CONF0_RS	(0x1 << CONF0_RS_BIT)

#define CONF0_JE_BIT	24
#define CONF0_ST_BIT	21
#define CONF0_EP_BIT	20
#define CONF0_ET_BIT	19
#define CONF0_SP_BIT	18
#define CONF0_SE_BIT	17
#define CONF0_LE_BIT	16
#define CONF0_AE_BIT	15
#define CONF0_MAP_BIT	8
#define CONF0_TD_BIT	7
#define CONF0_MU_BIT	6
#define CONF0_SEL_BIT	4
#define CONF0_ME_BIT	3
#define CONF0_RE_BIT	2
#define CONF0_TE_BIT	1
#define CONF0_RS_BIT	0

#define CONF1_FSINC	(0x3fffffff << CONF1_FSINC_BIT)

#define CONF1_FSINC_BIT	0

#define CONF2_CV	(0xffffff << CONF2_CV_BIT)
#define CONF2_ETINC	(0xff << CONF2_ETINC_BIT)

#define CONF2_CV_BIT	8
#define CONF2_ETINC_BIT	0

#define CONF3_OUTPORT	(0xf << CONF3_OUTPORT_BIT)
#define CONF3_INPORT	(0xf << CONF3_INPORT_BIT)
#define CONF3_STM	(0x3f << CONF3_STM_BIT)
#define CONF3_DI64R	(0x1 << CONF3_DI64R_BIT)
#define CONF3_DI64T	(0x1 << CONF3_DI64T_BIT)
#define CONF3_DI64	(0x1 << CONF3_DI64_BIT)
#define CONF3_DI	(0x1 << CONF3_DI_BIT)
#define CONF3_INRX	(0x1f << CONF3_INRX_BIT)
#define CONF3_INTX	(0x1f << CONF3_INTX_BIT)

#define CONF3_OUTPORT_BIT 28
#define CONF3_INPORT_BIT 24
#define CONF3_STM_BIT	16
#define CONF3_DI64R_BIT	13
#define CONF3_DI64T_BIT 12
#define CONF3_DI64_BIT	11
#define CONF3_DI_BIT	10
#define CONF3_INRX_BIT	5
#define CONF3_INTX_BIT	0

/*
 * Control register definitions 
 */
#define CTRL_NC		(0x1 << CTRL_NC_BIT)
#define CTRL_IS		(0x1 << CTRL_IS_BIT)
#define CTRL_SPWTC	(0xff << CTRL_SPWTC_BIT)
#define CTRL_CPF	(0xffff << CTRL_CPF_BIT)

#define CTRL_NC_BIT	31
#define CTRL_IS_BIT	30
#define CTRL_SPWTC_BIT	16
#define CTRL_CPF_BIT	0

/* 
 * Interrupt register definition
 */
#define SPWTDP_IRQ_S		(0x1 << SPWTDP_IRQ_S_BIT)
#define SPWTDP_IRQ_TR		(0x1 << SPWTDP_IRQ_TR_BIT)
#define SPWTDP_IRQ_TM		(0x1 << SPWTDP_IRQ_TM_BIT)
#define SPWTDP_IRQ_TT		(0x1 << SPWTDP_IRQ_TT_BIT)
#define SPWTDP_IRQ_DIR		(0x1 << SPWTDP_IRQ_DIR_BIT)
#define SPWTDP_IRQ_DIT		(0x1 << SPWTDP_IRQ_DIT_BIT)
#define SPWTDP_IRQ_EDI0		(0x1 << SPWTDP_IRQ_EDI0_BIT)
#define SPWTDP_IRQ_EDI1		(0x1 << SPWTDP_IRQ_EDI1_BIT)
#define SPWTDP_IRQ_EDI2		(0x1 << SPWTDP_IRQ_EDI2_BIT)
#define SPWTDP_IRQ_EDI3		(0x1 << SPWTDP_IRQ_EDI3_BIT)
#define SPWTDP_IRQ_SET		(0x1 << SPWTDP_IRQ_SET_BIT)
#define SPWTDP_IRQ_P0		(0x1 << SPWTDP_IRQ_P0_BIT)
#define SPWTDP_IRQ_P1		(0x1 << SPWTDP_IRQ_P1_BIT)
#define SPWTDP_IRQ_P2		(0x1 << SPWTDP_IRQ_P2_BIT)
#define SPWTDP_IRQ_P3		(0x1 << SPWTDP_IRQ_P3_BIT)
#define SPWTDP_IRQ_P4		(0x1 << SPWTDP_IRQ_P4_BIT)
#define SPWTDP_IRQ_P5		(0x1 << SPWTDP_IRQ_P5_BIT)
#define SPWTDP_IRQ_P6		(0x1 << SPWTDP_IRQ_P6_BIT)
#define SPWTDP_IRQ_P7		(0x1 << SPWTDP_IRQ_P7_BIT)
#define SPWTDP_IRQ_NCTC		(0x1 << SPWTDP_IRQ_NCTC_BIT)
#define SPWTDP_IRQ_WCLEAR	\
	(SPWTDP_IRQ_S | SPWTDP_IRQ_TR | SPWTDP_IRQ_TM | \
	 SPWTDP_IRQ_TT | SPWTDP_IRQ_DIR | SPWTDP_IRQ_DIT | SPWTDP_IRQ_EDI0 | \
	 SPWTDP_IRQ_EDI1 | SPWTDP_IRQ_EDI2 | SPWTDP_IRQ_EDI3 | SPWTDP_IRQ_SET |\
	 SPWTDP_IRQ_P0 | SPWTDP_IRQ_P1 | SPWTDP_IRQ_P2 | SPWTDP_IRQ_P3 | \
	 SPWTDP_IRQ_P4 | SPWTDP_IRQ_P5 | SPWTDP_IRQ_P6 | SPWTDP_IRQ_P7 | \
	 SPWTDP_IRQ_NCTC)
#define SPWTDP_IRQ_ALL		(SPWTDP_IRQ_WCLEAR)

#define SPWTDP_IRQ_S_BIT 0
#define SPWTDP_IRQ_TR_BIT 1
#define SPWTDP_IRQ_TM_BIT 2
#define SPWTDP_IRQ_TT_BIT 3
#define SPWTDP_IRQ_DIR_BIT 4
#define SPWTDP_IRQ_DIT_BIT 5
#define SPWTDP_IRQ_EDI0_BIT 6
#define SPWTDP_IRQ_EDI1_BIT 7
#define SPWTDP_IRQ_EDI2_BIT 8
#define SPWTDP_IRQ_EDI3_BIT 9
#define SPWTDP_IRQ_SET_BIT 10
#define SPWTDP_IRQ_P0_BIT 11
#define SPWTDP_IRQ_P1_BIT 12
#define SPWTDP_IRQ_P2_BIT 13
#define SPWTDP_IRQ_P3_BIT 14
#define SPWTDP_IRQ_P4_BIT 15
#define SPWTDP_IRQ_P5_BIT 16
#define SPWTDP_IRQ_P6_BIT 17
#define SPWTDP_IRQ_P7_BIT 18
#define SPWTDP_IRQ_NCTC_BIT 19

/* Register the SPWTDP Driver to the Driver Manager */
void spwtdp_register_drv(void);

/* Open a SPWTDP device by registration order index. A SPWTDP device can only by
 * opened once. The handle returned must be used as the input parameter 'spwtdp'
 * in the rest of the calls in the function interface.
 */
extern void *spwtdp_open(int dev_no);

/* Close a previously opened SPWTDP device */
extern int spwtdp_close(void *spwtdp);

/* Reset SPWTDP Core */
extern int spwtdp_reset(void *spwtdp);

/* Setup the frequency configuration registers */
extern int spwtdp_freq_setup(void *spwtdp, uint32_t fsinc, uint32_t cv,
			     uint8_t etinc);

/* Unmask Interrupts at Interrupt controller */
extern int spwtdp_interrupt_unmask(void *spwtdp, uint32_t irqmask);

/* Mask Interrupts at Interrupt controller */
extern int spwtdp_interrupt_mask(void *spwtdp, uint32_t irqmask);

/* Function ISR callback prototype
 *
 * ists    - Interrupt STatus register of the SPWTDP core read by ISR
 * data    - Custom data provided by user
 */
typedef void (*spwtdp_isr_t)(unsigned int ists, void *data);

/* Register an Interrupt handler and custom data, the function call is
 * removed by calling unregister function.
 */
extern int spwtdp_isr_register(void *spwtdp, spwtdp_isr_t isr, void *data);

/* Unregister an Interrupt handler */
extern int spwtdp_isr_unregister(void *spwtdp);

/* Get and clear interrupt status */
extern int spwtdp_interrupt_status(void *spwtdp, uint32_t *sts,
				   uint32_t clrmask);

/* Setup Initiator and target */
#define SPWTDP_TDP_ENABLE CONF0_TD
#define SPWTDP_TDP_DISABLE 0
#define SPWTDP_LATENCY_ENABLE CONF0_LE
#define SPWTDP_LATENCY_DISABLE 0
#define SPWTDP_EXTET_INC_POLARITY_RISING CONF0_EP
#define SPWTDP_EXTET_INC_POLARITY_FALLING 0
#define SPWTDP_EXTET_INC_ENABLE CONF0_ET
#define SPWTDP_EXTET_INC_DISABLE 0
#define SPWTDP_EXTET_POLARITY_RISING CONF0_SP
#define SPWTDP_EXTET_POLARITY_FALLING 0
#define SPWTDP_EXTET_ENABLE CONF0_SE
#define SPWTDP_EXTET_DISABLE 0
#define SPWTDP_TARGET_SPWSYNC_ENABLE CONF0_ST
#define SPWTDP_TARGET_SPWSYNC_DISABLE 0
#define SPWTDP_TARGET_JITTERC_ENABLE CONF0_JE
#define SPWTDP_TARGET_JITTERC_DISABLE 0
#define SPWTDP_TARGET_MITIGATION_ENABLE CONF0_ME
#define SPWTDP_TARGET_MITIGATION_DISABLE 0
extern int spwtdp_initiator_conf(void *spwtdp, uint8_t mapping,
				 uint32_t options);
extern int spwtdp_target_conf(void *spwtdp, uint8_t mapping, uint32_t options);

/* Setup Initiator and target dist interrupts */
extern int spwtdp_initiator_int_conf(void *spwtdp, uint8_t stm, uint8_t inrx,
				     uint8_t intx);
#define SPWTDP_TARGET_DISTINT_INTACK CONF3_DI
#define SPWTDP_TARGET_DISTINT_INT 0
extern int spwtdp_target_int_conf(void *spwtdp, uint8_t inrx, uint8_t intx,
				  uint32_t options);

/* Enable Initiator and target */
extern int spwtdp_initiator_enable(void *spwtdp);
extern int spwtdp_target_enable(void *spwtdp);

/* Disable Initiator and target */
extern int spwtdp_initiator_disable(void *spwtdp);
extern int spwtdp_target_disable(void *spwtdp);

/* Get and clear status */
extern int spwtdp_status(void *spwtdp, uint32_t *sts, uint32_t clrmask);

/* Define time struct */
/* Length of the max data sample (136 bits), aligned to the next 32-bit word
 * (160 bits=20 byte=5 words)
 */
#define SPWTDP_TIME_DATA_LENGTH 20
struct spwtdp_time_t {
	uint8_t data[SPWTDP_TIME_DATA_LENGTH];
	uint32_t preamble;
};
typedef struct spwtdp_time_t spwtdp_time_t;

/* Get datation elapsed time */
extern int spwtdp_dat_et_get(void *spwtdp, spwtdp_time_t * val);
extern int spwtdp_tsrx_et_get(void *spwtdp, spwtdp_time_t * val);
extern int spwtdp_tstx_et_get(void *spwtdp, spwtdp_time_t * val);
extern int spwtdp_lat_et_get(void *spwtdp, spwtdp_time_t * val);
extern int spwtdp_cmd_et_get(void *spwtdp, spwtdp_time_t * val);

/* Configure TSTX */
extern int spwtdp_initiator_tstx_conf(void *spwtdp, uint8_t tstc);

/* Manage control register */
extern int spwtdp_initiator_cmd_et_set(void *spwtdp, spwtdp_time_t val);
extern int spwtdp_initiator_cmd_spwtc_set(void *spwtdp, uint8_t spwtc);
#define SPWTDP_TARGET_CTRL_NEWCOMMAND_ENABLE CTRL_NC
#define SPWTDP_TARGET_CTRL_NEWCOMMAND_DISABLE 0
#define SPWTDP_TARGET_CTRL_INIT CTRL_IS
#define SPWTDP_TARGET_CTRL_SYNC 0
extern int spwtdp_target_cmd_conf(void *spwtdp, uint8_t spwtc, uint16_t cpf,
				  uint32_t options);

/* Get precision */
extern int spwtdp_precision_get(void *spwtdp, uint8_t *fine, uint8_t *coarse);

#ifdef __cplusplus
}
#endif

#endif
