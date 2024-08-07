/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *
 * Copyright (C) 2024 Kevin Kirspel
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _ALTERA_AVALON_HBUS_REGS_H
#define _ALTERA_AVALON_HBUS_REGS_H

#include <bsp_system.h>

/* CSR */
#define ALTERA_HBUS_CSR_RACT_MSK            (0x1)
#define ALTERA_HBUS_CSR_RACT_OFST           (0)
#define ALTERA_HBUS_CSR_RDECERR_MSK         (0x100)
#define ALTERA_HBUS_CSR_RDECERR_OFST        (8)
#define ALTERA_HBUS_CSR_RTRSERR_MSK         (0x200)
#define ALTERA_HBUS_CSR_RTRSERR_OFST        (9)
#define ALTERA_HBUS_CSR_RRSTOERR_MSK        (0x400)
#define ALTERA_HBUS_CSR_RRSTOERR_OFST       (10)
#define ALTERA_HBUS_CSR_RDSSTALL_MSK        (0x800)
#define ALTERA_HBUS_CSR_RDSSTALL_OFST       (11)
#define ALTERA_HBUS_CSR_WACT_MSK            (0x10000)
#define ALTERA_HBUS_CSR_WACT_OFST           (16)
#define ALTERA_HBUS_CSR_WDECERR_MSK         (0x1000000)
#define ALTERA_HBUS_CSR_WDECERR_OFST        (24)
#define ALTERA_HBUS_CSR_WTRSERR_MSK         (0x2000000)
#define ALTERA_HBUS_CSR_WTRSERR_OFST        (25)
#define ALTERA_HBUS_CSR_WRSTOERR_MSK        (0x4000000)
#define ALTERA_HBUS_CSR_WRSTOERR_OFST       (26)

/* IEN */
#define ALTERA_HBUS_IEN_RPCINTE_MSK         (0x1)
#define ALTERA_HBUS_IEN_RPCINTE_OFST        (0)
#define ALTERA_HBUS_IEN_INTP_MSK            (0x80000000)
#define ALTERA_HBUS_IEN_INTP_OFST           (31)

/* ISR */
#define ALTERA_HBUS_ISR_RPCINTS_MSK         (0x1)
#define ALTERA_HBUS_ISR_RPCINTS_OFST        (0)

/* MBR */
#define ALTERA_HBUS_MBR_ADDRESS_MSK         (0xFF000000)
#define ALTERA_HBUS_MBR_ADDRESS(x)          (x & ALTERA_HBUS_MBR_ADDRESS_MSK)

/* MCR */
#define ALTERA_HBUS_MCR_WRAPSIZE_MSK        (0x3)
#define ALTERA_HBUS_MCR_WRAPSIZE_OFST       (0)
#define ALTERA_HBUS_MCR_DEVTYPE_MSK         (0x10)
#define ALTERA_HBUS_MCR_DEVTYPE_OFST        (4)
#define ALTERA_HBUS_MCR_CRT_MSK             (0x20)
#define ALTERA_HBUS_MCR_CRT_OFST            (5)
#define ALTERA_HBUS_MCR_ACS_MSK             (0x10000)
#define ALTERA_HBUS_MCR_ACS_OFST            (16)
#define ALTERA_HBUS_MCR_TCMO_MSK            (0x20000)
#define ALTERA_HBUS_MCR_TCMO_OFST           (17)
#define ALTERA_HBUS_MCR_MAXLEN_MSK          (0x3FC0000)
#define ALTERA_HBUS_MCR_MAXLEN_OFST         (18)
#define ALTERA_HBUS_MCR_MAXEN_MSK           (0x80000000)
#define ALTERA_HBUS_MCR_MAXEN_OFST          (31)

#define ALTERA_HBUS_MCR_WRAPSIZE_SELECT(x)  \
  ((x) == 32 ? 0x3 : (x) == 64 ? 0x1 : (x) == 16 ? 0x2 : 0x3)
#define ALTERA_HBUS_MCR_WRAPSIZE(x)         \
  ((ALTERA_HBUS_MCR_WRAPSIZE_SELECT(x) << ALTERA_HBUS_MCR_WRAPSIZE_OFST) & \
  ALTERA_HBUS_MCR_WRAPSIZE_MSK)
#define ALTERA_HBUS_MCR_DEVTYPE(x)          \
  (((x) << ALTERA_HBUS_MCR_DEVTYPE_OFST) & ALTERA_HBUS_MCR_DEVTYPE_MSK)
#define ALTERA_HBUS_MCR_CRT(x)              \
  (((x) << ALTERA_HBUS_MCR_CRT_OFST) & ALTERA_HBUS_MCR_CRT_MSK)
