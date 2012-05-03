/*
 *------------------------------------------------------------------
 *
 *	m68302.h - Definitions for Motorola MC68302 processor.
 *
 * Section references in this file refer to revision 2 of Motorola's
 * "MC68302 Integrated Multiprotocol Processor User's Manual".
 * (Motorola document MC68302UM/AD REV 2.)
 *
 * Based on Don Meyer's cpu68302.h that was posted in comp.sys.m68k
 * on 17 February, 1993.
 *
 * COPYRIGHT 1995 David W. Glessner.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the following conditions are met:
 * 1. Redistribution of source code and documentation must retain
 *    the above copyright notice, this list of conditions and the
 *    following disclaimer.
 * 2. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * This software is provided "AS IS" without warranty of any kind,
 * either expressed or implied, including, but not limited to, the
 * implied warranties of merchantability, title and fitness for a
 * particular purpose.
 *
 *------------------------------------------------------------------
 */

#ifndef _RTEMS_M68K_M68302_H
#define _RTEMS_M68K_M68302_H

/*
 * BAR - Base Address Register
 * Section 2.7
 */
#define M302_BAR	(*((volatile uint16_t   *) 0xf2))

/*
 * SCR - System Control Register
 * Section 3.8.1
 */
#define M302_SCR	(*((volatile uint32_t   *) 0xf4))
/*
 * SCR bits
 */
#define RBIT_SCR_IPA		0x08000000
#define RBIT_SCR_HWT		0x04000000
#define RBIT_SCR_WPV		0x02000000
#define RBIT_SCR_ADC		0x01000000

#define RBIT_SCR_ERRE		0x00400000
#define RBIT_SCR_VGE		0x00200000
#define RBIT_SCR_WPVE		0x00100000
#define RBIT_SCR_RMCST		0x00080000
#define RBIT_SCR_EMWS		0x00040000
#define RBIT_SCR_ADCE		0x00020000
#define RBIT_SCR_BCLM		0x00010000

#define RBIT_SCR_FRZW		0x00008000
#define RBIT_SCR_FRZ2		0x00004000
#define RBIT_SCR_FRZ1		0x00002000
#define RBIT_SCR_SAM		0x00001000
#define RBIT_SCR_HWDEN		0x00000800
#define RBIT_SCR_HWDCN2		0x00000400
#define RBIT_SCR_HWDCN1		0x00000200  /* 512 clocks */
#define RBIT_SCR_HWDCN0		0x00000100  /* 128 clocks */

#define RBIT_SCR_LPREC		0x00000080
#define RBIT_SCR_LPP16		0x00000040
#define RBIT_SCR_LPEN		0x00000020
#define RBIT_SCR_LPCLKDIV	0x0000001f


/*
 * 68000 interrupt and trap vector numbers
 */
#define M68K_IVEC_BUS_ERROR		 2
#define M68K_IVEC_ADDRESS_ERROR		 3
#define M68K_IVEC_ILLEGAL_OPCODE	 4
#define M68K_IVEC_ZERO_DIVIDE		 5
#define M68K_IVEC_CHK			 6
#define M68K_IVEC_TRAPV			 7
#define M68K_IVEC_PRIVILEGE		 8
#define M68K_IVEC_TRACE			 9
#define M68K_IVEC_LINE_A		10
#define M68K_IVEC_LINE_F		11
/*	Unassigned, Reserved		12-14 */
#define M68K_IVEC_UNINITIALIZED_INT	15
/*	Unassigned, Reserved		16-23 */
#define M68K_IVEC_SPURIOUS_INT		24

#define M68K_IVEC_LEVEL1_AUTOVECTOR	25
#define M68K_IVEC_LEVEL2_AUTOVECTOR	26
#define M68K_IVEC_LEVEL3_AUTOVECTOR	27
#define M68K_IVEC_LEVEL4_AUTOVECTOR	28
#define M68K_IVEC_LEVEL5_AUTOVECTOR	29
#define M68K_IVEC_LEVEL6_AUTOVECTOR	30
#define M68K_IVEC_LEVEL7_AUTOVECTOR	31

