/*
 * Atmel AT91RM9200_USART Register definitions, used in KIT637_V6 (CSB637)
 *
 * Copyright (c) 2003 by Cogent Computer Systems
 * Written by Mike Kelly <mike@cogcomp.com>
 *
 * Modified by Fernando Nicodemos <fgnicodemos@terra.com.br>
 * from NCB - Sistemas Embarcados Ltda. (Brazil)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
*/

#ifndef __AT91RM9200_USART_H__
#define __AT91RM9200_USART_H__

#include <bits.h>

/* Register Offsets */
#define US_CR         0x00            /* Control Register */
#define US_MR         0x04            /* Mode Register */
#define US_IER        0x08            /* Interrupt Enable Register */
#define US_IDR        0x0C            /* Interrupt Disable Register */
#define US_IMR        0x10            /* Interrupt Mask Register */
#define US_SR         0x14            /* Channel Status Register */
#define US_RHR        0x18            /* Receiver Holding Register */
#define US_THR        0x1C            /* Transmitter Holding Register */
#define US_BRGR       0x20            /* Baud Rate Generator Register */
#define US_RTOR       0x24	      /* Receiver Time-out Register */
#define US_TTGR       0x28            /* Transmitter Timeguard Register */
#define US_C1R        0x40            /* Chip ID1 Register - FI DI Ratio Register */
#define US_C2R        0x44            /* Chip ID2 Register - Number of Erros Register */
#define US_FNTR       0x48            /* Force NTRST Register */
#define US_IF         0x4C            /* IrDA Filter Register  */

/* Bit Defines */
/* Control Register, US_CR, Offset 0x00 */
#define	US_CR_RSTRX    BIT2            /* 1 = Reset and disable receiver */
#define	US_CR_RSTTX    BIT3            /* 1 = Reset and disable transmitter */
#define	US_CR_RXEN     BIT4            /* 1 = Receiver enable */
#define	US_CR_RXDIS    BIT5            /* 1 = Receiver disable */
#define	US_CR_TXEN     BIT6            /* 1 = Transmitter enable */
#define	US_CR_TXDIS    BIT7            /* 1 = Transmitter disable */
#define	US_CR_RSTSTA   BIT8            /* 1 = Reset PARE, FRAME and OVRE in DBGU_SR. */
#define	US_CR_STTBRK   BIT9	       /* 1 = Start transmission of a Break */
#define	US_CR_STPBRK   BIT10	       /* 1 = Stop transmission of a Break */
#define	US_CR_STTTO    BIT11	       /* 1 = Start Time-out */
#define	US_CR_SENDA    BIT12	       /* 1 = Send Address - MDROP mode only */
#define	US_CR_RSTIT    BIT13           /* 1 = Reset Iteration */
#define	US_CR_RSTNACK  BIT14	       /* 1 = Reset Non Acknowledge */
#define	US_CR_RETTO    BIT15	       /* 1 = Restart Time-out */
#define	US_CR_DTREN    BIT16	       /* 1 = Data Terminal Ready Enable - AT91RM9200 only */
#define	US_CR_DTRDIS   BIT17	       /* 1 = Data Terminal Ready Disable - AT91RM9200 only */
#define	US_CR_RTSEN    BIT18	       /* 1 = Request To Send Enable */
#define	US_CR_RTSDIS   BIT19	       /* 1 = Request To Send Disable */


