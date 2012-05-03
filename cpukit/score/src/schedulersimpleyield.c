/*
 *  Scheduler Simple Handler / Yield
 *
 *  COPYRIGHT (c) 2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/scheduler.h>
#include <rtems/score/thread.h>
#include <rtems/score/schedulersimple.h>

void _Scheduler_simple_Yield( void )
{
  ISR_Level       level;
  Thread_Control *executing;

  executing = _Thread_Executing;
  _ISR_Disable( level );

    _Scheduler_simple_Ready_queue_requeue(&_Scheduler, executing);

    _ISR_Flash( level );

    _Scheduler_simple_Schedule();

    if ( !_Thread_Is_heir( executing ) )
      _Thread_Dispatch_necessary = true;

  _ISR_Enable( level );
}
