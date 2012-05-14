/*
 *  Scheduler Simple SMP Handler / Block
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
#include <rtems/score/schedulersimplesmp.h>

void _Scheduler_simple_smp_Block(
  Thread_Control   *the_thread
)
{
  _Scheduler_simple_Extract( the_thread );

  _Scheduler_simple_smp_Schedule();
}
