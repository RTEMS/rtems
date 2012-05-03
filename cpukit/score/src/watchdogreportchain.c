/**
 *  @file watchdogreportchain.c
 *
 *  This should only be used for debugging.
 */

/*  COPYRIGHT (c) 1989-2008.
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
#include <rtems/score/watchdog.h>
#include <rtems/score/isr.h>
#include <rtems/bspIo.h>

void _Watchdog_Report_chain(
  const char        *name,
  Chain_Control     *header
)
{
  ISR_Level          level;
  Chain_Node        *node;

  _ISR_Disable( level );
    printk( "Watchdog Chain: %s %p\n", name, header );
    if ( !_Chain_Is_empty( header ) ) {
      for ( node = _Chain_First( header ) ;
            node != _Chain_Tail(header) ;
            node = node->next )
      {
        Watchdog_Control *watch = (Watchdog_Control *) node;

        _Watchdog_Report( NULL, watch );
      }
      printk( "== end of %s \n", name );
    } else {
      printk( "Chain is empty\n" );
    }
  _ISR_Enable( level );
}
