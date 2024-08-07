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

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2008 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
******************************************************************************/

#ifndef __ALTERA_AVALON_TIMER_REGS_H__
#define __ALTERA_AVALON_TIMER_REGS_H__

#include <bsp_system.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ALTERA_AVALON_TIMER_STATUS_TO_MSK             (0x1)
#define ALTERA_AVALON_TIMER_STATUS_TO_OFST            (0)
#define ALTERA_AVALON_TIMER_STATUS_RUN_MSK            (0x2)
#define ALTERA_AVALON_TIMER_STATUS_RUN_OFST           (1)

#define ALTERA_AVALON_TIMER_CONTROL_ITO_MSK           (0x1)
#define ALTERA_AVALON_TIMER_CONTROL_ITO_OFST          (0)
#define ALTERA_AVALON_TIMER_CONTROL_CONT_MSK          (0x2)
#define ALTERA_AVALON_TIMER_CONTROL_CONT_OFST         (1)
#define ALTERA_AVALON_TIMER_CONTROL_START_MSK         (0x4)
#define ALTERA_AVALON_TIMER_CONTROL_START_OFST        (2)
#define ALTERA_AVALON_TIMER_CONTROL_STOP_MSK          (0x8)
#define ALTERA_AVALON_TIMER_CONTROL_STOP_OFST         (3)

#define ALTERA_AVALON_TIMER_PERIODL_MSK             (0xFFFF)
#define ALTERA_AVALON_TIMER_PERIODL_OFST            (0)

#define ALTERA_AVALON_TIMER_PERIODH_MSK             (0xFFFF)
#define ALTERA_AVALON_TIMER_PERIODH_OFST            (0)

#define ALTERA_AVALON_TIMER_SNAPL_MSK               (0xFFFF)
#define ALTERA_AVALON_TIMER_SNAPL_OFST              (0)

#define ALTERA_AVALON_TIMER_SNAPH_MSK               (0xFFFF)
#define ALTERA_AVALON_TIMER_SNAPH_OFST              (0)

#define ALTERA_AVALON_TIMER_PERIOD_0_MSK             (0xFFFF)
#define ALTERA_AVALON_TIMER_PERIOD_0_OFST            (0)

#define ALTERA_AVALON_TIMER_PERIOD_1_MSK             (0xFFFF)
#define ALTERA_AVALON_TIMER_PERIOD_1_OFST            (0)

#define ALTERA_AVALON_TIMER_PERIOD_2_MSK             (0xFFFF)
#define ALTERA_AVALON_TIMER_PERIOD_2_OFST            (0)

#define ALTERA_AVALON_TIMER_PERIOD_3_MSK             (0xFFFF)
#define ALTERA_AVALON_TIMER_PERIOD_3_OFST            (0)

#define ALTERA_AVALON_TIMER_SNAP_0_MSK               (0xFFFF)
#define ALTERA_AVALON_TIMER_SNAP_0_OFST              (0)

#define ALTERA_AVALON_TIMER_SNAP_1_MSK               (0xFFFF)
#define ALTERA_AVALON_TIMER_SNAP_1_OFST              (0)

#define ALTERA_AVALON_TIMER_SNAP_2_MSK               (0xFFFF)
#define ALTERA_AVALON_TIMER_SNAP_2_OFST              (0)

#define ALTERA_AVALON_TIMER_SNAP_3_MSK               (0xFFFF)
#define ALTERA_AVALON_TIMER_SNAP_3_OFST              (0)

#define MTIMECMP_MAX_VALUE                            0xFFFFFFFFFFFFFFFF

typedef struct
{
  volatile uint32_t status;
  volatile uint32_t control;
  volatile uint32_t period_lo;
  volatile uint32_t period_hi;
  volatile uint32_t snap_lo;
  volatile uint32_t snap_hi;
}altera_avalon_timer_regs_32;

typedef struct
{
  volatile uint32_t status;
  volatile uint32_t control;
  volatile uint32_t period_0;
  volatile uint32_t period_1;
  volatile uint32_t period_2;
  volatile uint32_t period_3;
  volatile uint32_t snap_0;
  volatile uint32_t snap_1;
  volatile uint32_t snap_2;
  volatile uint32_t snap_3;
}altera_avalon_timer_regs_64;

typedef struct
{
  volatile uint32_t status;
  volatile uint32_t control;
  volatile uint32_t period;
  volatile uint32_t snap;
  volatile uint32_t prescalar;
}altera_avalon_timer_precale_regs;

typedef struct
{
  volatile uint32_t mtimecmp_lo;
  volatile uint32_t mtimecmp_hi;
  volatile uint32_t mtime_lo;
  volatile uint32_t mtime_hi;
}altera_niosv_timer_regs;

#define CLOCK_REGS \
  ((volatile altera_niosv_timer_regs* )ALT_CPU_MTIME_OFFSET)
#define CLOCK_FREQ      ALT_CPU_FREQ
#define CLOCK_VECTOR    NIOSV_INTERRUPT_VECTOR_TIMER

#define TIMER_REGS \
  ((volatile altera_avalon_timer_precale_regs* )BENCHMARK_TIMER_BASE)
#define TIMER_FREQ      BENCHMARK_TIMER_FREQ
#define TIMER_VECTOR    BENCHMARK_TIMER_IRQ

#define WATCHDOG_REGS \
  ((volatile altera_avalon_timer_regs_32* )WATCHDOG_TIMER_BASE)
#define WATCHDOG_FREQ   WATCHDOG_TIMER_FREQ
#define WATCHDOG_VECTOR WATCHDOG_TIMER_IRQ

#ifdef __cplusplus
}
#endif

#endif /* __ALTERA_AVALON_TIMER_REGS_H__ */
