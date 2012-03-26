/*
 * Copyright (c) 2012 Sebastian Huber.  All rights reserved.
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

void _ARMV7M_Set_exception_priority_and_handler(
  int index,
  int priority,
  ARMV7M_Exception_handler handler
)
{
  _ARMV7M_Set_exception_priority( index, priority );
  _ARMV7M_Set_exception_handler( index, handler );
}

#endif /* ARM_MULTILIB_ARCH_V7M */
