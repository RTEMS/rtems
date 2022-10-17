/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief MicroBlaze AXI Timer definitions
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

#ifndef LIBBSP_MICROBLAZE_FPGA_TIMER_H
#define LIBBSP_MICROBLAZE_FPGA_TIMER_H

#include <bspopts.h>

#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
#define MICROBLAZE_TIMER_TCSR0_T0INT BSP_BIT32(8)
#define MICROBLAZE_TIMER_TCSR0_ENT0 BSP_BIT32(7)
#define MICROBLAZE_TIMER_TCSR0_ENIT0 BSP_BIT32(6)
#define MICROBLAZE_TIMER_TCSR0_LOAD0 BSP_BIT32(5)
#define MICROBLAZE_TIMER_TCSR0_ARHT0 BSP_BIT32(4)
#define MICROBLAZE_TIMER_TCSR0_GENT0 BSP_BIT32(2)
#define MICROBLAZE_TIMER_TCSR0_UDT0 BSP_BIT32(1)
  /* Control/Status register */
  uint32_t tcsr0;
  /* Load register */
  uint32_t tlr0;
  /* Timer counter register */
  uint32_t tcr0;
} Microblaze_Timer;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_MICROBLAZE_FPGA_TIMER_H */
