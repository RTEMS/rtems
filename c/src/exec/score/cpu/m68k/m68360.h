/*
 **************************************************************************
 **************************************************************************
 **                                                                      **
 **  MOTOROLA MC68360 QUAD INTEGRATED COMMUNICATIONS CONTROLLER (QUICC)  **
 **                                                                      **
 **                        HARDWARE DECLARATIONS                         **
 **                                                                      **
 **                                                                      **
 **  Submitted By:                                                       **
 **                                                                      **
 **      W. Eric Norum                                                   **
 **      Saskatchewan Accelerator Laboratory                             **
 **      University of Saskatchewan                                      **
 **      107 North Road                                                  **
 **      Saskatoon, Saskatchewan, CANADA                                 **
 **      S7N 5C6                                                         **
 **                                                                      **
 **      eric@skatter.usask.ca                                           **
 **                                                                      **
 **  $Id$                                                                **
 **                                                                      **
 **************************************************************************
 **************************************************************************
 */

#ifndef __MC68360_h
#define __MC68360_h

/*
 *************************************************************************
 *                         REGISTER SUBBLOCKS                            *
 *************************************************************************
 */

/*
 * Memory controller registers
 */
typedef struct m360MEMCRegisters_ {
	rtems_unsigned32	br;
	rtems_unsigned32	or;
	rtems_unsigned32	_pad[2];
} m360MEMCRegisters_t;

/*
 * Serial Communications Controller registers
 */
typedef struct m360SCCRegisters_ {
	rtems_unsigned32	gsmr_l;
	rtems_unsigned32	gsmr_h;
	rtems_unsigned16	psmr;
	rtems_unsigned16	_pad0;
	rtems_unsigned16	todr;
	rtems_unsigned16	dsr;
	rtems_unsigned16	scce;
	rtems_unsigned16	_pad1;
	rtems_unsigned16	sccm;
	rtems_unsigned8		_pad2;
	rtems_unsigned8		sccs;
	rtems_unsigned32	_pad3[2];
} m360SCCRegisters_t;

/*
 * Serial Management Controller registers
 */
typedef struct m360SMCRegisters_ {
	rtems_unsigned16	_pad0;
	rtems_unsigned16	smcmr;
	rtems_unsigned16	_pad1;
	rtems_unsigned8		smce;
	rtems_unsigned8		_pad2;
	rtems_unsigned16	_pad3;
	rtems_unsigned8		smcm;
	rtems_unsigned8		_pad4;
	rtems_unsigned32	_pad5;
} m360SMCRegisters_t;


/*
 *************************************************************************
 *                         Miscellaneous Parameters                      *
 *************************************************************************
 */
typedef struct m360MiscParms_ {
	rtems_unsigned16	rev_num;
	rtems_unsigned16	_res1;
	rtems_unsigned32	_res2;
	rtems_unsigned32	_res3;
} m360MiscParms_t;

/*
 *************************************************************************
 *                              RISC Timers                              *
 *************************************************************************
 */
typedef struct m360TimerParms_ {
	rtems_unsigned16	tm_base;
	rtems_unsigned16	_tm_ptr;
	rtems_unsigned16	_r_tmr;
	rtems_unsigned16	_r_tmv;
	rtems_unsigned32	tm_cmd;
	rtems_unsigned32	tm_cnt;
} m360TimerParms_t;

/*
 * RISC Controller Configuration Register (RCCR)
 * All other bits in this register are either reserved or
 * used only with a Motorola-supplied RAM microcode packge.
 */
#define M360_RCCR_TIME		(1<<15)	/* Enable timer */
#define M360_RCCR_TIMEP(x)	((x)<<8)	/* Timer period */

/*
 * Command register
 * Set up this register before issuing a M360_CR_OP_SET_TIMER command.
 */
#define M360_TM_CMD_V		(1<<31)		/* Set to enable timer */
#define M360_TM_CMD_R		(1<<30)		/* Set for automatic restart */
#define M360_TM_CMD_TIMER(x)	((x)<<16)	/* Select timer */
#define M360_TM_CMD_PERIOD(x)	(x)		/* Timer period (16 bits) */

/*
 *************************************************************************
 *                               DMA Controllers                         *
 *************************************************************************
 */
typedef struct m360IDMAparms_ {
	rtems_unsigned16	ibase;
	rtems_unsigned16	ibptr;
	rtems_unsigned32	_istate;
	rtems_unsigned32	_itemp;
} m360IDMAparms_t;

/*
 *************************************************************************
 *                   Serial Communication Controllers                    *
 *************************************************************************
 */
