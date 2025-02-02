/*
 * Copyright (C) 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
#define SC16IS752_FCR_FIFO_EN        0x01
#define SC16IS752_FCR_RX_FIFO_RST    0x02
#define SC16IS752_FCR_TX_FIFO_RST    0x04
#define SC16IS752_FCR_TX_FIFO_TRG_8  0x00
#define SC16IS752_FCR_TX_FIFO_TRG_16 0x10
#define SC16IS752_FCR_TX_FIFO_TRG_32 0x20
#define SC16IS752_FCR_TX_FIFO_TRG_56 0x30
#define SC16IS752_FCR_RX_FIFO_TRG_8  0x00
#define SC16IS752_FCR_RX_FIFO_TRG_16 0x40
#define SC16IS752_FCR_RX_FIFO_TRG_56 0x80
#define SC16IS752_FCR_RX_FIFO_TRG_60 0xc0

/* EFCR */
#define SC16IS752_EFCR_RS485_ENABLE (1u << 0)
#define SC16IS752_EFCR_RX_DISABLE (1u << 1)
#define SC16IS752_EFCR_TX_DISABLE (1u << 2)
#define SC16IS752_EFCR_RTSCON (1u << 4)
#define SC16IS752_EFCR_RTSINVER (1u << 5)

/* IER */
#define SC16IS752_IER_RHR (1u << 0)
#define SC16IS752_IER_THR (1u << 1)
#define SC16IS752_IER_RECEIVE_LINE_STATUS (1u << 2)
#define SC16IS752_IER_MODEM_STATUS (1u << 3)
#define SC16IS752_IER_SLEEP_MODE (1u << 4)
#define SC16IS752_IER_XOFF (1u << 5)
#define SC16IS752_IER_RTS (1u << 6)
#define SC16IS752_IER_CTS (1u << 7)

/* IIR */
#define SC16IS752_IIR_TX_INTERRUPT (1u << 1)
#define SC16IS752_IIR_RX_INTERRUPT (1u << 2)

/* LCR */
#define SC16IS752_LCR_CHRL_5_BIT (0u << 1) | (0u << 0)
#define SC16IS752_LCR_CHRL_6_BIT (0u << 1) | (1u << 0)
#define SC16IS752_LCR_CHRL_7_BIT (1u << 1) | (0u << 0)
#define SC16IS752_LCR_CHRL_8_BIT (1u << 1) | (1u << 0)
#define SC16IS752_LCR_2_STOP_BIT (1u << 2)
#define SC16IS752_LCR_SET_PARITY (1u << 3)
#define SC16IS752_LCR_EVEN_PARITY (1u << 4)
#define SC16IS752_LCR_BREAK (1u << 5)
#define SC16IS752_LCR_ENABLE_DIVISOR (1u << 7)

/* LSR */
#define SC16IS752_LSR_TXEMPTY (1u << 5)
#define SC16IS752_LSR_RXRDY (1u << 0)
#define SC16IS752_LSR_ERROR_BITS (7u << 2)

/* MCR */
#define SC16IS752_MCR_DTR             (1u << 0)
#define SC16IS752_MCR_RTS             (1u << 1)
#define SC16IS752_MCR_TCR_TLR         (1u << 2)
#define SC16IS752_MCR_LOOPBACK        (1u << 4)
#define SC16IS752_MCR_XON_ANY         (1u << 5)
#define SC16IS752_MCR_IRDA_ENABLE     (1u << 6)
#define SC16IS752_MCR_PRESCALE_NEEDED (1u << 7)

/* MSR */
#define SC16IS752_MSR_dCTS (1u << 0)
#define SC16IS752_MSR_dDSR (1u << 1)
#define SC16IS752_MSR_dRI  (1u << 2)
#define SC16IS752_MSR_dCD  (1u << 3)
#define SC16IS752_MSR_CTS  (1u << 4)
#define SC16IS752_MSR_DSR  (1u << 5)
#define SC16IS752_MSR_RI   (1u << 6)
#define SC16IS752_MSR_CD   (1u << 7)

/* EFR */
#define SC16IS752_EFR_ENHANCED_FUNC_ENABLE (1u << 4)
#define SC16IS752_EFR_SPECIAL_CHAR_DETECT (1u << 5)
#define SC16IS752_EFR_RTS_FLOW_CTRL_EN (1u << 6)
#define SC16IS752_EFR_CTS_FLOW_CTRL_EN (1u << 7)

/* IOCONTROL: User accessible. Therefore see sc16is752.h for the defines. */

#define SC16IS752_FIFO_DEPTH 64

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_ATSAM_SC16IS752_H */
