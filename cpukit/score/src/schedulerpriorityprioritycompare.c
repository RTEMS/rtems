/*
 *  COPYRIGHT (c) 2011.
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
#include <rtems/config.h>
#include <rtems/score/chain.h>
#include <rtems/score/schedulerpriority.h>

int _Scheduler_priority_Priority_compare(
  Priority_Control      p1,
  Priority_Control      p2
)
{
  return _Scheduler_priority_Priority_compare_body( p1, p2 );
}
