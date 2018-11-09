/**
 *  @file
 *
 *  @brief CPU ISR Vector Install
 */

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
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/armv7m.h>
#include <rtems/score/isr.h>

#ifdef ARM_MULTILIB_ARCH_V7M

void _CPU_ISR_install_vector(
  uint32_t         vector,
  CPU_ISR_handler  new_handler,
  CPU_ISR_handler *old_handler
)
{
  uint32_t level;

  _ISR_Local_disable( level );
  if ( old_handler != NULL ) {
    *old_handler = _ARMV7M_Get_exception_handler( (int) vector );
  }
  _ARMV7M_Set_exception_handler( (int) vector, new_handler );
  _ISR_Local_enable( level );
}

#endif /* ARM_MULTILIB_ARCH_V7M */
