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

int _ARMV7M_Get_exception_priority( int vector )
{
  if ( _ARMV7M_Is_vector_an_irq( vector ) ) {
    return _ARMV7M_NVIC_Get_priority( ARMV7M_IRQ_OF_VECTOR( vector ) );
  } else if ( vector >= ARMV7M_VECTOR_MEM_MANAGE ) {
    return _ARMV7M_SCB->shpr [vector - 4];
  } else {
    return vector - 4;
  }
}

#endif /* ARM_MULTILIB_ARCH_V7M */
