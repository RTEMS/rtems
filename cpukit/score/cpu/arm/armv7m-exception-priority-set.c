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

#include <rtems/score/cpu.h>

#ifdef ARM_MULTILIB_ARCH_V7M

#include <rtems/score/armv7m.h>

void _ARMV7M_Set_exception_priority( int vector, int priority )
{
  if (vector >= ARMV7M_VECTOR_IRQ(0)) {
    _ARMV7M_NVIC->ipr [vector - ARMV7M_VECTOR_IRQ(0)] = (uint8_t) priority;
  } else if (vector >= ARMV7M_VECTOR_MEM_MANAGE) {
    _ARMV7M_SCB->shpr [vector - 4] = (uint8_t) priority;
  }
}

#endif /* ARM_MULTILIB_ARCH_V7M */
