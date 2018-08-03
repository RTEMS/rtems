/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief BSP interrupt support lock implementation.
 */

/*
 * Based on concepts of Pavel Pisa, Till Straumann and Eric Valette.
 *
 * Copyright (c) 2008, 2018 embedded brains GmbH.
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

#include <bsp/irq-generic.h>

#include <rtems/score/apimutex.h>
#include <rtems/score/sysstate.h>

void bsp_interrupt_lock(void)
{
  if (_System_state_Is_up(_System_state_Get())) {
    _RTEMS_Lock_allocator();
  }
}

void bsp_interrupt_unlock(void)
{
  if (_System_state_Is_up(_System_state_Get())) {
    _RTEMS_Unlock_allocator();
  }
}
