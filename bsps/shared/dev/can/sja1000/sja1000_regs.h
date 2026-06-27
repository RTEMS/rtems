/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup SJA1000
 *
 * @brief This source file contains the register's definition of SJA1000
 *   controller.
 */

/*
 * Copyright (C) 2025-2026 Michal Lenc <michallenc@seznam.cz> self-funded
 * Copyright (C) 2025-2026 Pavel Pisa <pisa@cmp.felk.cvut.cz> self-funded
 * Implementation is based on original LinCAN SJA1000 driver
 * Copyright (C) 2002-2009 DCE FEE CTU Prague <http://dce.felk.cvut.cz>
 * Copyright (C) 2002-2009 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 * Copyright (C) 2004-2005 Tomasz Motylewski (BFAD GmbH)
 * Funded by OCERA and FRESCOR IST projects
 * Based on CAN driver code by Arnaud Westenberg <arnaud@wanadoo.nl>
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

#ifndef _DEV_CAN_SJA1000_SJA1000_REGS_H
#define _DEV_CAN_SJA1000_SJA1000_REGS_H

#include <dev/can/can-helpers.h>

/* PeliCAN mode */
enum sja1000_regs {
  SJA1000_MODE = 0x00,
  SJA1000_CMR = 0x01,
  SJA1000_SR = 0x02,
  SJA1000_IR = 0x03,
  SJA1000_IER = 0x04,
  SJA1000_BTR0 = 0x06,
  SJA1000_BTR1 = 0x07,
  SJA1000_OCR = 0x08,
  SJA1000_ALC = 0x0b,
  SJA1000_ECC = 0x0c,
  SJA1000_EWLR = 0x0d,
  SJA1000_RXERR = 0x0e,
  SJA1000_TXERR0 = 0x0e,
  SJA1000_TXERR1 = 0x0f,
  SJA1000_RMC = 0x1d,
  SJA1000_RBSA = 0x1e,
  SJA1000_FRM = 0x10,
  SJA1000_ID0 = 0x11,
  SJA1000_ID1 = 0x12,
  SJA1000_ID2 = 0x13,
  SJA1000_ID3 = 0x14,
  SJA1000_DATS = 0x13,
  SJA1000_DATE = 0x15,
  SJA1000_ACR0 = 0x10,
  SJA1000_ACR1 = 0x11,
  SJA1000_ACR2 = 0x12,
  SJA1000_ACR3 = 0x13,
  SJA1000_AMR0 = 0x14,
  SJA1000_AMR1 = 0x15,
  SJA1000_AMR2 = 0x16,
  SJA1000_AMR3 = 0x17,
  SJA1000_CDR = 0x1f
};

/* Mode registers */
#define REG_MODE_RM BIT(0)
#define REG_MODE_LOM BIT(1)
#define REG_MODE_STM BIT(2)
#define REG_MODE_AFM BIT(3)
#define REG_MODE_SM BIT(4)

/* Command registers */
#define REG_CMR_TR BIT(0)
#define REG_CMR_AT BIT(1)
#define REG_CMR_RRB BIT(2)
#define REG_CMR_CDO BIT(3)
#define REG_CMR_SRR BIT(4)

/* Status registers */
#define REG_SR_RBS BIT(0)
#define REG_SR_DOS BIT(1)
#define REG_SR_TBS BIT(2)
#define REG_SR_TCS BIT(3)
#define REG_SR_RS BIT(4)
#define REG_SR_TS BIT(5)
#define REG_SR_ES BIT(6)
#define REG_SR_BS BIT(7)
#define REG_SR_WORKER_MASK (0x5)

/* Interrupt registers */
#define REG_INT_RI BIT(0)
#define REG_INT_TI BIT(1)
#define REG_INT_EI BIT(2)
#define REG_INT_DOI BIT(3)
#define REG_INT_WUI BIT(4)
#define REG_INT_EPI BIT(5)
#define REG_INT_ALI BIT(6)
#define REG_INT_BEI BIT(7)
#define REG_INT_MASK (0xff)
#define REG_INT_WORKER_MASK (0xfc)

/* Arbitration lost capture register */
#define REG_ALC_RTR (0x1f)
#define REG_ALC_IDE (0x1c)
#define REG_ALC_SRTR (0x0b)

/* Error Code Capture Register */
#define REG_ECC_DIR BIT(5)
#define REG_ECC_ERCC GENMASK(7, 6)
#define REG_ECC_FUNC GENMASK(4, 0)
#define REG_ECC_ERCC_BIT (0)
#define REG_ECC_ERCC_FORM (1)
#define REG_ECC_ERCC_STUFF (2)
#define REG_ECC_ERCC_OTHER (3)

#define REG_ECC_ERROR_ACTIVE (0x11)
#define REG_ECC_ERROR_PASSIVE (0x16)

/* Frame format information */
#define REG_FRM_DLC GENMASK(3, 0)
#define REG_FRM_RTR BIT(6)
#define REG_FRM_FF BIT(7)
#define REG_FRM_DLC_MASK (BIT(4) - 1)

/* Bus Timing 1 Register */
#define REG_BTR1_TSEG1_MAX (15)
#define REG_BTR1_TSEG2_MAX (7)

/* Output Control Register */
#define REG_OCR_MODE_BIPHASE (0)
#define REG_OCR_MODE_TEST (1)
#define REG_OCR_MODE_NORMAL (2)
#define REG_OCR_MODE_CLOCK (3)
#define REG_OCR_TX0_LH (0x18)
#define REG_OCR_TX1_ZZ (0)

/* Clock Divider register */
#define REG_CDR_OFF BIT(3)
#define REG_CDR_RXINPEN BIT(5)
#define REG_CDR_CBP BIT(6)
#define REG_CDR_PELICAN BIT(7)
#define REG_CDR_CLKOUT_DIV1 (7)
#define REG_CDR_CLKOUT_DIV2 (0)
#define REG_CDR_CLKOUT_DIV4 (1)
#define REG_CDR_CLKOUT_DIV6 (2)
#define REG_CDR_CLKOUT_DIV8 (3)
#define REG_CDR_CLKOUT_DIV10 (4)
#define REG_CDR_CLKOUT_DIV12 (5)
#define REG_CDR_CLKOUT_DIV14 (6)
#define REG_CDR_CLKOUT_MASK (7)

#endif /* _DEV_CAN_SJA1000_SJA1000_REGS_H */
