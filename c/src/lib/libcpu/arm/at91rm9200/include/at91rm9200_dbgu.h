/*
 * Atmel AT91RM9200_DBGU Register definitions
 *
 * Copyright (c) 2003 by Cogent Computer Systems
 * Written by Mike Kelly <mike@cogcomp.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef __AT91RM9200_DBGU_H__
#define __AT91RM9200_DBGU_H__

#include "bits.h"

/* Register Offsets */
#define DBGU_CR         0x00            /* Control Register */
#define DBGU_MR         0x04            /* Mode Register */
#define DBGU_IER        0x08            /* Interrupt Enable Register */
#define DBGU_IDR        0x0C            /* Interrupt Disable Register */
#define DBGU_IMR        0x10            /* Interrupt Mask Register */
#define DBGU_SR         0x14            /* Channel Status Register */
#define DBGU_RHR        0x18            /* Receiver Holding Register */
#define DBGU_THR        0x1C            /* Transmitter Holding Register */
#define DBGU_BRGR       0x20            /* Baud Rate Generator Register */
#define DBGU_C1R        0x40            /* Chip ID1 Register */
#define DBGU_C2R        0x44            /* Chip ID2 Register */
#define DBGU_FNTR       0x48            /* Force NTRST Register */

/* Bit Defines */
/* Control Register, DBGU_CR, Offset 0x00 */
#define DBGU_CR_RSTRX   BIT2            /* 1 = Reset and disable receiver */
#define DBGU_CR_RSTTX   BIT3            /* 1 = Reset and disable transmitter */
#define DBGU_CR_RXEN    BIT4            /* 1 = Receiver enable */
#define DBGU_CR_RXDIS   BIT5            /* 1 = Receiver disable */
#define DBGU_CR_TXEN    BIT6            /* 1 = Transmitter enable */
#define DBGU_CR_TXDIS   BIT7            /* 1 = Transmitter disable */
#define DBGU_CR_RSTSTA  BIT8            /* 1 = Reset PARE, FRAME and OVRE in DBGU_SR. */

/* Mode Register. DBGU_MR. Offset 0x04 */
#define DBGU_MR_PAR_EVEN    (0x0 <<  9) /* Even Parity */
#define DBGU_MR_PAR_ODD     (0x1 <<  9) /* Odd Parity */
#define DBGU_MR_PAR_SPACE   (0x2 <<  9) /* Parity forced to 0 (Space) */
#define DBGU_MR_PAR_MARK    (0x3 <<  9) /* Parity forced to 1 (Mark) */
#define DBGU_MR_PAR_NONE    (0x4 <<  9) /* No Parity */
#define DBGU_MR_PAR_MDROP   (0x6 <<  9) /* Multi-drop mode */
#define DBGU_MR_CHMODE_NORM (0x0 << 14) /* Normal Mode */
#define DBGU_MR_CHMODE_AUTO (0x1 << 14) /* Auto Echo: RXD drives TXD */
#define DBGU_MR_CHMODE_LOC  (0x2 << 14) /* Local Loopback: TXD drives RXD */
#define DBGU_MR_CHMODE_REM  (0x3 << 14) /* Remote Loopback: RXD pin connected to TXD pin. */

/* Interrupt Enable Register, DBGU_IER, Offset 0x08 */
/* Interrupt Disable Register, DBGU_IDR, Offset 0x0C */
/* Interrupt Mask Register, DBGU_IMR, Offset 0x10 */
/* Channel Status Register, DBGU_SR, Offset 0x14 */
#define DBGU_INT_RXRDY      BIT0        /* RXRDY Interrupt */
#define DBGU_INT_TXRDY      BIT1        /* TXRDY Interrupt */
#define DBGU_INT_ENDRX      BIT3        /* End of Receive Transfer Interrupt */
/*efine DBGU_INT_ENDTX      BIT4        /* End of Transmit Interrupt */
#define DBGU_INT_OVRE       BIT5        /* Overrun Interrupt */
#define DBGU_INT_FRAME      BIT6        /* Framing Error Interrupt */
#define DBGU_INT_PARE       BIT7        /* Parity Error Interrupt */
#define DBGU_INT_TXEMPTY    BIT9        /* TXEMPTY Interrupt */
#define DBGU_INT_TXBUFE     BIT11       /* TXBUFE Interrupt */
#define DBGU_INT_RXBUFF     BIT12       /* RXBUFF Interrupt */
#define DBGU_INT_COMM_TX    BIT30       /* COMM_TX Interrupt */
#define DBGU_INT_COMM_RX    BIT31       /* COMM_RX Interrupt */
#define DBGU_INT_ALL        0xC0001AFB  /* all assigned bits */

/* FORCE_NTRST Register, DBGU_FNTR, Offset 0x48 */
#define DBGU_FNTR_NTRST         BIT0    /* 1 = Force NTRST low in JTAG */

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
} at91rm9200_dbgu_regs_t;

#endif /* __AT91RM9200_DBGU_H__ */
