/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsSPARCShared
 *
 * @brief This header file provides interfaces of a CPU counter implementation
 *   for SPARC BSPs.
 */

/*
 * Copyright (C) 2016, 2023 embedded brains GmbH & Co. KG
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

#ifndef _BSP_SPARC_COUNTER_H
#define _BSP_SPARC_COUNTER_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct timecounter;

void _SPARC_Counter_at_tick_clock( void );

CPU_Counter_ticks _SPARC_Counter_read_default( void );

CPU_Counter_ticks _SPARC_Counter_read_clock_isr_disabled( void );

CPU_Counter_ticks _SPARC_Counter_read_clock( void );

uint32_t _SPARC_Get_timecount_clock( struct timecounter * );

typedef CPU_Counter_ticks ( *SPARC_Counter_read )( void );

/*
 * The SPARC processors supported by RTEMS have no built-in CPU counter
 * support.  We have to use some hardware counter module for this purpose, for
 * example the GPTIMER instance used by the clock driver.  The BSP must provide
 * an implementation of the CPU counter read function.  This allows the use of
 * dynamic hardware enumeration.
 */
typedef struct {
  SPARC_Counter_read                read_isr_disabled;
  SPARC_Counter_read                read;
  volatile const CPU_Counter_ticks *counter_register;
  volatile const uint32_t          *pending_register;
  uint32_t                          pending_mask;
  CPU_Counter_ticks                 accumulated;
  CPU_Counter_ticks                 interval;
} SPARC_Counter;

extern SPARC_Counter _SPARC_Counter;

#define SPARC_COUNTER_DEFINITION \
  SPARC_Counter _SPARC_Counter = { \
    .read_isr_disabled = _SPARC_Counter_read_default, \
    .read = _SPARC_Counter_read_default \
  }

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BSP_SPARC_COUNTER_H */