#define M68K_IVEC_TRAP0			32
#define M68K_IVEC_TRAP1			33
#define M68K_IVEC_TRAP2			34
#define M68K_IVEC_TRAP3			35
#define M68K_IVEC_TRAP4			36
#define M68K_IVEC_TRAP5			37
#define M68K_IVEC_TRAP6			38
#define M68K_IVEC_TRAP7			39
#define M68K_IVEC_TRAP8			40
#define M68K_IVEC_TRAP9			41
#define M68K_IVEC_TRAP10		42
#define M68K_IVEC_TRAP11		43
#define M68K_IVEC_TRAP12		44
#define M68K_IVEC_TRAP13		45
#define M68K_IVEC_TRAP14		46
#define M68K_IVEC_TRAP15		47
/*
 *	Unassigned, Reserved		48-59
 *
 * Note: Vectors 60-63 are used by the MC68302 (e.g. BAR, SCR).
 */

/*
 * MC68302 Interrupt Vectors
 * Section 3.2
 */
enum m68302_ivec_e {
    M302_IVEC_ERR	=0,
    M302_IVEC_PB8	=1,	/* General-Purpose Interrupt 0 */
    M302_IVEC_SMC2	=2,
    M302_IVEC_SMC1	=3,
    M302_IVEC_TIMER3	=4,
    M302_IVEC_SCP	=5,
    M302_IVEC_TIMER2	=6,
    M302_IVEC_PB9	=7,	/* General-Purpose Interrupt 1 */
    M302_IVEC_SCC3	=8,
    M302_IVEC_TIMER1	=9,
    M302_IVEC_SCC2	=10,
    M302_IVEC_IDMA	=11,
    M302_IVEC_SDMA	=12,	/* SDMA Channels Bus Error */
    M302_IVEC_SCC1	=13,
    M302_IVEC_PB10	=14,	/* General-Purpose Interrupt 2 */
    M302_IVEC_PB11	=15,	/* General-Purpose Interrupt 3 */
    M302_IVEC_IRQ1	=17,	/* External Device */
    M302_IVEC_IRQ6	=22,	/* External Device */
    M302_IVEC_IRQ7	=23	/* External Device */
};


/*
 * GIMR - Global Interrupt Mode Register
 * Section 3.2.5.1
 */
#define RBIT_GIMR_MOD		(1<<15)
#define RBIT_GIMR_IV7		(1<<14)
#define RBIT_GIMR_IV6		(1<<13)
#define RBIT_GIMR_IV1		(1<<12)
#define RBIT_GIMR_ET7		(1<<10)
#define RBIT_GIMR_ET6		(1<<9)
#define RBIT_GIMR_ET1		(1<<8)
#define RBIT_GIMR_VECTOR	(7<<5)

/*
 * IPR - Interrupt Pending Register    (Section 3.2.5.2)
 * IMR - Interrupt Mask Register       (Section 3.2.5.3)
 * ISR - Interrupt In-Service Register (Section 3.2.5.4)
 */
#define RBIT_IPR_PB11		(1<<15)
#define RBIT_IPR_PB10		(1<<14)
#define RBIT_IPR_SCC1		(1<<13)
#define RBIT_IPR_SDMA		(1<<12)
#define RBIT_IPR_IDMA		(1<<11)
#define RBIT_IPR_SCC2		(1<<10)
#define RBIT_IPR_TIMER1		(1<<9)
#define RBIT_IPR_SCC3		(1<<8)
#define RBIT_IPR_PB9		(1<<7)
#define RBIT_IPR_TIMER2		(1<<6)
#define RBIT_IPR_SCP		(1<<5)
#define RBIT_IPR_TIMER3		(1<<4)
#define RBIT_IPR_SMC1		(1<<3)
#define RBIT_IPR_SMC2		(1<<2)
#define RBIT_IPR_PB8		(1<<1)
#define RBIT_IPR_ERR		(1<<0)

#define RBIT_ISR_PB11		(1<<15)
#define RBIT_ISR_PB10		(1<<14)
#define RBIT_ISR_SCC1		(1<<13)
#define RBIT_ISR_SDMA		(1<<12)
#define RBIT_ISR_IDMA		(1<<11)
#define RBIT_ISR_SCC2		(1<<10)
#define RBIT_ISR_TIMER1		(1<<9)
#define RBIT_ISR_SCC3		(1<<8)
#define RBIT_ISR_PB9		(1<<7)
#define RBIT_ISR_TIMER2		(1<<6)
#define RBIT_ISR_SCP		(1<<5)
#define RBIT_ISR_TIMER3		(1<<4)
#define RBIT_ISR_SMC1		(1<<3)
#define RBIT_ISR_SMC2		(1<<2)
#define RBIT_ISR_PB8		(1<<1)

