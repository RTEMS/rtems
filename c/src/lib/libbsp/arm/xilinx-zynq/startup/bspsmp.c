/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <info@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/smpimpl.h>

#include <bsp/start.h>

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  /*
   * Enable the second CPU.
   */
  if (cpu_index != 0) {
    volatile uint32_t* const kick_address = (uint32_t*) 0xfffffff0UL;
    _ARM_Data_synchronization_barrier();
    _ARM_Instruction_synchronization_barrier();
    *kick_address = (uint32_t) _start;
    _ARM_Data_synchronization_barrier();
    _ARM_Instruction_synchronization_barrier();
    _ARM_Send_event();
  }

  /*
   * Wait for secondary processor to complete its basic initialization so that
   * we can enable the unified L2 cache.
   */
  return _Per_CPU_State_wait_for_non_initial_state(cpu_index, 0);
}
