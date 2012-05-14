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
 **************************************************************************
 **************************************************************************
 */

#ifndef _RTEMS_M68K_M68360_H
#define _RTEMS_M68K_M68360_H

/*
 *************************************************************************
 *                         REGISTER SUBBLOCKS                            *
 *************************************************************************
 */

/*
 * Memory controller registers
 */
typedef struct m360MEMCRegisters_ {
	unsigned long		br;
	unsigned long		or;
	unsigned long		_pad[2];
} m360MEMCRegisters_t;

/*
 * Serial Communications Controller registers
 */
typedef struct m360SCCRegisters_ {
	unsigned long		gsmr_l;
	unsigned long		gsmr_h;
	unsigned short		psmr;
	unsigned short		_pad0;
	unsigned short		todr;
	unsigned short		dsr;
	unsigned short		scce;
	unsigned short		_pad1;
	unsigned short		sccm;
	unsigned char		_pad2;
	unsigned char		sccs;
	unsigned long		_pad3[2];
} m360SCCRegisters_t;

/*
 * Serial Management Controller registers
 */
typedef struct m360SMCRegisters_ {
	unsigned short		_pad0;
	unsigned short		smcmr;
	unsigned short		_pad1;
	unsigned char		smce;
	unsigned char		_pad2;
	unsigned short		_pad3;
	unsigned char		smcm;
	unsigned char		_pad4;
	unsigned long		_pad5;
} m360SMCRegisters_t;


/*
 *************************************************************************
 *                         Miscellaneous Parameters                      *
 *************************************************************************
 */
typedef struct m360MiscParms_ {
	unsigned short		rev_num;
	unsigned short		_res1;
	unsigned long		_res2;
	unsigned long		_res3;
} m360MiscParms_t;

/*
 *************************************************************************
 *                              RISC Timers                              *
 *************************************************************************
 */
typedef struct m360TimerParms_ {
	unsigned short		tm_base;
	unsigned short		_tm_ptr;
	unsigned short		_r_tmr;
	unsigned short		_r_tmv;
	unsigned long		tm_cmd;
	unsigned long		tm_cnt;
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
	unsigned short		ibase;
	unsigned short		ibptr;
	unsigned long		_istate;
	unsigned long		_itemp;
} m360IDMAparms_t;

/*
 *************************************************************************
 *                   Serial Communication Controllers                    *
 *************************************************************************
 */
typedef struct m360SCCparms_ {
	unsigned short		rbase;
	unsigned short		tbase;
	unsigned char		rfcr;
	unsigned char		tfcr;
	unsigned short		mrblr;
	unsigned long		_rstate;
	unsigned long		_pad0;
	unsigned short		_rbptr;
	unsigned short		_pad1;
	unsigned long		_pad2;
	unsigned long		_tstate;
	unsigned long		_pad3;
	unsigned short		_tbptr;
	unsigned short		_pad4;
	unsigned long		_pad5;
	unsigned long		_rcrc;
	unsigned long		_tcrc;
	union {
		struct {
			unsigned long		_res0;
			unsigned long		_res1;
			unsigned short		max_idl;
			unsigned short		_idlc;
			unsigned short		brkcr;
			unsigned short		parec;
			unsigned short		frmec;
			unsigned short		nosec;
			unsigned short		brkec;
			unsigned short		brklen;
			unsigned short		uaddr[2];
			unsigned short		_rtemp;
			unsigned short		toseq;
			unsigned short		character[8];
			unsigned short		rccm;
			unsigned short		rccr;
			unsigned short		rlbc;
		} uart;
		struct {
			unsigned long		crc_p;
			unsigned long		crc_c;
		} transparent;

	} un;
} m360SCCparms_t;

