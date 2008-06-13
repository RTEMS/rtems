/*
 *  Body for Attribute Routines
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/stack.h>
#include <rtems/rtems/modes.h>

uint32_t   rtems_interrupt_mask = RTEMS_INTERRUPT_MASK;

rtems_attribute rtems_interrupt_level_attribute(
  uint32_t   level
)
{
  return RTEMS_INTERRUPT_LEVEL(level);
}
