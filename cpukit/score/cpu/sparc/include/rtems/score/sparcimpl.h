/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
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

/*
 * Provides a mutable alias to _SPARC_Counter for use in
 * _SPARC_Counter_initialize().  The _SPARC_Counter and _SPARC_Counter_mutable
 * are defined via the SPARC_COUNTER_DEFINITION define.
 */
extern SPARC_Counter _SPARC_Counter_mutable;

CPU_Counter_ticks _SPARC_Counter_read_address( void );

CPU_Counter_ticks _SPARC_Counter_read_asr23( void );

CPU_Counter_ticks _SPARC_Counter_difference_normal(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
);

CPU_Counter_ticks _SPARC_Counter_difference_clock_period(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
);

/*
 * Returns always a value of one regardless of the parameters.  This prevents
 * an infinite loop in rtems_counter_delay_ticks().  Its only a reasonably safe
 * default.
 */
CPU_Counter_ticks _SPARC_Counter_difference_one(
  CPU_Counter_ticks second,
  CPU_Counter_ticks first
);

static inline void _SPARC_Counter_initialize(
  SPARC_Counter_read                counter_read,
  SPARC_Counter_difference          counter_difference,
  volatile const CPU_Counter_ticks *counter_address
)
{
  _SPARC_Counter_mutable.counter_read = counter_read;
  _SPARC_Counter_mutable.counter_difference = counter_difference;
  _SPARC_Counter_mutable.counter_address = counter_address;
}

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
    "\t.size\t_SPARC_Counter, 12\n" \
    "_SPARC_Counter:\n" \
    "_SPARC_Counter_mutable:\n" \
    "\t.long\t_SPARC_Counter_read_address\n" \
    "\t.long\t_SPARC_Counter_difference_one\n" \
    "\t.long\t_SPARC_Counter\n" \
    "\t.previous\n" \
  )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SPARCIMPL_H */
