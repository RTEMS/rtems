/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/thread.h>

#ifdef ARM_MULTILIB_ARCH_V7M

#include <rtems/score/armv7m.h>

void _ARMV7M_Interrupt_service_enter( void )
{
  ++_Thread_Dispatch_disable_level;
  ++_ISR_Nest_level;
}

void _ARMV7M_Interrupt_service_leave( void )
{
  --_ISR_Nest_level;
  --_Thread_Dispatch_disable_level;
  if (
    _ISR_Nest_level == 0
      && _Thread_Dispatch_disable_level == 0
      && _Thread_Dispatch_necessary
  ) {
    _ARMV7M_SCB->icsr = ARMV7M_SCB_ICSR_PENDSVSET;
  }
}

#endif /* ARM_MULTILIB_ARCH_V7M */