typedef struct m360SCCparms_ {
	rtems_unsigned16	rbase;
	rtems_unsigned16	tbase;
	rtems_unsigned8		rfcr;
	rtems_unsigned8		tfcr;
	rtems_unsigned16	mrblr;
	rtems_unsigned32	_rstate;
	rtems_unsigned32	_pad0;
	rtems_unsigned16	_rbptr;
	rtems_unsigned16	_pad1;
	rtems_unsigned32	_pad2;
	rtems_unsigned32	_tstate;
	rtems_unsigned32	_pad3;
	rtems_unsigned16	_tbptr;
	rtems_unsigned16	_pad4;
	rtems_unsigned32	_pad5;
	rtems_unsigned32	_rcrc;
	rtems_unsigned32	_tcrc;
	union {
		struct {
			rtems_unsigned32	_res0;
			rtems_unsigned32	_res1;
			rtems_unsigned16	max_idl;
			rtems_unsigned16	_idlc;
			rtems_unsigned16	brkcr;
			rtems_unsigned16	parec;
			rtems_unsigned16	frmec;
			rtems_unsigned16	nosec;
			rtems_unsigned16	brkec;
			rtems_unsigned16	brklen;
			rtems_unsigned16	uaddr[2];
			rtems_unsigned16	_rtemp;
			rtems_unsigned16	toseq;
			rtems_unsigned16	character[8];
			rtems_unsigned16	rccm;
			rtems_unsigned16	rccr;
			rtems_unsigned16	rlbc;
		} uart;
	} un;
} m360SCCparms_t;

typedef struct m360SCCENparms_ {
	rtems_unsigned16	rbase;
	rtems_unsigned16	tbase;
	rtems_unsigned8		rfcr;
	rtems_unsigned8		tfcr;
	rtems_unsigned16	mrblr;
	rtems_unsigned32	_rstate;
	rtems_unsigned32	_pad0;
	rtems_unsigned16	_rbptr;
	rtems_unsigned16	_pad1;
	rtems_unsigned32	_pad2;
	rtems_unsigned32	_tstate;
	rtems_unsigned32	_pad3;
	rtems_unsigned16	_tbptr;
	rtems_unsigned16	_pad4;
	rtems_unsigned32	_pad5;
	rtems_unsigned32	_rcrc;
	rtems_unsigned32	_tcrc;
	union {
		struct {
			rtems_unsigned32	_res0;
			rtems_unsigned32	_res1;
			rtems_unsigned16	max_idl;
			rtems_unsigned16	_idlc;
			rtems_unsigned16	brkcr;
			rtems_unsigned16	parec;
			rtems_unsigned16	frmec;
			rtems_unsigned16	nosec;
			rtems_unsigned16	brkec;
			rtems_unsigned16	brklen;
			rtems_unsigned16	uaddr[2];
			rtems_unsigned16	_rtemp;
			rtems_unsigned16	toseq;
			rtems_unsigned16	character[8];
			rtems_unsigned16	rccm;
			rtems_unsigned16	rccr;
			rtems_unsigned16	rlbc;
		} uart;
		struct {
			rtems_unsigned32	c_pres;
			rtems_unsigned32	c_mask;
			rtems_unsigned32	crcec;
			rtems_unsigned32	alec;
			rtems_unsigned32	disfc;
			rtems_unsigned16	pads;
			rtems_unsigned16	ret_lim;
			rtems_unsigned16	_ret_cnt;
			rtems_unsigned16	mflr;
			rtems_unsigned16	minflr;
			rtems_unsigned16	maxd1;
			rtems_unsigned16	maxd2;
			rtems_unsigned16	_maxd;
			rtems_unsigned16	dma_cnt;
			rtems_unsigned16	_max_b;
			rtems_unsigned16	gaddr1;
			rtems_unsigned16	gaddr2;
			rtems_unsigned16	gaddr3;
			rtems_unsigned16	gaddr4;
			rtems_unsigned32	_tbuf0data0;
			rtems_unsigned32	_tbuf0data1;
			rtems_unsigned32	_tbuf0rba0;
			rtems_unsigned32	_tbuf0crc;
			rtems_unsigned16	_tbuf0bcnt;
			rtems_unsigned16	paddr_h;
			rtems_unsigned16	paddr_m;
			rtems_unsigned16	paddr_l;
			rtems_unsigned16	p_per;
			rtems_unsigned16	_rfbd_ptr;
			rtems_unsigned16	_tfbd_ptr;
			rtems_unsigned16	_tlbd_ptr;
			rtems_unsigned32	_tbuf1data0;
			rtems_unsigned32	_tbuf1data1;
			rtems_unsigned32	_tbuf1rba0;
			rtems_unsigned32	_tbuf1crc;
			rtems_unsigned16	_tbuf1bcnt;
			rtems_unsigned16	_tx_len;
			rtems_unsigned16	iaddr1;
			rtems_unsigned16	iaddr2;
			rtems_unsigned16	iaddr3;
			rtems_unsigned16	iaddr4;
			rtems_unsigned16	_boff_cnt;
			rtems_unsigned16	taddr_l;
			rtems_unsigned16	taddr_m;
			rtems_unsigned16	taddr_h;
		} ethernet;
	} un;
} m360SCCENparms_t;

/*
 * Receive and transmit function code register bits
 * These apply to the function code registers of all devices, not just SCC.
 */
