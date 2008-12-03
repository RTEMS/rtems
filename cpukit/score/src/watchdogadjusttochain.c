/**
 *  @file watchdogadjusttochain.c
 *
 *  This is used by the Timer Server task.
 */

/*  COPYRIGHT (c) 1989-2008.
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
#include <rtems/score/isr.h>
#include <rtems/score/watchdog.h>

void _Watchdog_Adjust_to_chain(
  Chain_Control               *header,
  Watchdog_Interval            units_arg,
  Chain_Control               *to_fire

)
{
  Watchdog_Interval  units = units_arg;
  ISR_Level          level;
  Chain_Node        *node;

  if ( !units ) {
    return;
  }
  _ISR_Disable( level );

  if ( !_Chain_Is_empty( header ) ) {
    while ( units ) {
      if ( units < _Watchdog_First( header )->delta_interval ) {
	_Watchdog_First( header )->delta_interval -= units;
	break;
      } else {
	units -= _Watchdog_First( header )->delta_interval;
	_Watchdog_First( header )->delta_interval = 0;

        do {
          node = _Chain_Get_unprotected( header );
          _Chain_Append_unprotected( to_fire, node );

	  _ISR_Flash( level );

        } while ( !_Chain_Is_empty( header ) &&
                  _Watchdog_First( header )->delta_interval == 0 );

	if ( _Chain_Is_empty( header ) )
	  break;
      }
    }

  }
  _ISR_Enable( level );
}