#define RBIT_IMR_PB11		(1<<15)		/* PB11   Interrupt Mask */
#define RBIT_IMR_PB10		(1<<14)		/* PB10   Interrupt Mask */
#define RBIT_IMR_SCC1		(1<<13)		/* SCC1   Interrupt Mask */
#define RBIT_IMR_SDMA		(1<<12)		/* SDMA   Interrupt Mask */
#define RBIT_IMR_IDMA		(1<<11)		/* IDMA   Interrupt Mask */
#define RBIT_IMR_SCC2		(1<<10)		/* SCC2   Interrupt Mask */
#define RBIT_IMR_TIMER1		(1<<9)		/* TIMER1 Interrupt Mask */
#define RBIT_IMR_SCC3		(1<<8)		/* SCC3   Interrupt Mask */
#define RBIT_IMR_PB9		(1<<7)		/* PB9    Interrupt Mask */
#define RBIT_IMR_TIMER2		(1<<6)		/* TIMER2 Interrupt Mask */
#define RBIT_IMR_SCP		(1<<5)		/* SCP    Interrupt Mask */
#define RBIT_IMR_TIMER3		(1<<4)		/* TIMER3 Interrupt Mask */
#define RBIT_IMR_SMC1		(1<<3)		/* SMC1   Interrupt Mask */
#define RBIT_IMR_SMC2		(1<<2)		/* SMC2   Interrupt Mask */
#define RBIT_IMR_PB8		(1<<1)		/* PB8    Interrupt Mask */


/*
 * DRAM Refresh
 * Section 3.9
 *
 * The DRAM refresh memory map replaces the SCC2 Tx BD 6 and Tx BD 7
 * structures in the parameter RAM.
 *
 * Access to the DRAM registers can be accomplished by
 * the following approach:
 *
 *	volatile m302_DRAM_refresh_t *dram;
 *	dram = (volatile m302_DRAM_refresh_t *) &m302.scc2.bd.tx[6];
 *
 * Then simply use pointer references (e.g. dram->count = 3).
 */
typedef struct {
    uint16_t  	dram_high;	/* DRAM high address and FC */
    uint16_t  	dram_low;	/* DRAM low address */
    uint16_t  	increment;	/* increment step (bytes/row) */
    uint16_t  	count;		/* RAM refresh cycle count (#rows) */
    uint16_t  	t_ptr_h;	/* temporary refresh high addr & FC */
    uint16_t  	t_ptr_l;	/* temporary refresh low address */
    uint16_t  	t_count;	/* temporary refresh cycles count */
    uint16_t  	res;		/* reserved */
} m302_DRAM_refresh_t;


/*
 * TMR - Timer Mode Register (for timers 1 and 2)
 * Section 3.5.2.1
 */
#define RBIT_TMR_ICLK_STOP	(0<<1)
#define RBIT_TMR_ICLK_MASTER	(1<<1)
#define RBIT_TMR_ICLK_MASTER16	(2<<1)
#define RBIT_TMR_ICLK_TIN	(3<<1)

#define RBIT_TMR_OM		(1<<5)
#define RBIT_TMR_ORI		(1<<4)
#define RBIT_TMR_FRR		(1<<3)
#define RBIT_TMR_RST		(1<<0)


/*
 * TER - Timer Event Register (for timers 1 and 2)
 * Section 3.5.2.5
 */
#define RBIT_TER_REF	(1<<1)		/* Output Reference Event */
#define RBIT_TER_CAP	(1<<0)		/* Capture Event */


/*
 * SCC Buffer Descriptors and Buffer Descriptors Table
 * Section 4.5.5
 */
typedef struct m302_SCC_bd {
    uint16_t   status;			/* status and control */
    uint16_t   length;			/* data length */
    volatile uint8_t    *buffer;		/* data buffer pointer */
} m302_SCC_bd_t;

typedef struct {
    m302_SCC_bd_t	rx[8];		/* receive buffer descriptors */
    m302_SCC_bd_t	tx[8];		/* transmit buffer descriptors */
} m302_SCC_bd_table_t;


/*
 * SCC Parameter RAM (offset 0x080 from an SCC Base)
 * Section 4.5.6
 *
 * Each SCC parameter RAM area begins at offset 0x80 from each SCC base
 * area (0x400, 0x500, or 0x600 from the dual-port RAM base).
 *
 * Offsets 0x9c-0xbf from each SCC base area compose the protocol-specific
 * portion of the SCC parameter RAM.
 */