/* Mode Register. US_MR. Offset 0x04 */
#define	US_MR_USMODE		(0xF <<  0)		/* Mode of the USART */
#define	US_MR_USMODE_NORMAL		0
#define	US_MR_USMODE_RS485		1
#define	US_MR_USMODE_HWHS		2
#define	US_MR_USMODE_MODEM		3
#define	US_MR_USMODE_ISO7816_T0		4
#define	US_MR_USMODE_ISO7816_T1		6
#define	US_MR_USMODE_IRDA		8
#define	US_MR_USCLKS		(3 <<  4)		/* Clock Selection */
#define	US_MR_USCLKS_MCK	(0 <<  4)
#define	US_MR_USCLKS_MCK_DIV8	(1 <<  4)
#define	US_MR_USCLKS_SCK	(3 <<  4)
#define	US_MR_CHRL		(3 <<  6)		/* Character Length */
#define	US_MR_CHRL_5		(0 <<  6)
#define	US_MR_CHRL_6		(1 <<  6)
#define	US_MR_CHRL_7		(2 <<  6)
#define	US_MR_CHRL_8		(3 <<  6)
#define	US_MR_SYNC		(1 <<  8)		/* Synchronous Mode Select */
#define	US_MR_PAR		(7 <<  9)		/* Parity Type */
#define	US_MR_PAR_EVEN		(0 <<  9)		/* Even Parity */
#define	US_MR_PAR_ODD		(1 <<  9)		/* Odd Parity */
#define	US_MR_PAR_SPACE		(2 <<  9)		/* Parity forced to 0 (Space) */
#define	US_MR_PAR_MARK		(3 <<  9)		/* Parity forced to 1 (Mark) */
#define	US_MR_PAR_NONE		(4 <<  9)		/* No Parity */
#define	US_MR_PAR_MDROP	        (6 <<  9)		/* Multi-drop mode */
#define	US_MR_NBSTOP		(3 << 12)		/* Number of Stop Bits */
#define	US_MR_NBSTOP_1		(0 << 12)
#define	US_MR_NBSTOP_1_5	(1 << 12)
#define	US_MR_NBSTOP_2		(2 << 12)
#define	US_MR_CHMODE		(3 << 14)		/* Channel Mode */
#define	US_MR_CHMODE_NORM	(0 << 14)		/* Normal Mode */
#define	US_MR_CHMODE_AUTO	(1 << 14)		/* Auto Echo: RXD drives TXD */
#define	US_MR_CHMODE_LOC	(2 << 14)		/* Local Loopback: TXD drives RXD */
#define	US_MR_CHMODE_REM	(3 << 14)		/* Remote Loopback: RXD pin connected to TXD pin. */
#define	US_MR_MSBF		(1 << 16)		/* Bit Order */
#define	US_MR_MODE9		(1 << 17)		/* 9-bit Character Length */
#define	US_MR_CLKO		(1 << 18)		/* Clock Output Select */
#define	US_MR_OVER		(1 << 19)		/* Oversampling Mode */
#define	US_MR_INACK		(1 << 20)		/* Inhibit Non Acknowledge */
#define	US_MR_DSNACK		(1 << 21)		/* Disable Successive NACK */
#define	US_MR_MAX_ITER		(7 << 24)		/* Max Iterations */
#define	US_MR_FILTER		(1 << 28)		/* Infrared Receive Line Filter */

/* Interrupt Enable Register, US_IER, Offset 0x08 */
/* Interrupt Disable Register, US_IDR, Offset 0x0C */
/* Interrupt Mask Register, US_IMR, Offset 0x10 */
/* Channel Status Register, US_SR, Offset 0x14 */
#define	US_IER_RXRDY      BIT0	/* RXRDY Interrupt */
#define	US_IER_TXRDY      BIT1	/* TXRDY Interrupt */
#define	US_IER_RXBRK      BIT2	/* End of Receive Transfer Interrupt */
#define	US_IER_ENDRX      BIT3	/* End of Receiver Transfer */
//#define	US_IER_ENDTX      BIT4	/* End of Transmit Interrupt */
#define	US_IER_OVRE       BIT5	/* Overrun Interrupt */
#define	US_IER_FRAME      BIT6	/* Framing Error Interrupt */
#define	US_IER_PARE       BIT7	/* Parity Error */
#define	US_IER_TIMEOUT    BIT8	/* Receiver Time-out */
#define	US_IER_TXEMPTY    BIT9	/* Transmitter Empty */
#define	US_IER_ITERATION  BIT10	/* Max number of Repetitions Reached */
#define	US_IER_TXBUFE     BIT11	/* Transmission Buffer Empty */
#define	US_IER_RXBUFF     BIT12	/* Reception Buffer Full */
#define	US_IER_NACK       BIT13	/* Non Acknowledge */
#define	US_IER_RIIC       BIT16	/* Ring Indicator Input Change [AT91RM9200 only] */
#define	US_IER_DSRIC      BIT17	/* Data Set Ready Input Change [AT91RM9200 only] */
#define	US_IER_DCDIC      BIT18	/* Data Carrier Detect Input Change [AT91RM9200 only] */
#define	US_IER_CTSIC      BIT19	/* Clear to Send Input Change */
#define US_IER_ALL        0xC0001AFB  /* all assigned bits */

/* FORCE_NTRST Register, US_FNTR, Offset 0x48 */
#define US_FNTR_NTRST         BIT0    /* 1 = Force NTRST low in JTAG */

typedef struct {
    volatile uint32_t cr;
    volatile uint32_t mr;
    volatile uint32_t ier;
    volatile uint32_t idr;
    volatile uint32_t imr;
    volatile uint32_t sr;
    volatile uint32_t rhr;
    volatile uint32_t thr;
    volatile uint32_t brgr;
    volatile uint32_t _res0[7];
    volatile uint32_t cidr;
    volatile uint32_t exid;
    volatile uint32_t fnr;
} at91rm9200_usart_regs_t;

#endif /* __AT91RM9200_USART_H__ */