typedef struct m360SCCENparms_ {
	unsigned short		rbase;
	unsigned short		tbase;
	unsigned char		rfcr;
	unsigned char		tfcr;
	unsigned short		mrblr;
	unsigned long		_rstate;
	unsigned long		_pad0;
	unsigned short		_rbptr;
	unsigned short		_pad1;
	unsigned long		_pad2;
	unsigned long		_tstate;
	unsigned long		_pad3;
	unsigned short		_tbptr;
	unsigned short		_pad4;
	unsigned long		_pad5;
	unsigned long		_rcrc;
	unsigned long		_tcrc;
	union {
		struct {
			unsigned long		_res0;
			unsigned long		_res1;
			unsigned short		max_idl;
			unsigned short		_idlc;
			unsigned short		brkcr;
			unsigned short		parec;
			unsigned short		frmec;
			unsigned short		nosec;
			unsigned short		brkec;
			unsigned short		brklen;
			unsigned short		uaddr[2];
			unsigned short		_rtemp;
			unsigned short		toseq;
			unsigned short		character[8];
			unsigned short		rccm;
			unsigned short		rccr;
			unsigned short		rlbc;
		} uart;
		struct {
			unsigned long		c_pres;
			unsigned long		c_mask;
			unsigned long		crcec;
			unsigned long		alec;
			unsigned long		disfc;
			unsigned short		pads;
			unsigned short		ret_lim;
			unsigned short		_ret_cnt;
			unsigned short		mflr;
			unsigned short		minflr;
			unsigned short		maxd1;
			unsigned short		maxd2;
			unsigned short		_maxd;
			unsigned short		dma_cnt;
			unsigned short		_max_b;
			unsigned short		gaddr1;
			unsigned short		gaddr2;
			unsigned short		gaddr3;
			unsigned short		gaddr4;
			unsigned long		_tbuf0data0;
			unsigned long		_tbuf0data1;
			unsigned long		_tbuf0rba0;
			unsigned long		_tbuf0crc;
			unsigned short		_tbuf0bcnt;
			unsigned short		paddr_h;
			unsigned short		paddr_m;
			unsigned short		paddr_l;
			unsigned short		p_per;
			unsigned short		_rfbd_ptr;
			unsigned short		_tfbd_ptr;
			unsigned short		_tlbd_ptr;
			unsigned long		_tbuf1data0;
			unsigned long		_tbuf1data1;
			unsigned long		_tbuf1rba0;
			unsigned long		_tbuf1crc;
			unsigned short		_tbuf1bcnt;
			unsigned short		_tx_len;
			unsigned short		iaddr1;
			unsigned short		iaddr2;
			unsigned short		iaddr3;
			unsigned short		iaddr4;
			unsigned short		_boff_cnt;
			unsigned short		taddr_h;
			unsigned short		taddr_m;
			unsigned short		taddr_l;
		} ethernet;
		struct {
			unsigned long		crc_p;
			unsigned long		crc_c;
		} transparent;
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
	unsigned short		rbase;
	unsigned short		tbase;
	unsigned char		rfcr;
	unsigned char		tfcr;
	unsigned short		mrblr;
	unsigned long		_rstate;
	unsigned long		_pad0;
	unsigned short		_rbptr;
	unsigned short		_pad1;
	unsigned long		_pad2;
	unsigned long		_tstate;
	unsigned long		_pad3;
	unsigned short		_tbptr;
	unsigned short		_pad4;
	unsigned long		_pad5;
	union {
		struct {
			unsigned short		max_idl;
			unsigned short		_pad0;
			unsigned short		brklen;
			unsigned short		brkec;
			unsigned short		brkcr;
			unsigned short		_r_mask;
		} uart;
		struct {
			unsigned short		_pad0[5];
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
	unsigned short		rbase;
	unsigned short		tbase;
	unsigned char		rfcr;
	unsigned char		tfcr;
	unsigned short		mrblr;
	unsigned long		_rstate;
	unsigned long		_pad0;
	unsigned short		_rbptr;
	unsigned short		_pad1;
	unsigned long		_pad2;
	unsigned long		_tstate;
	unsigned long		_pad3;
	unsigned short		_tbptr;
	unsigned short		_pad4;
	unsigned long		_pad5;
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
	unsigned short		status;
	unsigned short		length;
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
	unsigned short		status;
	unsigned short		_pad;
	unsigned long		length;
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
	unsigned char		dpram0[0x400];	/* Microcode program */
	unsigned char		dpram1[0x200];
	unsigned char		dpram2[0x100];	/* Microcode scratch */
	unsigned char		dpram3[0x100];	/* Not on REV A or B masks */
	unsigned char		_rsv0[0xC00-0x800];
	m360SCCENparms_t	scc1p;
	unsigned char		_rsv1[0xCB0-0xC00-sizeof(m360SCCENparms_t)];
	m360MiscParms_t		miscp;
	unsigned char		_rsv2[0xD00-0xCB0-sizeof(m360MiscParms_t)];
	m360SCCparms_t		scc2p;
	unsigned char		_rsv3[0xD80-0xD00-sizeof(m360SCCparms_t)];
	m360SPIparms_t		spip;
	unsigned char		_rsv4[0xDB0-0xD80-sizeof(m360SPIparms_t)];
	m360TimerParms_t	tmp;
	unsigned char		_rsv5[0xE00-0xDB0-sizeof(m360TimerParms_t)];
	m360SCCparms_t		scc3p;
	unsigned char		_rsv6[0xE70-0xE00-sizeof(m360SCCparms_t)];
	m360IDMAparms_t		idma1p;
	unsigned char		_rsv7[0xE80-0xE70-sizeof(m360IDMAparms_t)];
	m360SMCparms_t		smc1p;
	unsigned char		_rsv8[0xF00-0xE80-sizeof(m360SMCparms_t)];
	m360SCCparms_t		scc4p;
	unsigned char		_rsv9[0xF70-0xF00-sizeof(m360SCCparms_t)];
	m360IDMAparms_t		idma2p;
	unsigned char		_rsv10[0xF80-0xF70-sizeof(m360IDMAparms_t)];
	m360SMCparms_t		smc2p;
	unsigned char		_rsv11[0x1000-0xF80-sizeof(m360SMCparms_t)];

	/*
	 * SIM Block
	 */
	unsigned long		mcr;
	unsigned long		_pad00;
	unsigned char		avr;
	unsigned char		rsr;
	unsigned short		_pad01;
	unsigned char		clkocr;
	unsigned char		_pad02;
	unsigned short		_pad03;
	unsigned short		pllcr;
	unsigned short		_pad04;
	unsigned short		cdvcr;
	unsigned short		pepar;
	unsigned long		_pad05[2];
	unsigned short		_pad06;
	unsigned char		sypcr;
	unsigned char		swiv;
	unsigned short		_pad07;
	unsigned short		picr;
	unsigned short		_pad08;
	unsigned short		pitr;
	unsigned short		_pad09;
	unsigned char		_pad10;
	unsigned char		swsr;
	unsigned long		bkar;
	unsigned long		bcar;
	unsigned long		_pad11[2];

	/*
	 * MEMC Block
	 */
	unsigned long		gmr;
	unsigned short		mstat;
	unsigned short		_pad12;
	unsigned long		_pad13[2];
	m360MEMCRegisters_t	memc[8];
	unsigned char		_pad14[0xF0-0xD0];
	unsigned char		_pad15[0x100-0xF0];
	unsigned char		_pad16[0x500-0x100];
	
	/*
	 * IDMA1 Block
	 */
	unsigned short		iccr;
	unsigned short		_pad17;
	unsigned short		cmr1;
	unsigned short		_pad18;
	unsigned long		sapr1;
	unsigned long		dapr1;
	unsigned long		bcr1;
	unsigned char		fcr1;
	unsigned char		_pad19;
	unsigned char		cmar1;
	unsigned char		_pad20;
	unsigned char		csr1;
	unsigned char		_pad21;
	unsigned short		_pad22;
	
	/*
	 * SDMA Block
	 */
	unsigned char		sdsr;
	unsigned char		_pad23;
	unsigned short		sdcr;
	unsigned long		sdar;
	
	/*
	 * IDMA2 Block
	 */
	unsigned short		_pad24;
	unsigned short		cmr2;
	unsigned long		sapr2;
	unsigned long		dapr2;
	unsigned long		bcr2;
	unsigned char		fcr2;
	unsigned char		_pad26;
	unsigned char		cmar2;
	unsigned char		_pad27;
	unsigned char		csr2;
	unsigned char		_pad28;
	unsigned short		_pad29;
	unsigned long		_pad30;
	
	/*
	 * CPIC Block
	 */
	unsigned long		cicr;
	unsigned long		cipr;
	unsigned long		cimr;
	unsigned long		cisr;

	/*
	 * Parallel I/O Block
	 */
	unsigned short		padir;
	unsigned short		papar;
	unsigned short		paodr;
	unsigned short		padat;
	unsigned long		_pad31[2];
	unsigned short		pcdir;
	unsigned short		pcpar;
	unsigned short		pcso;
	unsigned short		pcdat;
	unsigned short		pcint;
	unsigned short		_pad32;
	unsigned long		_pad33[5];
	
	/*
	 * TIMER Block
	 */
	unsigned short		tgcr;
	unsigned short		_pad34;
	unsigned long		_pad35[3];
	unsigned short		tmr1;
	unsigned short		tmr2;
	unsigned short		trr1;
	unsigned short		trr2;
	unsigned short		tcr1;
	unsigned short		tcr2;
	unsigned short		tcn1;
	unsigned short		tcn2;
	unsigned short		tmr3;
	unsigned short		tmr4;
	unsigned short		trr3;
	unsigned short		trr4;
	unsigned short		tcr3;
	unsigned short		tcr4;
	unsigned short		tcn3;
	unsigned short		tcn4;
	unsigned short		ter1;
	unsigned short		ter2;
	unsigned short		ter3;
	unsigned short		ter4;
	unsigned long		_pad36[2];
	
	/*
	 * CP Block
	 */
	unsigned short		cr;
	unsigned short		_pad37;
	unsigned short		rccr;
	unsigned short		_pad38;
	unsigned long		_pad39[3];
	unsigned short		_pad40;
	unsigned short		rter;
	unsigned short		_pad41;
	unsigned short		rtmr;
	unsigned long		_pad42[5];

	/*
	 * BRG Block
	 */
	unsigned long		brgc1;
	unsigned long		brgc2;
	unsigned long		brgc3;
	unsigned long		brgc4;

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
	unsigned short		spmode;
	unsigned short		_pad43[2];
	unsigned char		spie;
	unsigned char		_pad44;
	unsigned short		_pad45;
	unsigned char		spim;
	unsigned char		_pad46[2];
	unsigned char		spcom;
	unsigned short		_pad47[2];

	/*
	 * PIP Block
	 */
	unsigned short		pipc;
	unsigned short		_pad48;
	unsigned short		ptpr;
	unsigned long		pbdir;
	unsigned long		pbpar;
	unsigned short		_pad49;
	unsigned short		pbodr;
	unsigned long		pbdat;
	unsigned long		_pad50[6];

	/*
	 * SI Block
	 */
	unsigned long		simode;
	unsigned char		sigmr;
	unsigned char		_pad51;
	unsigned char		sistr;
	unsigned char		sicmr;
	unsigned long		_pad52;
	unsigned long		sicr;
	unsigned short		_pad53;
	unsigned short		sirp[2];
	unsigned short		_pad54;
	unsigned long		_pad55[2];
	unsigned char		siram[256];
} m360_t;

extern volatile m360_t m360;

/*
 * definitions for the port b SPI pin bits
 */
#define M360_PB_SPI_MISO_MSK       (1<< 3)
#define M360_PB_SPI_MOSI_MSK       (1<< 2)
#define M360_PB_SPI_CLK_MSK        (1<< 1)

#endif /* _RTEMS_M68K_M68360_H */
