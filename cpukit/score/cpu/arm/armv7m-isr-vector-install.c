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

#include <rtems/score/isr.h>

#ifdef ARM_MULTILIB_ARCH_V7M

#include <rtems/score/armv7m.h>

void _CPU_ISR_install_vector(
  uint32_t vector,
  proc_ptr new_handler,
  proc_ptr *old_handler
)
{
  uint32_t level;

  _ISR_Disable( level );
  if ( old_handler != NULL ) {
    *old_handler = _ARMV7M_Get_exception_handler( (int) vector );
  }
  _ARMV7M_Set_exception_handler( (int) vector, new_handler );
  _ISR_Enable( level );
}

#endif /* ARM_MULTILIB_ARCH_V7M */
