/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUSPARC
 *
 * @brief This header file provides interfaces used by the SPARC port of RTEMS.
 */

/*
 * Copyright (C) 2016, 2018 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SCORE_SPARCIMPL_H
#define _RTEMS_SCORE_SPARCIMPL_H

#include <rtems/score/cpu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct timecounter;

/*
 * Provides a mutable alias to _SPARC_Counter for use in
 * _SPARC_Counter_initialize().  The _SPARC_Counter and _SPARC_Counter_mutable
 * are defined via the SPARC_COUNTER_DEFINITION define.
 */
extern SPARC_Counter _SPARC_Counter_mutable;

void _SPARC_Counter_at_tick_clock( void );

CPU_Counter_ticks _SPARC_Counter_read_default( void );

CPU_Counter_ticks _SPARC_Counter_read_up( void );

CPU_Counter_ticks _SPARC_Counter_read_down( void );

CPU_Counter_ticks _SPARC_Counter_read_clock_isr_disabled( void );

CPU_Counter_ticks _SPARC_Counter_read_clock( void );

CPU_Counter_ticks _SPARC_Counter_read_asr23( void );

uint32_t _SPARC_Get_timecount_up( struct timecounter * );

uint32_t _SPARC_Get_timecount_down( struct timecounter * );

uint32_t _SPARC_Get_timecount_clock( struct timecounter * );

uint32_t _SPARC_Get_timecount_asr23( struct timecounter * );

/*
 * Defines the _SPARC_Counter and _SPARC_Counter_mutable global variables.
 * Place this define in the global file scope of the CPU counter support file
 * of the BSP.
 */
#define SPARC_COUNTER_DEFINITION \
  __asm__ ( \
    "\t.global\t_SPARC_Counter\n" \
    "\t.global\t_SPARC_Counter_mutable\n" \
    "\t.section\t.data._SPARC_Counter,\"aw\",@progbits\n" \
    "\t.align\t4\n" \
    "\t.type\t_SPARC_Counter, #object\n" \
    "\t.size\t_SPARC_Counter, 28\n" \
    "_SPARC_Counter:\n" \
    "_SPARC_Counter_mutable:\n" \
    "\t.long\t_SPARC_Counter_read_default\n" \
    "\t.long\t_SPARC_Counter_read_default\n" \
    "\t.long\t0\n" \
    "\t.long\t0\n" \
    "\t.long\t0\n" \
    "\t.long\t0\n" \
    "\t.long\t0\n" \
    "\t.previous\n" \
  )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SPARCIMPL_H */
