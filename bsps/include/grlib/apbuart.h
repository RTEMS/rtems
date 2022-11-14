/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup uart
 */

/*
 *  COPYRIGHT (c) 2007.
 *  Gaisler Research
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
 *
 */

#ifndef __APBUART_H__
#define __APBUART_H__

/**
 * @defgroup uart UART
 *
 * @ingroup RTEMSBSPsSharedGRLIB
 *
 * @brief Driver interface for APBUART
 *
 * @{
 */

#include "ambapp.h"
#include "grlib.h"

#ifdef __cplusplus
extern "C" {
#endif

#define APBUART_CTRL_RE 0x1
#define APBUART_CTRL_TE 0x2
#define APBUART_CTRL_RI 0x4
#define APBUART_CTRL_TI 0x8
#define APBUART_CTRL_PS 0x10
#define APBUART_CTRL_PE 0x20
#define APBUART_CTRL_FL 0x40
#define APBUART_CTRL_LB 0x80
#define APBUART_CTRL_EC 0x100
#define APBUART_CTRL_TF 0x200
#define APBUART_CTRL_RF 0x400
#define APBUART_CTRL_DB 0x800
#define APBUART_CTRL_BI 0x1000
#define APBUART_CTRL_DI 0x2000
#define APBUART_CTRL_FA 0x80000000

#define APBUART_STATUS_DR 0x1
#define APBUART_STATUS_TS 0x2
#define APBUART_STATUS_TE 0x4
#define APBUART_STATUS_BR 0x8
#define APBUART_STATUS_OV 0x10
#define APBUART_STATUS_PE 0x20
#define APBUART_STATUS_FE 0x40
#define APBUART_STATUS_ERR 0x78
#define APBUART_STATUS_TH 0x80
#define APBUART_STATUS_RH 0x100
#define APBUART_STATUS_TF 0x200
#define APBUART_STATUS_RF 0x400

void apbuart_outbyte_wait(const struct apbuart_regs *regs);

void apbuart_outbyte_polled(struct apbuart_regs *regs, char ch);

int apbuart_inbyte_nonblocking(struct apbuart_regs *regs);

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __APBUART_H__ */
