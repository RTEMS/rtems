/*
 * Copyright (c) 2016, 2018 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
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