#define M360_RFCR_MOT		(1<<4)
#define M360_RFCR_DMA_SPACE	0x8
#define M360_TFCR_MOT		(1<<4)
#define M360_TFCR_DMA_SPACE	0x8

/*
 *************************************************************************
 *                     Serial Management Controllers                     *
 *************************************************************************
 */
typedef struct m360SMCparms_ {
	rtems_unsigned16	rbase;
	rtems_unsigned16	tbase;
	rtems_unsigned8		rfcr;
	rtems_unsigned8		tfcr;
	rtems_unsigned16	mrblr;
	rtems_unsigned32	_rstate;
	rtems_unsigned32	_pad0;
	rtems_unsigned16	_rbptr;
	rtems_unsigned16	_pad1;
	rtems_unsigned32	_pad2;
	rtems_unsigned32	_tstate;
	rtems_unsigned32	_pad3;
	rtems_unsigned16	_tbptr;
	rtems_unsigned16	_pad4;
	rtems_unsigned32	_pad5;
	union {
		struct {
			rtems_unsigned16	max_idl;
			rtems_unsigned16	_pad0;
			rtems_unsigned16	brklen;
			rtems_unsigned16	brkec;
			rtems_unsigned16	brkcr;
			rtems_unsigned16	_r_mask;
		} uart;
		struct {
			rtems_unsigned16	_pad0[5];
		} transparent;
	} un;
} m360SMCparms_t;

/*
 * Mode register
 */
#define M360_SMCMR_CLEN(x)		((x)<<11)	/* Character length */
#define M360_SMCMR_2STOP		(1<<10)	/* 2 stop bits */
#define M360_SMCMR_PARITY		(1<<9)	/* Enable parity */
#define M360_SMCMR_EVEN			(1<<8)	/* Even parity */
#define M360_SMCMR_SM_GCI		(0<<4)	/* GCI Mode */
#define M360_SMCMR_SM_UART		(2<<4)	/* UART Mode */
#define M360_SMCMR_SM_TRANSPARENT	(3<<4)	/* Transparent Mode */
#define M360_SMCMR_DM_LOOPBACK		(1<<2)	/* Local loopback mode */
#define M360_SMCMR_DM_ECHO		(2<<2)	/* Echo mode */
#define M360_SMCMR_TEN			(1<<1)	/* Enable transmitter */
#define M360_SMCMR_REN			(1<<0)	/* Enable receiver */

/*
 * Event and mask registers (SMCE, SMCM)
 */
#define M360_SMCE_BRK	(1<<4)
#define M360_SMCE_BSY	(1<<2)
#define M360_SMCE_TX	(1<<1)
#define M360_SMCE_RX	(1<<0)

/*
 *************************************************************************
 *                      Serial Peripheral Interface                      *
 *************************************************************************
 */
typedef struct m360SPIparms_ {
	rtems_unsigned16	rbase;
	rtems_unsigned16	tbase;
	rtems_unsigned8		rfcr;
	rtems_unsigned8		tfcr;
	rtems_unsigned16	mrblr;
	rtems_unsigned32	_rstate;
	rtems_unsigned32	_pad0;
	rtems_unsigned16	_rbptr;
	rtems_unsigned16	_pad1;
	rtems_unsigned32	_pad2;
	rtems_unsigned32	_tstate;
	rtems_unsigned32	_pad3;
	rtems_unsigned16	_tbptr;
	rtems_unsigned16	_pad4;
	rtems_unsigned32	_pad5;
} m360SPIparms_t;

/*
 * Mode register (SPMODE)
 */
#define M360_SPMODE_LOOP		(1<<14)	/* Local loopback mode */
#define M360_SPMODE_CI			(1<<13)	/* Clock invert */
#define M360_SPMODE_CP			(1<<12)	/* Clock phase */
#define M360_SPMODE_DIV16		(1<<11)	/* Divide BRGCLK by 16 */
#define M360_SPMODE_REV			(1<<10)	/* Reverse data */
#define M360_SPMODE_MASTER		(1<<9)	/* SPI is master */
#define M360_SPMODE_EN			(1<<8)	/* Enable SPI */
#define M360_SPMODE_CLEN(x)		((x)<<4)	/* Character length */
#define M360_SPMODE_PM(x)		(x)	/* Prescaler modulus */

/*
 * Mode register (SPCOM)
 */
#define M360_SPCOM_STR			(1<<7)	/* Start transmit */

/*
 * Event and mask registers (SPIE, SPIM)
 */
#define M360_SPIE_MME	(1<<5)		/* Multi-master error */
#define M360_SPIE_TXE	(1<<4)		/* Tx error */
#define M360_SPIE_BSY	(1<<2)		/* Busy condition*/
#define M360_SPIE_TXB	(1<<1)		/* Tx buffer */
#define M360_SPIE_RXB	(1<<0)		/* Rx buffer */

