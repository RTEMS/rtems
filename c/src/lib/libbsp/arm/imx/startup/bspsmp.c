/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

uint32_t _CPU_SMP_Initialize(void)
{
  return 1;
}

bool _CPU_SMP_Start_processor(uint32_t cpu_index)
{
  (void) cpu_index;

  /* Nothing to do */

  return true;
}

void _CPU_SMP_Finalize_initialization(uint32_t cpu_count)
{
}

void _CPU_SMP_Prepare_start_multitasking(void)
{
}

void _CPU_SMP_Send_interrupt(uint32_t target_processor_index)
{
}
