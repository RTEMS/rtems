/* SPDX-License-Identifier: BSD-2-Clause OR Apache-2.0 OR GPL-2.0-or-later */

/**
 * @file
 *
 * @ingroup DCAN
 *
 * @brief This header file provides DCAN register definitions.
 */

/*
 * Copyright (C) 2026 Ning Zhang <riverquiet66@gmail.com>
 * Copyright (C) 2026 Michal Lenc <michallenc@seznam.cz>
 * Copyright (C) 2026 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 * Based on DCAN driver code by Prashanth S <fishesprashanth@gmail.com>
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

#ifndef _DEV_CAN_DCAN_DCAN_REGS_H
#define _DEV_CAN_DCAN_DCAN_REGS_H

#include <dev/can/can-helpers.h>
#include <stdint.h>

/*
 * DCAN register offsets.
 */
enum dcan_regs {
  DCAN_CTL = 0x000,
  DCAN_ES = 0x004,
  DCAN_ERRC = 0x008,
  DCAN_BTR = 0x00c,
  DCAN_INT = 0x010,
  DCAN_TEST = 0x014,
  DCAN_PERR = 0x01c,
  DCAN_ABOTR = 0x080,
  DCAN_TXRQ_X = 0x084,
  DCAN_NWDAT_X = 0x098,
  DCAN_INTPND_X = 0x0ac,
  DCAN_MSGVAL_X = 0x0c0,
  DCAN_IF3OBS = 0x140,
  DCAN_TIOC = 0x1e0,
  DCAN_RIOC = 0x1e4
};

/*
 * DCAN register arrays.
 */
#define DCAN_TXRQ(n) (0x088 + ((n) * 4))
#define DCAN_NWDAT(n) (0x09c + ((n) * 4))
#define DCAN_INTPND(n) (0x0b0 + ((n) * 4))
#define DCAN_MSGVAL(n) (0x0c4 + ((n) * 4))
#define DCAN_INTMUX(n) (0x0d8 + ((n) * 4))

/*
 * DCAN interface register sets.
 *
 * n = 1, 2, or 3
 * For DCAN_IFCMD(3) is IF3OBServation
 */
#define DCAN_IFCMD(n) (0x100 + (((n) - 1) * 0x20))
#define DCAN_IFMSK(n) (0x104 + (((n) - 1) * 0x20))
#define DCAN_IFARB(n) (0x108 + (((n) - 1) * 0x20))
#define DCAN_IFMCTL(n) (0x10c + (((n) - 1) * 0x20))
#define DCAN_IFDATA(n) (0x110 + (((n) - 1) * 0x20))
#define DCAN_IFDATB(n) (0x114 + (((n) - 1) * 0x20))

#define DCAN_IF3UPD(n) (0x160 + ((n) * 4))

/*
 * CTL register.
 */
#define DCAN_CTL_INIT BIT(0)
#define DCAN_CTL_IE0 BIT(1)
#define DCAN_CTL_SIE BIT(2)
#define DCAN_CTL_EIE BIT(3)
#define DCAN_CTL_DAR BIT(5)
#define DCAN_CTL_CCE BIT(6)
#define DCAN_CTL_TEST BIT(7)
#define DCAN_CTL_IDS BIT(8)
#define DCAN_CTL_ABO BIT(9)
#define DCAN_CTL_PMD GENMASK(13, 10)
#define DCAN_CTL_SWR BIT(15)
#define DCAN_CTL_INITDBG BIT(16)
#define DCAN_CTL_IE1 BIT(17)
#define DCAN_CTL_DE1 BIT(18)
#define DCAN_CTL_DE2 BIT(19)
#define DCAN_CTL_DE3 BIT(20)
#define DCAN_CTL_PDR BIT(24)
#define DCAN_CTL_WUBA BIT(25)

/*
 * ES register.
 */
#define DCAN_ES_LEC GENMASK(2, 0)
#define DCAN_ES_TXOK BIT(3)
#define DCAN_ES_RXOK BIT(4)
#define DCAN_ES_EPASS BIT(5)
#define DCAN_ES_EWARN BIT(6)
#define DCAN_ES_BOFF BIT(7)
#define DCAN_ES_PER BIT(8)
#define DCAN_ES_WAKEUPPND BIT(9)
#define DCAN_ES_PDA BIT(10)

/*
 * ES.LEC values.
 */
#define DCAN_ES_LEC_NO_ERROR 0u
#define DCAN_ES_LEC_STUFF_ERROR 1u
#define DCAN_ES_LEC_FORM_ERROR 2u
#define DCAN_ES_LEC_ACK_ERROR 3u
#define DCAN_ES_LEC_BIT1_ERROR 4u
#define DCAN_ES_LEC_BIT0_ERROR 5u
#define DCAN_ES_LEC_CRC_ERROR 6u
#define DCAN_ES_LEC_NO_EVENT 7u

/*
 * ERRC register.
 */
#define DCAN_ERRC_TEC GENMASK(7, 0)
#define DCAN_ERRC_REC GENMASK(14, 8)
#define DCAN_ERRC_RP BIT(15)

/*
 * BTR register.
 */