/*
 *************************************************************************
 *                 SDMA (SCC, SMC, SPI) Buffer Descriptors               *
 *************************************************************************
 */
typedef struct m360BufferDescriptor_ {
	rtems_unsigned16	status;
	rtems_unsigned16	length;
	volatile void		*buffer;
} m360BufferDescriptor_t;

/*
 * Bits in receive buffer descriptor status word
 */
#define M360_BD_EMPTY		(1<<15)	/* Ethernet, SCC UART, SMC UART, SPI */
#define M360_BD_WRAP		(1<<13)	/* Ethernet, SCC UART, SMC UART, SPI */
#define M360_BD_INTERRUPT	(1<<12)	/* Ethernet, SCC UART, SMC UART, SPI */
#define M360_BD_LAST		(1<<11)	/* Ethernet, SPI */
#define M360_BD_CONTROL_CHAR	(1<<11)	/* SCC UART */
#define M360_BD_FIRST_IN_FRAME	(1<<10)	/* Ethernet */
#define M360_BD_ADDRESS		(1<<10)	/* SCC UART */
#define M360_BD_CONTINUOUS	(1<<9)	/* SCC UART, SMC UART, SPI */
#define M360_BD_MISS		(1<<8)	/* Ethernet */
#define M360_BD_IDLE		(1<<8)	/* SCC UART, SMC UART */
#define M360_BD_ADDRSS_MATCH	(1<<7)	/* SCC UART */
#define M360_BD_LONG		(1<<5)	/* Ethernet */
#define M360_BD_BREAK		(1<<5)	/* SCC UART, SMC UART */
#define M360_BD_NONALIGNED	(1<<4)	/* Ethernet */
#define M360_BD_FRAMING_ERROR	(1<<4)	/* SCC UART, SMC UART */
#define M360_BD_SHORT		(1<<3)	/* Ethernet */
#define M360_BD_PARITY_ERROR	(1<<3)	/* SCC UART, SMC UART */
#define M360_BD_CRC_ERROR	(1<<2)	/* Ethernet */
#define M360_BD_OVERRUN		(1<<1)	/* Ethernet, SCC UART, SMC UART, SPI */
#define M360_BD_COLLISION	(1<<0)	/* Ethernet */
#define M360_BD_CARRIER_LOST	(1<<0)	/* SCC UART */
#define M360_BD_MASTER_ERROR	(1<<0)	/* SPI */

/*
 * Bits in transmit buffer descriptor status word
 * Many bits have the same meaning as those in receiver buffer descriptors.
 */
#define M360_BD_READY		(1<<15)	/* Ethernet, SCC UART, SMC UART, SPI */
#define M360_BD_PAD		(1<<14)	/* Ethernet */
#define M360_BD_CTS_REPORT	(1<<11)	/* SCC UART */
#define M360_BD_TX_CRC		(1<<10)	/* Ethernet */
#define M360_BD_DEFER		(1<<9)	/* Ethernet */
#define M360_BD_HEARTBEAT	(1<<8)	/* Ethernet */
#define M360_BD_PREAMBLE	(1<<8)	/* SCC UART, SMC UART */
#define M360_BD_LATE_COLLISION	(1<<7)	/* Ethernet */
#define M360_BD_NO_STOP_BIT	(1<<7)	/* SCC UART */
#define M360_BD_RETRY_LIMIT	(1<<6)	/* Ethernet */
#define M360_BD_RETRY_COUNT(x)	(((x)&0x3C)>>2)	/* Ethernet */
#define M360_BD_UNDERRUN	(1<<1)	/* Ethernet, SPI */
#define M360_BD_CARRIER_LOST	(1<<0)	/* Ethernet */
#define M360_BD_CTS_LOST	(1<<0)	/* SCC UART */

/*
 *************************************************************************
 *                           IDMA Buffer Descriptors                     *
 *************************************************************************
 */
typedef struct m360IDMABufferDescriptor_ {
	rtems_unsigned16	status;
	rtems_unsigned16	_pad;
	rtems_unsigned32	length;
	void			*source;
	void			*destination;
} m360IDMABufferDescriptor_t;

/*
 *************************************************************************
 *       RISC Communication Processor Module Command Register (CR)       *
 *************************************************************************
 */