typedef struct {
    uint8_t  	rfcr;		/* Rx Function Code */
    uint8_t  	tfcr;		/* Tx Function Code */
    uint16_t  	mrblr;		/* Maximum Rx Buffer Length */
    uint16_t  	_rstate;	/* Rx Internal State */
    uint8_t  	res2;
    uint8_t  	rbd;		/* Rx Internal Buffer Number */
    uint32_t  	_rdptr;		/* Rx Internal Data Pointer */
    uint16_t  	_rcount;	/* Rx Internal Byte Count */
    uint16_t  	_rtmp;		/* Rx Temp */
    uint16_t  	_tstate;	/* Tx Internal State */
    uint8_t  	res7;
    uint8_t  	tbd;		/* Tx Internal Buffer Number */
    uint32_t  	_tdptr;		/* Tx Internal Data Pointer */
    uint16_t  	_tcount;	/* Tx Internal Byte Count */
    uint16_t  	_ttmp;		/* Tx Temp */
} m302_SCC_parameters_t;

/*
 * UART-Specific SCC Parameter RAM
 * Section 4.5.11.3
 */
typedef struct {
    uint16_t  	max_idl;	/* Maximum IDLE Characters (rx) */
    uint16_t  	idlc;		/* Temporary rx IDLE counter */
    uint16_t  	brkcr;		/* Break Count Register (tx) */
    uint16_t  	parec;		/* Receive Parity Error Counter */
    uint16_t  	frmec;		/* Receive Framing Error Counter */
    uint16_t  	nosec;		/* Receive Noise Counter */
    uint16_t  	brkec;		/* Receive Break Condition Counter */
    uint16_t  	uaddr1;		/* UART ADDRESS Character 1 */
    uint16_t  	uaddr2;		/* UART ADDRESS Character 2 */
    uint16_t  	rccr;		/* Receive Control Character Register */
    uint16_t  	character[8];	/* Control Characters 1 through 8*/
} m302_SCC_UartSpecific_t;
/*
 *  This definition allows for the checking of receive buffers
 *  for errors.
 */

#define RCV_ERR    0x003F

/*
 * UART receive buffer descriptor bit definitions.
 * Section 4.5.11.14
 */
#define RBIT_UART_CTRL   (1<<11)	/* buffer contains a control char */
#define RBIT_UART_ADDR   (1<<10)	/* first byte contains an address */
#define RBIT_UART_MATCH  (1<<9)		/* indicates which addr char matched */
#define RBIT_UART_IDLE   (1<<8)		/* buffer closed due to IDLE sequence */
#define RBIT_UART_BR     (1<<5)		/* break sequence was received */
#define RBIT_UART_FR     (1<<4)		/* framing error was received */
#define RBIT_UART_PR     (1<<3)		/* parity error was received */
#define RBIT_UART_OV     (1<<1)		/* receiver overrun occurred */
#define RBIT_UART_CD     (1<<0)		/* carrier detect lost */
#define RBIT_UART_STATUS 0x003B		/* all status bits */

/*
 * UART transmit buffer descriptor bit definitions.
 * Section 4.5.11.15
 */
#define RBIT_UART_CR       (1<<11)	/* clear-to-send report
					 * this results in two idle bits
					 * between back-to-back frames
					 */
#define RBIT_UART_A        (1<<10)	/* buffer contains address characters
					 * only valid in multidrop mode (UM0=1)
					 */
#define RBIT_UART_PREAMBLE (1<<9)	/* send preamble before data */
#define RBIT_UART_CTS_LOST (1<<0)	/* CTS lost */

/*
 * UART event register
 * Section 4.5.11.16
 */
#define M302_UART_EV_CTS   (1<<7)	/* CTS status changed */
#define M302_UART_EV_CD    (1<<6)	/* carrier detect status changed */
#define M302_UART_EV_IDL   (1<<5)	/* IDLE sequence status changed */
#define M302_UART_EV_BRK   (1<<4)	/* break character was received */
#define M302_UART_EV_CCR   (1<<3)	/* control character received */
#define M302_UART_EV_TX    (1<<1)	/* buffer has been transmitted */
#define M302_UART_EV_RX    (1<<0)	/* buffer has been received */