#define DCAN_BTR_BRP GENMASK(5, 0)
#define DCAN_BTR_SJW GENMASK(7, 6)
#define DCAN_BTR_TSEG1 GENMASK(11, 8)
#define DCAN_BTR_TSEG2 GENMASK(14, 12)
#define DCAN_BTR_BRPE GENMASK(19, 16)

/*
 * INT register.
 */
#define DCAN_INT_INT0ID GENMASK(15, 0)
#define DCAN_INT_INT1ID GENMASK(23, 16)

#define DCAN_INT_STATUS_ID 0x8000u
#define DCAN_INT_NO_INTERRUPT 0x0000u

/*
 * TEST register.
 */
#define DCAN_TEST_SILENT BIT(3)
#define DCAN_TEST_LBACK BIT(4)
#define DCAN_TEST_TX GENMASK(6, 5)
#define DCAN_TEST_RX BIT(7)
#define DCAN_TEST_EXL BIT(8)
#define DCAN_TEST_RDA BIT(9)

/*
 * PERR register.
 */
#define DCAN_PERR_MESSAGE_NUMBER GENMASK(7, 0)
#define DCAN_PERR_WORD_NUMBER GENMASK(10, 8)

/*
 * IFxCMD register.
 */
#define DCAN_IFCMD_MSG_NUM(n) ((uint32_t)(n) & GENMASK(7, 0))
#define DCAN_IFCMD_MESSAGENUMBER GENMASK(7, 0)
#define DCAN_IFCMD_DMAACTIVE BIT(14)
#define DCAN_IFCMD_BUSY BIT(15)
#define DCAN_IFCMD_DATAB BIT(16)
#define DCAN_IFCMD_DATAA BIT(17)
#define DCAN_IFCMD_TXRQST_NEWDAT BIT(18)
#define DCAN_IFCMD_CLRINTPND BIT(19)
#define DCAN_IFCMD_CONTROL BIT(20)
#define DCAN_IFCMD_ARB BIT(21)
#define DCAN_IFCMD_MASK BIT(22)
#define DCAN_IFCMD_WR_RD BIT(23)

/*
 * IFxMSK register.
 */
#define DCAN_IFMSK_MSK GENMASK(28, 0)
#define DCAN_IFMSK_MDIR BIT(30)
#define DCAN_IFMSK_MXTD BIT(31)

/*
 * IFxARB register.
 */
#define DCAN_IFARB_ID GENMASK(28, 0)
#define DCAN_IFARB_DIR BIT(29)
#define DCAN_IFARB_XTD BIT(30)
#define DCAN_IFARB_MSGVAL BIT(31)

/*
 * IFxMCTL register.
 */
#define DCAN_IFMCTL_DATALENGTHCODE GENMASK(3, 0)
#define DCAN_IFMCTL_EOB BIT(7)
#define DCAN_IFMCTL_TXRQST BIT(8)
#define DCAN_IFMCTL_RMTEN BIT(9)
#define DCAN_IFMCTL_RXIE BIT(10)
#define DCAN_IFMCTL_TXIE BIT(11)
#define DCAN_IFMCTL_UMASK BIT(12)
#define DCAN_IFMCTL_INTPND BIT(13)
#define DCAN_IFMCTL_MSGLST BIT(14)
#define DCAN_IFMCTL_NEWDAT BIT(15)

/*
 * IFxDATA / IFxDATB registers.
 */
#define DCAN_IFDATA_DATA GENMASK(31, 0)
#define DCAN_IFDATB_DATA GENMASK(31, 0)

/*
 * IF3OBS register.
 */
#define DCAN_IF3OBS_MASK BIT(0)
#define DCAN_IF3OBS_ARB BIT(1)
#define DCAN_IF3OBS_CTRL BIT(2)
#define DCAN_IF3OBS_DATAA BIT(3)
#define DCAN_IF3OBS_DATAB BIT(4)
#define DCAN_IF3OBS_IF3SM BIT(8)
#define DCAN_IF3OBS_IF3SA BIT(9)
#define DCAN_IF3OBS_IF3SC BIT(10)
#define DCAN_IF3OBS_IF3SDA BIT(11)
#define DCAN_IF3OBS_IF3SDB BIT(12)
#define DCAN_IF3OBS_IF3UPD BIT(15)

/*
 * TIOC register.
 */
#define DCAN_TIOC_IN BIT(0)
#define DCAN_TIOC_OUT BIT(1)
#define DCAN_TIOC_DIR BIT(2)
#define DCAN_TIOC_FUNC BIT(3)
#define DCAN_TIOC_OD BIT(16)
#define DCAN_TIOC_PD BIT(17)
#define DCAN_TIOC_PU BIT(18)

/*
 * RIOC register.
 */
#define DCAN_RIOC_IN BIT(0)
#define DCAN_RIOC_OUT BIT(1)
#define DCAN_RIOC_DIR BIT(2)
#define DCAN_RIOC_FUNC BIT(3)
#define DCAN_RIOC_OD BIT(16)
#define DCAN_RIOC_PD BIT(17)
#define DCAN_RIOC_PU BIT(18)

#endif
