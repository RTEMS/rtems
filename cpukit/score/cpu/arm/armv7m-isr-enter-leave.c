/**
 *  @file
 *
 *  @brief ARMV7M Interrupt Service Enter and Leave
 */

/*
 * Copyright (c) 2011, 2017 Sebastian Huber.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/armv7m.h>
#include <rtems/score/isr.h>
#include <rtems/score/threaddispatch.h>

#ifdef ARM_MULTILIB_ARCH_V7M

void _ARMV7M_Interrupt_service_enter( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  ++cpu_self->thread_dispatch_disable_level;
  ++cpu_self->isr_nest_level;
}

void _ARMV7M_Interrupt_service_leave( void )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  --cpu_self->thread_dispatch_disable_level;
  --cpu_self->isr_nest_level;

  /*
   * Optimistically activate a pendable service call if a thread dispatch is
   * necessary.  The _ARMV7M_Pendable_service_call() will check that a thread
   * dispatch is allowed.
   */
  if ( cpu_self->dispatch_necessary ) {
    _ARMV7M_SCB->icsr = ARMV7M_SCB_ICSR_PENDSVSET;
  }
}

#endif /* ARM_MULTILIB_ARCH_V7M */
