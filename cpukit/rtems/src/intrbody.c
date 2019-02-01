/**
 *  @file
 *
 *  @brief RTEMS Interrupt Support
 *  @ingroup ClassicINTR
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/status.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/intr.h>

#if !defined(RTEMS_SMP)

/*
 *  Undefine all of these is normally a macro and we want a real body in
 *  the library for other language bindings.
 */
#undef rtems_interrupt_disable
#undef rtems_interrupt_enable
#undef rtems_interrupt_flash

/*
 *  Prototype them to avoid warnings
 */
rtems_interrupt_level rtems_interrupt_disable( void );
void rtems_interrupt_enable( rtems_interrupt_level previous_level );
void rtems_interrupt_flash( rtems_interrupt_level previous_level );

/*
 *  Now define real bodies
 */
rtems_interrupt_level rtems_interrupt_disable( void )
{
  rtems_interrupt_level previous_level;

  _ISR_Local_disable( previous_level );

  return previous_level;
}

void rtems_interrupt_enable(
  rtems_interrupt_level previous_level
)
{
  _ISR_Local_enable( previous_level );
}

void rtems_interrupt_flash(
  rtems_interrupt_level previous_level
)
{
  _ISR_Local_flash( previous_level );
}

#endif /* RTEMS_SMP */

#undef rtems_interrupt_is_in_progress
bool rtems_interrupt_is_in_progress( void );

bool rtems_interrupt_is_in_progress( void )
{
  return _ISR_Is_in_progress();
}