#define ALTERA_HBUS_MCR_CRT_UPDATE(r, x)    \
  ((r) = ((r) & ALTERA_HBUS_MCR_CRT_MSK) | ALTERA_HBUS_MCR_CRT(x))
#define ALTERA_HBUS_MCR_ACS(x)              \
  (((x) << ALTERA_HBUS_MCR_ACS_OFST) & ALTERA_HBUS_MCR_ACS_MSK)
#define ALTERA_HBUS_MCR_TCMO(x)             \
  (((x) << ALTERA_HBUS_MCR_TCMO_OFST) & ALTERA_HBUS_MCR_TCMO_MSK)
#define ALTERA_HBUS_MCR_MAXLEN(x)           \
  (((((x) / 2) - 1) << ALTERA_HBUS_MCR_MAXLEN_OFST) & \
  ALTERA_HBUS_MCR_MAXLEN_MSK)
#define ALTERA_HBUS_MCR_MAXEN(x)            \
  (((x) << ALTERA_HBUS_MCR_MAXEN_OFST) & ALTERA_HBUS_MCR_MAXEN_MSK)

/* MTR */
#define ALTERA_HBUS_MTR_LTCY_MSK            (0xF)
#define ALTERA_HBUS_MTR_LTCY_OFST           (0)
#define ALTERA_HBUS_MTR_RFU_MSK             (0xF0)
#define ALTERA_HBUS_MTR_RFU_OFST            (4)
#define ALTERA_HBUS_MTR_WCSH_MSK            (0xF00)
#define ALTERA_HBUS_MTR_WCSH_OFST           (8)
#define ALTERA_HBUS_MTR_RCSH_MSK            (0xF000)
#define ALTERA_HBUS_MTR_RCSH_OFST           (12)
#define ALTERA_HBUS_MTR_WCSS_MSK            (0xF0000)
#define ALTERA_HBUS_MTR_WCSS_OFST           (16)
#define ALTERA_HBUS_MTR_RCSS_MSK            (0xF00000)
#define ALTERA_HBUS_MTR_RCSS_OFST           (20)
#define ALTERA_HBUS_MTR_WCSHI_MSK           (0xF000000)
#define ALTERA_HBUS_MTR_WCSHI_OFST          (24)
#define ALTERA_HBUS_MTR_RCSHI_MSK           (0xF0000000)
#define ALTERA_HBUS_MTR_RCSHI_OFST          (28)

#define ALTERA_HBUS_MTR_LTCY_SELECT(x)      \
  ((x) == 6 ? 0x1 : (x) == 4 ? 0xf : (x) == 5 ? 0x0 : (x) == 3 ? 0xe : 0x1)
#define ALTERA_HBUS_MTR_LTCY(x)             \
  ((ALTERA_HBUS_MTR_LTCY_SELECT(x) << ALTERA_HBUS_MTR_LTCY_OFST) & \
  ALTERA_HBUS_MTR_LTCY_MSK)

/* GPOR */
#define ALTERA_HBUS_GPOR_GPO_MSK            (0x3)
#define ALTERA_HBUS_GPOR_GPO_OFST           (0)

/* WPR */
#define ALTERA_HBUS_WPR_WP_MSK              (0x1)
#define ALTERA_HBUS_WPR_WP_OFST             (0)

/* LBR */
#define ALTERA_HBUS_LBR_LOOPBACK_MSK        (0x1)
#define ALTERA_HBUS_LBR_LOOPBACK_OFST       (0)

/* TAR */
#define ALTERA_HBUS_TAR_WTA_MSK             (0x3)
#define ALTERA_HBUS_TAR_WTA_OFST            (0)
#define ALTERA_HBUS_TAR_RTA_MSK             (0x30)
#define ALTERA_HBUS_TAR_RTA_OFST            (4)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  volatile uint32_t csr;
  volatile uint32_t ien;
  volatile uint32_t isr;
  volatile uint32_t icr;
  volatile uint32_t mbr0;
  volatile uint32_t mbr1;
  volatile uint32_t mbr2;
  volatile uint32_t mbr3;
  volatile uint32_t mcr0;
  volatile uint32_t mcr1;
  volatile uint32_t mcr2;
  volatile uint32_t mcr3;
  volatile uint32_t mtr0;
  volatile uint32_t mtr1;
  volatile uint32_t mtr2;
  volatile uint32_t mtr3;
  volatile uint32_t gpor;
  volatile uint32_t wpr;
  volatile uint32_t lbr;
  volatile uint32_t tar;
}altera_avalon_hbus_ctrl_regs;

#define HBUS_CTRL_REGS \
  (( volatile altera_avalon_hbus_ctrl_regs* ) \
  HYPERBUS_CTRL_ALTERA_AXI4_SLAVE_REGISTER_BASE )

#ifdef __cplusplus
}
#endif

#endif