/*
 * HDLC-Specific SCC Parameter RAM
 * Section 4.5.12.3
 *
 * c_mask_l should be 0xF0B8 for 16-bit CRC, 0xdebb for 32-bit CRC
 * c_mask_h is a don't care  for 16-bit CRC, 0x20E2 for 32-bit CRC
 */
typedef struct {
    uint16_t  	rcrc_l;		/* Temp Receive CRC Low */
    uint16_t  	rcrc_h;		/* Temp Receive CRC High */
    uint16_t  	c_mask_l;	/* CRC Mask Low */
    uint16_t  	c_mask_h;	/* CRC Mask High */
    uint16_t  	tcrc_l;		/* Temp Transmit CRC Low */
    uint16_t  	tcrc_h;		/* Temp Transmit CRC High */

    uint16_t  	disfc;		/* Discard Frame Counter */
    uint16_t  	crcec;		/* CRC Error Counter */
    uint16_t  	abtsc;		/* Abort Sequence Counter */
    uint16_t  	nmarc;		/* Nonmatching Address Received Cntr */
    uint16_t  	retrc;		/* Frame Retransmission Counter */

    uint16_t  	mflr;		/* Maximum Frame Length Register */
    uint16_t  	max_cnt;	/* Maximum_Length Counter */

    uint16_t  	hmask;		/* User Defined Frame Address Mask */
    uint16_t  	haddr1;		/* User Defined Frame Address */
    uint16_t  	haddr2;		/* " */
    uint16_t  	haddr3;		/* " */
    uint16_t  	haddr4;		/* " */
} m302_SCC_HdlcSpecific_t;
/*
 * HDLC receiver buffer descriptor bit definitions
 * Section 4.5.12.10
 */
#define RBIT_HDLC_EMPTY_BIT  0x8000	/* buffer associated with BD is empty */
#define RBIT_HDLC_LAST_BIT   0x0800	/* buffer is last in a frame */
#define RBIT_HDLC_FIRST_BIT  0x0400	/* buffer is first in a frame */
#define RBIT_HDLC_FRAME_LEN  0x0020	/* receiver frame length violation */
#define RBIT_HDLC_NONOCT_Rx  0x0010	/* received non-octet aligned frame */
#define RBIT_HDLC_ABORT_SEQ  0x0008	/* received abort sequence */
#define RBIT_HDLC_CRC_ERROR  0x0004	/* frame contains a CRC error */
#define RBIT_HDLC_OVERRUN    0x0002	/* receiver overrun occurred */
#define RBIT_HDLC_CD_LOST    0x0001	/* carrier detect lost */

/*
 * HDLC transmit buffer descriptor bit definitions
 * Section 4.5.12.11
 */
#define RBIT_HDLC_READY_BIT  0x8000	/* buffer is ready to transmit */
#define RBIT_HDLC_EXT_BUFFER 0x4000	/* buffer is in external memory */
#define RBIT_HDLC_WRAP_BIT   0x2000	/* last buffer in bd table, so wrap */
#define RBIT_HDLC_WAKE_UP    0x1000	/* interrupt when buffer serviced */
#define RBIT_HDLC_LAST_BIT   0x0800	/* buffer is last in the frame */
#define RBIT_HDLC_TxCRC_BIT  0x0400	/* transmit a CRC sequence */
#define RBIT_HDLC_UNDERRUN   0x0002	/* transmitter underrun */
#define RBIT_HDLC_CTS_LOST   0x0001	/* CTS lost */

/*
 * HDLC event register bit definitions
 * Section 4.5.12.12
 */
#define RBIT_HDLC_CTS   0x80		/* CTS status changed */
#define RBIT_HDLC_CD    0x40		/* carrier detect status changed */
#define RBIT_HDLC_IDL   0x20		/* IDLE sequence status changed */
#define RBIT_HDLC_TXE   0x10		/* transmit error */
#define RBIT_HDLC_RXF   0x08		/* received frame */
#define RBIT_HDLC_BSY  0x04		/* frame rcvd and discarded due to
					 * lack of buffers
					 */
#define RBIT_HDLC_TXB    0x02		/* buffer has been transmitted */
#define RBIT_HDLC_RXB    0x01		/* received buffer */



typedef struct {
    m302_SCC_bd_table_t          bd;	/* +000 Buffer Descriptor Table */
    m302_SCC_parameters_t        parm;	/* +080 Common Parameter RAM */
    union {				/* +09C Protocol-Specific Parm RAM */
	m302_SCC_UartSpecific_t  uart;
	m302_SCC_HdlcSpecific_t  hdlc;
    } prot;
    uint8_t  	   res[0x040];	/* +0C0 reserved, (not implemented) */
} m302_SCC_t;