#define M360_CR_RST		(1<<15)	/* Reset communication processor */
#define M360_CR_OP_INIT_RX_TX	(0<<8)	/* SCC, SMC UART, SMC GCI, SPI */
#define M360_CR_OP_INIT_RX	(1<<8)	/* SCC, SMC UART, SPI */
#define M360_CR_OP_INIT_TX	(2<<8)	/* SCC, SMC UART, SPI */
#define M360_CR_OP_INIT_HUNT	(3<<8)	/* SCC, SMC UART */
#define M360_CR_OP_STOP_TX	(4<<8)	/* SCC, SMC UART */
#define M360_CR_OP_GR_STOP_TX	(5<<8)	/* SCC */
#define M360_CR_OP_INIT_IDMA	(5<<8)	/* IDMA */
#define M360_CR_OP_RESTART_TX	(6<<8)	/* SCC, SMC UART */
#define M360_CR_OP_CLOSE_RX_BD	(7<<8)	/* SCC, SMC UART, SPI */
#define M360_CR_OP_SET_GRP_ADDR	(8<<8)	/* SCC */
#define M360_CR_OP_SET_TIMER	(8<<8)	/* Timer */
#define M360_CR_OP_GCI_TIMEOUT	(9<<8)	/* SMC GCI */
#define M360_CR_OP_RESERT_BCS	(10<<8)	/* SCC */
#define M360_CR_OP_GCI_ABORT	(10<<8)	/* SMC GCI */
#define M360_CR_CHAN_SCC1	(0<<4)	/* Channel selection */
#define M360_CR_CHAN_SCC2	(4<<4)
#define M360_CR_CHAN_SPI	(5<<4)
#define M360_CR_CHAN_TIMER	(5<<4)
#define M360_CR_CHAN_SCC3	(8<<4)
#define M360_CR_CHAN_SMC1	(9<<4)
#define M360_CR_CHAN_IDMA1	(9<<4)
#define M360_CR_CHAN_SCC4	(12<<4)
#define M360_CR_CHAN_SMC2	(13<<4)
#define M360_CR_CHAN_IDMA2	(13<<4)
#define M360_CR_FLG		(1<<0)	/* Command flag */

/*
 *************************************************************************
 *                 System Protection Control Register (SYPCR)            *
 *************************************************************************
 */
#define M360_SYPCR_SWE		(1<<7)  /* Software watchdog enable */
#define M360_SYPCR_SWRI		(1<<6)  /* Software watchdog reset select */
#define M360_SYPCR_SWT1		(1<<5)  /* Software watchdog timing bit 1 */
#define M360_SYPCR_SWT0		(1<<4)  /* Software watchdog timing bit 0 */
#define M360_SYPCR_DBFE		(1<<3)  /* Double bus fault monitor enable */
#define M360_SYPCR_BME		(1<<2)  /* Bus monitor external enable */
#define M360_SYPCR_BMT1		(1<<1)  /* Bus monitor timing bit 1 */
#define M360_SYPCR_BMT0		(1<<0)  /* Bus monitor timing bit 0 */

/*
 *************************************************************************
 *                        Memory Control Registers                       *
 *************************************************************************
 */
#define M360_GMR_RCNT(x)	((x)<<24)	/* Refresh count */
#define M360_GMR_RFEN		(1<<23)	/* Refresh enable */
#define M360_GMR_RCYC(x)	((x)<<21)	/* Refresh cycle length */
#define M360_GMR_PGS(x)		((x)<<18)	/* Page size */
#define M360_GMR_DPS_32BIT	(0<<16)	/* DRAM port size */
#define M360_GMR_DPS_16BIT	(1<<16)
#define M360_GMR_DPS_8BIT	(2<<16)
#define M360_GMR_DPS_DSACK	(3<<16)
#define M360_GMR_WBT40		(1<<15)	/* Wait between 040 transfers */
#define M360_GMR_WBTQ		(1<<14)	/* Wait between 360 transfers */
#define M360_GMR_SYNC		(1<<13)	/* Synchronous external access */
#define M360_GMR_EMWS		(1<<12)	/* External master wait state */
#define M360_GMR_OPAR		(1<<11)	/* Odd parity */
#define M360_GMR_PBEE		(1<<10)	/* Parity bus error enable */
#define M360_GMR_TSS40		(1<<9)	/* TS* sample for 040 */
#define M360_GMR_NCS		(1<<8)	/* No CPU space */
#define M360_GMR_DWQ		(1<<7)	/* Delay write for 360 */
#define M360_GMR_DW40		(1<<6)	/* Delay write for 040 */
#define M360_GMR_GAMX		(1<<5)	/* Global address mux enable */

#define M360_MEMC_BR_FC(x)	((x)<<7)	/* Function code limit */
#define M360_MEMC_BR_TRLXQ	(1<<6)	/* Relax timing requirements */
#define M360_MEMC_BR_BACK40	(1<<5)	/* Burst acknowledge to 040 */
#define M360_MEMC_BR_CSNT40	(1<<4)	/* CS* negate timing for 040 */
#define M360_MEMC_BR_CSNTQ	(1<<3)	/* CS* negate timing for 360 */
#define M360_MEMC_BR_PAREN	(1<<2)	/* Enable parity checking */
#define M360_MEMC_BR_WP		(1<<1)	/* Write Protect */
#define M360_MEMC_BR_V		(1<<0)	/* Base/Option register are valid */

