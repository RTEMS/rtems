/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_ATSAM_SC16IS752_H
#define LIBBSP_ARM_ATSAM_SC16IS752_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* General register set */
#define SC16IS752_RHR 0x0
#define SC16IS752_THR 0x0
#define SC16IS752_IER 0x1
#define SC16IS752_FCR 0x2
#define SC16IS752_IIR 0x2
#define SC16IS752_LCR 0x3
#define SC16IS752_MCR 0x4
#define SC16IS752_LSR 0x5
#define SC16IS752_MSR 0x6
#define SC16IS752_TCR 0x6
#define SC16IS752_SPR 0x7
#define SC16IS752_TLR 0x7
#define SC16IS752_TXLVL 0x8
#define SC16IS752_RXLVL 0x9
#define SC16IS752_IODIR 0xA
#define SC16IS752_IOSTATE 0xB
#define SC16IS752_IOINTENA 0xC
#define SC16IS752_IOCONTROL 0xE
#define SC16IS752_EFCR 0xF

/* Special register set */
#define SC16IS752_DLL 0x0
#define SC16IS752_DLH 0x1

/* Enhanced register set */
#define SC16IS752_EFR 0x2
#define SC16IS752_XON1 0x4
#define SC16IS752_XON2 0x5
#define SC16IS752_XOFF1 0x6
#define SC16IS752_XOFF2 0x7

/* FCR */
#define FCR_FIFO_EN        0x01
#define FCR_RX_FIFO_RST    0x02
#define FCR_TX_FIFO_RST    0x04
#define FCR_TX_FIFO_TRG_8  0x00
#define FCR_TX_FIFO_TRG_16 0x10
#define FCR_TX_FIFO_TRG_32 0x20
#define FCR_TX_FIFO_TRG_56 0x30
#define FCR_RX_FIFO_TRG_8  0x00
#define FCR_RX_FIFO_TRG_16 0x40
#define FCR_RX_FIFO_TRG_56 0x80
#define FCR_RX_FIFO_TRG_60 0xc0

/* EFCR */
#define EFCR_RS485_ENABLE (1u << 0)
#define EFCR_RX_DISABLE (1u << 1)
#define EFCR_TX_DISABLE (1u << 2)

/* IER */
#define IER_RHR (1u << 0)
#define IER_THR (1u << 1)
#define IER_RECEIVE_LINE_STATUS (1u << 2)
#define IER_MODEM_STATUS (1u << 3)
#define IER_SLEEP_MODE (1u << 4)
#define IER_XOFF (1u << 5)
#define IER_RTS (1u << 6)
#define IER_CTS (1u << 7)

/* IIR */
#define IIR_TX_INTERRUPT (1u << 1)
#define IIR_RX_INTERRUPT (1u << 2)

/* LCR */
#define LCR_CHRL_5_BIT (0u << 1) | (0u << 0)
#define LCR_CHRL_6_BIT (0u << 1) | (1u << 0)
#define LCR_CHRL_7_BIT (1u << 1) | (0u << 0)
#define LCR_CHRL_8_BIT (1u << 1) | (1u << 0)
#define LCR_2_STOP_BIT (1u << 2)
#define LCR_SET_PARITY (1u << 3)
#define LCR_EVEN_PARITY (1u << 4)
#define LCR_ENABLE_DIVISOR (1u << 7)

/* LSR */
#define LSR_TXEMPTY (1u << 5)
#define LSR_RXRDY (1u << 0)
#define LSR_ERROR_BITS (7u << 2)

/* MCR */
#define MCR_DTR             (1u << 0)
#define MCR_RTS             (1u << 1)
#define MCR_TCR_TLR         (1u << 2)
#define MCR_LOOPBACK        (1u << 4)
#define MCR_XON_ANY         (1u << 5)
#define MCR_IRDA_ENABLE     (1u << 6)
#define MCR_PRESCALE_NEEDED (1u << 7)

/* MSR */
#define MSR_dCTS (1u << 0)
#define MSR_dDSR (1u << 1)
#define MSR_dRI  (1u << 2)
#define MSR_dCD  (1u << 3)
#define MSR_CTS  (1u << 4)
#define MSR_DSR  (1u << 5)
#define MSR_RI   (1u << 6)
#define MSR_CD   (1u << 7)

/* EFR */
#define EFR_ENHANCED_FUNC_ENABLE (1u << 4)
#define EFR_SPECIAL_CHAR_DETECT (1u << 5)
#define EFR_RTS_FLOW_CTRL_EN (1u << 6)
#define EFR_CTS_FLOW_CTRL_EN (1u << 7)

/* IOCONTROL: User accessible. Therefore see sc16is752.h for the defines. */

#define SC16IS752_FIFO_DEPTH 64

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_ATSAM_SC16IS752_H */