/*
 * Common SCC Registers
 */
typedef struct {
    uint16_t  	res1;
    uint16_t  	scon;	/* SCC Configuration Register        4.5.2 */
    uint16_t  	scm;	/* SCC Mode Register                 4.5.3 */
    uint16_t  	dsr;	/* SCC Data Synchronization Register 4.5.4 */
    uint8_t  	scce;	/* SCC Event Register  4.5.8.1 */
    uint8_t  	res2;
    uint8_t  	sccm;	/* SCC Mask Register   4.5.8.2 */
    uint8_t  	res3;
    uint8_t  	sccs;	/* SCC Status Register 4.5.8.3 */
    uint8_t  	res4;
    uint16_t  	res5;
} m302_SCC_Registers_t;

/*
 * SCON - SCC Configuration Register
 * Section 4.5.2
 */
#define RBIT_SCON_WOMS	(1<<15)		/* Wired-OR Mode Select (NMSI mode only)
					 * When set, the TXD driver is an
					 * open-drain output */
#define RBIT_SCON_EXTC	(1<<14)		/* External Clock Source */
#define RBIT_SCON_TCS	(1<<13)		/* Transmit Clock Source */
#define RBIT_SCON_RCS	(1<<12)		/* Receive Clock Source */

/*
 * SCM - SCC Mode Register bit definitions
 * Section 4.5.3
 * The parameter-specific mode bits occupy bits 15 through 6.
 */
#define RBIT_SCM_ENR	(1<<3)		/* Enable receiver */
#define RBIT_SCM_ENT	(1<<2)		/* Enable transmitter */


/*
 * Internal MC68302 Registers
 * starts at offset 0x800 from dual-port RAM base
 * Section 2.8
 */
typedef struct {
    /* offset +800 */
    uint16_t  	res0;
    uint16_t  	cmr;		/* IDMA Channel Mode Register */
    uint32_t  	sapr;		/* IDMA Source Address Pointer */
    uint32_t  	dapr;		/* IDMA Destination Address Pointer */
    uint16_t  	bcr;		/* IDMA Byte Count Register */
    uint8_t  	csr;		/* IDMA Channel Status Register */
    uint8_t  	res1;
    uint8_t  	fcr;		/* IDMA Function Code Register */
    uint8_t  	res2;

    /* offset +812 */
    uint16_t  	gimr;		/* Global Interrupt Mode Register */
    uint16_t  	ipr;		/* Interrupt Pending Register */
    uint16_t  	imr;		/* Interrupt Mask Register */
    uint16_t  	isr;		/* Interrupt In-Service Register */
    uint16_t  	res3;
    uint16_t  	res4;

    /* offset +81e */
    uint16_t  	pacnt;		/* Port A Control Register */
    uint16_t  	paddr;		/* Port A Data Direction Register */
    uint16_t  	padat;		/* Port A Data Register */
    uint16_t  	pbcnt;		/* Port B Control Register */
    uint16_t  	pbddr;		/* Port B Data Direction Register */
    uint16_t  	pbdat;		/* Port B Data Register */
    uint16_t  	res5;

    /* offset +82c */
    uint16_t  	res6;
    uint16_t  	res7;

    uint16_t  	br0;		/* Base Register   (CS0) */
    uint16_t  	or0;		/* Option Register (CS0) */
    uint16_t  	br1;		/* Base Register   (CS1) */
    uint16_t  	or1;		/* Option Register (CS1) */
    uint16_t  	br2;		/* Base Register   (CS2) */
    uint16_t  	or2;		/* Option Register (CS2) */
    uint16_t  	br3;		/* Base Register   (CS3) */
    uint16_t  	or3;		/* Option Register (CS3) */

    /* offset +840 */
    uint16_t  	tmr1;		/* Timer Unit 1 Mode Register */
    uint16_t  	trr1;		/* Timer Unit 1 Reference Register */
    uint16_t  	tcr1;		/* Timer Unit 1 Capture Register */
    uint16_t  	tcn1;		/* Timer Unit 1 Counter */
    uint8_t  	res8;
    uint8_t  	ter1;		/* Timer Unit 1 Event Register */
    uint16_t  	wrr;		/* Watchdog Reference Register */
    uint16_t  	wcn;		/* Watchdog Counter */
    uint16_t  	res9;
    uint16_t  	tmr2;		/* Timer Unit 2 Mode Register */
    uint16_t  	trr2;		/* Timer Unit 2 Reference Register */
    uint16_t  	tcr2;		/* Timer Unit 2 Capture Register */
    uint16_t  	tcn2;		/* Timer Unit 2 Counter */
    uint8_t  	resa;
    uint8_t  	ter2;		/* Timer Unit 2 Event Register */
    uint16_t  	resb;
    uint16_t  	resc;
    uint16_t  	resd;

    /* offset +860 */
    uint8_t  	cr;		/* Command Register */
    uint8_t  	rese[0x1f];

    /* offset +880, +890, +8a0 */
    m302_SCC_Registers_t scc[3];	/* SCC1, SCC2, SCC3 Registers */

    /* offset +8b0 */
    uint16_t  	spmode;		/* SCP,SMC Mode and Clock Cntrl Reg */
    uint16_t  	simask;		/* Serial Interface Mask Register */
    uint16_t  	simode;		/* Serial Interface Mode Register */
} m302_internalReg_t ;