#define M360_MEMC_OR_TCYC(x)	((x)<<28)	/* Cycle length (clocks) */
#define M360_MEMC_OR_WAITS(x)	M360_MEMC_OR_TCYC((x)+1)
#define M360_MEMC_OR_2KB	0x0FFFF800	/* Address range */
#define M360_MEMC_OR_4KB	0x0FFFF000
#define M360_MEMC_OR_8KB	0x0FFFE000
#define M360_MEMC_OR_16KB	0x0FFFC000
#define M360_MEMC_OR_32KB	0x0FFF8000
#define M360_MEMC_OR_64KB	0x0FFF0000
#define M360_MEMC_OR_128KB	0x0FFE0000
#define M360_MEMC_OR_256KB	0x0FFC0000
#define M360_MEMC_OR_512KB	0x0FF80000
#define M360_MEMC_OR_1MB	0x0FF00000
#define M360_MEMC_OR_2MB	0x0FE00000
#define M360_MEMC_OR_4MB	0x0FC00000
#define M360_MEMC_OR_8MB	0x0F800000
#define M360_MEMC_OR_16MB	0x0F000000
#define M360_MEMC_OR_32MB	0x0E000000
#define M360_MEMC_OR_64MB	0x0C000000
#define M360_MEMC_OR_128MB	0x08000000
#define M360_MEMC_OR_256MB	0x00000000
#define M360_MEMC_OR_FCMC(x)	((x)<<7)	/* Function code mask */
#define M360_MEMC_OR_BCYC(x)	((x)<<5)	/* Burst cycle length (clocks) */
#define M360_MEMC_OR_PGME	(1<<3)		/* Page mode enable */
#define M360_MEMC_OR_32BIT	(0<<1)		/* Port size */
#define M360_MEMC_OR_16BIT	(1<<1)
#define M360_MEMC_OR_8BIT	(2<<1)
#define M360_MEMC_OR_DSACK	(3<<1)
#define M360_MEMC_OR_DRAM	(1<<0)		/* Dynamic RAM select */

/*
 *************************************************************************
 *                         SI Mode Register (SIMODE)                     *
 *************************************************************************
 */
#define M360_SI_SMC2_BITS	0xFFFF0000	/* All SMC2 bits */
#define M360_SI_SMC2_TDM	(1<<31)	/* Multiplexed SMC2 */
#define M360_SI_SMC2_BRG1	(0<<28)	/* SMC2 clock souce */
#define M360_SI_SMC2_BRG2	(1<<28)
#define M360_SI_SMC2_BRG3	(2<<28)
#define M360_SI_SMC2_BRG4	(3<<28)
#define M360_SI_SMC2_CLK5	(0<<28)
#define M360_SI_SMC2_CLK6	(1<<28)
#define M360_SI_SMC2_CLK7	(2<<28)
#define M360_SI_SMC2_CLK8	(3<<28)
#define M360_SI_SMC1_BITS	0x0000FFFF	/* All SMC1 bits */
#define M360_SI_SMC1_TDM	(1<<15)	/* Multiplexed SMC1 */
#define M360_SI_SMC1_BRG1	(0<<12)	/* SMC1 clock souce */
#define M360_SI_SMC1_BRG2	(1<<12)
#define M360_SI_SMC1_BRG3	(2<<12)
#define M360_SI_SMC1_BRG4	(3<<12)
#define M360_SI_SMC1_CLK1	(0<<12)
#define M360_SI_SMC1_CLK2	(1<<12)
#define M360_SI_SMC1_CLK3	(2<<12)
#define M360_SI_SMC1_CLK4	(3<<12)

/*
 *************************************************************************
 *                  SDMA Configuration Register (SDMA)                   *
 *************************************************************************
 */
#define M360_SDMA_FREEZE	(2<<13)	/* Freeze on next bus cycle */
#define M360_SDMA_SISM_7	(7<<8)	/* Normal interrupt service mask */
#define M360_SDMA_SAID_4	(4<<4)	/* Normal arbitration ID */
#define M360_SDMA_INTE		(1<<1)	/* SBER interrupt enable */
#define M360_SDMA_INTB		(1<<0)	/* SBKP interrupt enable */

/*
 *************************************************************************
 *                      Baud (sic) Rate Generators                       *
 *************************************************************************
 */
#define M360_BRG_RST		(1<<17)		/* Reset generator */
#define M360_BRG_EN		(1<<16)		/* Enable generator */
#define M360_BRG_EXTC_BRGCLK	(0<<14)		/* Source is BRGCLK */
#define M360_BRG_EXTC_CLK2	(1<<14)		/* Source is CLK2 pin */
#define M360_BRG_EXTC_CLK6	(2<<14)		/* Source is CLK6 pin */
#define M360_BRG_ATB		(1<<13)		/* Autobaud */
#define M360_BRG_115200		(13<<1)		/* Assume 25 MHz clock */
#define M360_BRG_57600		(26<<1)
#define M360_BRG_38400		(40<<1)
#define M360_BRG_19200		(80<<1)
#define M360_BRG_9600		(162<<1)
#define M360_BRG_4800		(324<<1)
#define M360_BRG_2400		(650<<1)
#define M360_BRG_1200		(1301<<1)
#define M360_BRG_600		(2603<<1)
#define M360_BRG_300		((324<<1) | 1)
#define M360_BRG_150		((650<<1) | 1)
#define M360_BRG_75		((1301<<1) | 1)

