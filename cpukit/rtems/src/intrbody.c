/*
 *  Bodies for Inlined Interrupt Manager Routines
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/intr.h>

/*
 *  Real body for rtems_interrupt_disable
 */

#undef rtems_interrupt_disable

rtems_interrupt_level rtems_interrupt_disable( void )
{
  rtems_interrupt_level previous_level;

  _ISR_Disable( previous_level );

  return previous_level;
}

/*
 *  Real body for rtems_interrupt_enable
 */

#undef rtems_interrupt_enable

void rtems_interrupt_enable(
  rtems_interrupt_level previous_level
)
{
  _ISR_Enable( previous_level );
}

/*
 *  Real body for rtems_interrupt_flash
 */

#undef rtems_interrupt_flash

void rtems_interrupt_flash(
  rtems_interrupt_level previous_level
)
{
  _ISR_Flash( previous_level );
}

/*
 *  Real body for rtems_interrupt_is_in_progress
 */

#undef rtems_interrupt_is_in_progress

bool rtems_interrupt_is_in_progress( void )
{
  return _ISR_Is_in_progress();
}
