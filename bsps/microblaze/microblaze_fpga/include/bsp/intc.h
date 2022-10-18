/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief MicroBlaze AXI Interrupt Controller definitions
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#ifndef LIBBSP_MICROBLAZE_FPGA_INTC_H
#define LIBBSP_MICROBLAZE_FPGA_INTC_H

#include <bspopts.h>

#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  /* Interrupt Status Register */
  uint32_t isr;
  uint32_t ipr;
  /* Interrupt Enable Register */
  uint32_t ier;
  /* Interrupt Acknowledge Register */
  uint32_t iar;
  uint32_t sie;
  uint32_t cie;
  uint32_t ivr;
#define MICROBLAZE_INTC_MER_HIE BSP_BIT32(1)
#define MICROBLAZE_INTC_MER_ME BSP_BIT32(0)
  /* Master Enable Register */
  uint32_t mer;
  /* Interrupt Mode Register, this is present only for Fast Interrupt */
  uint32_t imr;
  /* Interrupt Level Register */
  uint32_t ilr;
} Microblaze_INTC;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_MICROBLAZE_FPGA_INTC_H */
