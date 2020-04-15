/**
 *  @file
 *
 *  @brief ARMV7M Set Exception Handler
 */

/*
 * Copyright (c) 2011, 2016 Sebastian Huber.  All rights reserved.
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
#include <rtems/rtems/cache.h>

#ifdef ARM_MULTILIB_ARCH_V7M

void _ARMV7M_Set_exception_handler(
  int index,
  ARMV7M_Exception_handler handler
)
{
  if ( _ARMV7M_SCB->vtor [index] != handler ) {
    _ARMV7M_SCB->vtor [index] = handler;
    rtems_cache_flush_multiple_data_lines(
      &_ARMV7M_SCB->vtor [index],
      sizeof(_ARMV7M_SCB->vtor [index])
    );
    rtems_cache_invalidate_multiple_instruction_lines(
      &_ARMV7M_SCB->vtor [index],
      sizeof(_ARMV7M_SCB->vtor [index])
    );
  }
}

#endif /* ARM_MULTILIB_ARCH_V7M */
