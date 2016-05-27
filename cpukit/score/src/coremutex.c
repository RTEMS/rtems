/**
 *  @file
 *
 *  @brief Initialize a Core Mutex
 *  @ingroup ScoreMutex
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/thread.h>

Status_Control _CORE_mutex_Initialize(
  CORE_mutex_Control           *the_mutex,
  Thread_Control               *executing,
  const CORE_mutex_Attributes  *the_mutex_attributes,
  bool                          initially_locked
)
{

/* Add this to the RTEMS environment later ?????????
  rtems_assert( initial_lock == CORE_MUTEX_LOCKED ||
                initial_lock == CORE_MUTEX_UNLOCKED );
 */

  the_mutex->Attributes    = *the_mutex_attributes;

  if ( initially_locked ) {
    the_mutex->nest_count = 1;
    the_mutex->holder     = executing;
    executing->resource_count++;
  } else {
    the_mutex->nest_count = 0;
    the_mutex->holder     = NULL;
  }

  _Thread_queue_Initialize( &the_mutex->Wait_queue );

  return STATUS_SUCCESSFUL;
}