/*
 * MC68302 dual-port RAM structure.
 * (Includes System RAM, Parameter RAM, and Internal Registers).
 * Section 2.8
 */
typedef struct {
    uint8_t  	mem[0x240];	/* +000 User Data Memory */
    uint8_t  	res1[0x1c0];	/* +240 reserved, (not implemented) */
    m302_SCC_t		scc1;		/* +400 SCC1 */
    m302_SCC_t		scc2;		/* +500 SCC2 */
    m302_SCC_t		scc3;		/* +600 SCC3 */
    uint8_t  	res2[0x100];	/* +700 reserved, (not implemented) */
    m302_internalReg_t	reg;		/* +800 68302 Internal Registers */
} m302_dualPortRAM_t;

/* some useful defines the some of the registers above */


/* ----
   MC68302 Chip Select Registers
      p3-46 2nd Edition

 */
#define BR_ENABLED     1
#define BR_DISABLED    0
#define BR_FC_NULL     0
#define BR_READ_ONLY   0
#define BR_READ_WRITE  2
#define OR_DTACK_0     0x0000
#define OR_DTACK_1     0x2000
#define OR_DTACK_2     0x4000
#define OR_DTACK_3     0x6000
#define OR_DTACK_4     0x8000
#define OR_DTACK_5     0xA000
#define OR_DTACK_6     0xC000
#define OR_DTACK_EXT   0xE000
#define OR_SIZE_64K    0x1FE0
#define OR_SIZE_128K   0x1FC0
#define OR_SIZE_256K   0x1F80
#define OR_SIZE_512K   0x1F00
#define OR_SIZE_1M     0x1E00
#define OR_SIZE_2M     0x1C00
#define OR_MASK_RW     0x0000
#define OR_NO_MASK_RW  0x0002
#define OR_MASK_FC     0x0000
#define OR_NO_MASK_FC  0x0001

#define MAKE_BR(base_address, enable, rw, fc) \
    ((base_address >> 11) | fc | rw | enable)

#define MAKE_OR(bsize, DtAck, RW_Mask, FC_Mask) \
    (DtAck | ((~(bsize - 1) & 0x00FFFFFF) >> 11) | FC_Mask | RW_Mask)

#define __REG_CAT(r, n) r ## n
#define WRITE_BR(csel, base_address, enable, rw, fc) \
          __REG_CAT(m302.reg.br, csel) = MAKE_BR(base_address, enable, rw, fc)
#define WRITE_OR(csel, bsize, DtAck, RW_Mask, FC_Mask) \
          __REG_CAT(m302.reg.or, csel) = MAKE_OR(bsize, DtAck, RW_Mask, FC_Mask)

/* ----
   MC68302 Watchdog Timer Enable Bit

 */
#define WATCHDOG_ENABLE    (1)
#define WATCHDOG_TRIGGER() (m302.reg.wrr = 0x10 | WATCHDOG_ENABLE, m302.reg.wcn = 0)
#define WATCHDOG_TOGGLE()  (m302.reg.wcn = WATCHDOG_TIMEOUT_PERIOD)
#define DISABLE_WATCHDOG() (m302.reg.wrr = 0)

/*
 * Declare the variable that's used to reference the variables in
 * the dual-port RAM.
 */
extern volatile m302_dualPortRAM_t m302;

#endif
