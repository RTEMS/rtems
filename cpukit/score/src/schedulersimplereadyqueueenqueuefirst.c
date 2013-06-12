/**
 *  @file
 *
 *  @brief Scheduler Simple Ready Queue Enqueue First
 *  @ingroup ScoreScheduler
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

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/thread.h>
#include <rtems/score/schedulersimple.h>

void _Scheduler_simple_Ready_queue_enqueue_first(
  Thread_Control    *the_thread
)
{
  Chain_Control *ready = (Chain_Control *) _Scheduler.information;

  _Scheduler_simple_Insert_priority_lifo( ready, the_thread );
}
