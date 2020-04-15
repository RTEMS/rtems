/**
 *  @file
 *
 *  @brief Body for RTEMS_INTERRUPT_LEVEL Macro
 *  @ingroup ClassicModes
 */


/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasks.h>
#include <rtems/score/stack.h>
#include <rtems/rtems/modes.h>

const uint32_t rtems_interrupt_mask = RTEMS_INTERRUPT_MASK;

rtems_mode rtems_interrupt_level_body(
  uint32_t   level
)
{
  return RTEMS_INTERRUPT_LEVEL(level);
}