/*
 *************************************************************************
 *                 MC68360 DUAL-PORT RAM AND REGISTERS                   *
 *************************************************************************
 */
typedef struct m360_ {
	/*
	 * Dual-port RAM
	 */
	rtems_unsigned8		dpram0[0x400];	/* Microcode program */
	rtems_unsigned8		dpram1[0x200];
	rtems_unsigned8		dpram2[0x100];	/* Microcode scratch */
	rtems_unsigned8		dpram3[0x100];	/* Not on REV A or B masks */
	rtems_unsigned8		_rsv0[0xC00-0x800];
	m360SCCENparms_t	scc1p;
	rtems_unsigned8		_rsv1[0xCB0-0xC00-sizeof(m360SCCENparms_t)];
	m360MiscParms_t		miscp;
	rtems_unsigned8		_rsv2[0xD00-0xCB0-sizeof(m360MiscParms_t)];
	m360SCCparms_t		scc2p;
	rtems_unsigned8		_rsv3[0xD80-0xD00-sizeof(m360SCCparms_t)];
	m360SPIparms_t		spip;
	rtems_unsigned8		_rsv4[0xDB0-0xD80-sizeof(m360SPIparms_t)];
	m360TimerParms_t	tmp;
	rtems_unsigned8		_rsv5[0xE00-0xDB0-sizeof(m360TimerParms_t)];
	m360SCCparms_t		scc3p;
	rtems_unsigned8		_rsv6[0xE70-0xE00-sizeof(m360SCCparms_t)];
	m360IDMAparms_t		idma1p;
	rtems_unsigned8		_rsv7[0xE80-0xE70-sizeof(m360IDMAparms_t)];
	m360SMCparms_t		smc1p;
	rtems_unsigned8		_rsv8[0xF00-0xE80-sizeof(m360SMCparms_t)];
	m360SCCparms_t		scc4p;
	rtems_unsigned8		_rsv9[0xF70-0xF00-sizeof(m360SCCparms_t)];
	m360IDMAparms_t		idma2p;
	rtems_unsigned8		_rsv10[0xF80-0xF70-sizeof(m360IDMAparms_t)];
	m360SMCparms_t		smc2p;
	rtems_unsigned8		_rsv11[0x1000-0xF80-sizeof(m360SMCparms_t)];

	/*
	 * SIM Block
	 */
	rtems_unsigned32	mcr;
	rtems_unsigned32	_pad00;
	rtems_unsigned8		avr;
	rtems_unsigned8		rsr;
	rtems_unsigned16	_pad01;
	rtems_unsigned8		clkocr;
	rtems_unsigned8		_pad02;
	rtems_unsigned16	_pad03;
	rtems_unsigned16	pllcr;
	rtems_unsigned16	_pad04;
	rtems_unsigned16	cdvcr;
	rtems_unsigned16	pepar;
	rtems_unsigned32	_pad05[2];
	rtems_unsigned16	_pad06;
	rtems_unsigned8		sypcr;
	rtems_unsigned8		swiv;
	rtems_unsigned16	_pad07;
	rtems_unsigned16	picr;
	rtems_unsigned16	_pad08;
	rtems_unsigned16	pitr;
	rtems_unsigned16	_pad09;
	rtems_unsigned8		_pad10;
	rtems_unsigned8		swsr;
	rtems_unsigned32	bkar;
	rtems_unsigned32	bcar;
	rtems_unsigned32	_pad11[2];

	/*
	 * MEMC Block
	 */
	rtems_unsigned32	gmr;
	rtems_unsigned16	mstat;
	rtems_unsigned16	_pad12;
	rtems_unsigned32	_pad13[2];
	m360MEMCRegisters_t	memc[8];
	rtems_unsigned8		_pad14[0xF0-0xD0];
	rtems_unsigned8		_pad15[0x100-0xF0];
	rtems_unsigned8		_pad16[0x500-0x100];
	
	/*
	 * IDMA1 Block
	 */
	rtems_unsigned16	iccr;
	rtems_unsigned16	_pad17;
	rtems_unsigned16	cmr1;
	rtems_unsigned16	_pad18;
	rtems_unsigned32	sapr1;
	rtems_unsigned32	dapr1;
	rtems_unsigned32	bcr1;
	rtems_unsigned8		fcr1;
	rtems_unsigned8		_pad19;
	rtems_unsigned8		cmar1;
	rtems_unsigned8		_pad20;
	rtems_unsigned8		csr1;
	rtems_unsigned8		_pad21;
	rtems_unsigned16	_pad22;
	
	/*
	 * SDMA Block
	 */
	rtems_unsigned8		sdsr;
	rtems_unsigned8		_pad23;
	rtems_unsigned16	sdcr;
	rtems_unsigned32	sdar;
	
	/*
	 * IDMA2 Block
	 */
	rtems_unsigned16	_pad24;
	rtems_unsigned16	cmr2;
	rtems_unsigned32	sapr2;
	rtems_unsigned32	dapr2;
	rtems_unsigned32	bcr2;
	rtems_unsigned8		fcr2;
	rtems_unsigned8		_pad26;
	rtems_unsigned8		cmar2;
	rtems_unsigned8		_pad27;
	rtems_unsigned8		csr2;
	rtems_unsigned8		_pad28;
	rtems_unsigned16	_pad29;
	rtems_unsigned32	_pad30;
	
	/*
	 * CPIC Block
	 */
	rtems_unsigned32	cicr;
	rtems_unsigned32	cipr;
	rtems_unsigned32	cimr;
	rtems_unsigned32	cisr;

	/*
	 * Parallel I/O Block
	 */
	rtems_unsigned16	padir;
	rtems_unsigned16	papar;
	rtems_unsigned16	paodr;
	rtems_unsigned16	padat;
	rtems_unsigned32	_pad31[2];
	rtems_unsigned16	pcdir;
	rtems_unsigned16	pcpar;
	rtems_unsigned16	pcso;
	rtems_unsigned16	pcdat;
	rtems_unsigned16	pcint;
	rtems_unsigned16	_pad32;
	rtems_unsigned32	_pad33[5];
	
	/*
	 * TIMER Block
	 */
	rtems_unsigned16	tgcr;
	rtems_unsigned16	_pad34;
	rtems_unsigned32	_pad35[3];
	rtems_unsigned16	tmr1;
	rtems_unsigned16	tmr2;
	rtems_unsigned16	trr1;
	rtems_unsigned16	trr2;
	rtems_unsigned16	tcr1;
	rtems_unsigned16	tcr2;
	rtems_unsigned16	tcn1;
	rtems_unsigned16	tcn2;
	rtems_unsigned16	tmr3;
	rtems_unsigned16	tmr4;
	rtems_unsigned16	trr3;
	rtems_unsigned16	trr4;
	rtems_unsigned16	tcr3;
	rtems_unsigned16	tcr4;
	rtems_unsigned16	tcn3;
	rtems_unsigned16	tcn4;
	rtems_unsigned16	ter1;
	rtems_unsigned16	ter2;
	rtems_unsigned16	ter3;
	rtems_unsigned16	ter4;
	rtems_unsigned32	_pad36[2];
	
	/*
	 * CP Block
	 */
	rtems_unsigned16	cr;
	rtems_unsigned16	_pad37;
	rtems_unsigned16	rccr;
	rtems_unsigned16	_pad38;
	rtems_unsigned32	_pad39[3];
	rtems_unsigned16	_pad40;
	rtems_unsigned16	rter;
	rtems_unsigned16	_pad41;
	rtems_unsigned16	rtmr;
	rtems_unsigned32	_pad42[5];

	/*
	 * BRG Block
	 */
	rtems_unsigned32	brgc1;
	rtems_unsigned32	brgc2;
	rtems_unsigned32	brgc3;
	rtems_unsigned32	brgc4;

	/*
	 * SCC Block
	 */
	m360SCCRegisters_t	scc1;
	m360SCCRegisters_t	scc2;
	m360SCCRegisters_t	scc3;
	m360SCCRegisters_t	scc4;

	/*
	 * SMC Block
	 */
	m360SMCRegisters_t	smc1;
	m360SMCRegisters_t	smc2;

	/*
	 * SPI Block
	 */
	rtems_unsigned16	spmode;
	rtems_unsigned16	_pad43[2];
	rtems_unsigned8		spie;
	rtems_unsigned8		_pad44;
	rtems_unsigned16	_pad45;
	rtems_unsigned8		spim;
	rtems_unsigned8		_pad46[2];
	rtems_unsigned8		spcom;
	rtems_unsigned16	_pad47[2];

	/*
	 * PIP Block
	 */
	rtems_unsigned16	pipc;
	rtems_unsigned16	_pad48;
	rtems_unsigned16	ptpr;
	rtems_unsigned32	pbdir;
	rtems_unsigned32	pbpar;
	rtems_unsigned16	_pad49;
	rtems_unsigned16	pbodr;
	rtems_unsigned32	pbdat;
	rtems_unsigned32	_pad50[6];

	/*
	 * SI Block
	 */
	rtems_unsigned32	simode;
	rtems_unsigned8		sigmr;
	rtems_unsigned8		_pad51;
	rtems_unsigned8		sistr;
	rtems_unsigned8		sicmr;
	rtems_unsigned32	_pad52;
	rtems_unsigned32	sicr;
	rtems_unsigned16	_pad53;
	rtems_unsigned16	sirp[2];
	rtems_unsigned16	_pad54;
	rtems_unsigned32	_pad55[2];
	rtems_unsigned8		siram[256];
} m360_t;

extern volatile m360_t m360;

#endif /* __MC68360_h */
