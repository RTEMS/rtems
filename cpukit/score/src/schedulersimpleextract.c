/**
 * @file
 *
 * @brief Removes a Thread from the Simple Queue
 *
 * @ingroup ScoreScheduler
 */

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

#include <rtems/score/schedulersimple.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/thread.h>

void _Scheduler_simple_Extract(
  Thread_Control    *the_thread
)
{
  _Chain_Extract_unprotected( &the_thread->Object.Node );
}
