/**
 *  @file
 *
 *  @brief Core Semaphore Initialize
 *  @ingroup ScoreSemaphore
 */

/*
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
#include <rtems/score/isr.h>
#include <rtems/score/coresemimpl.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

void _CORE_semaphore_Initialize(
  CORE_semaphore_Control          *the_semaphore,
  const CORE_semaphore_Attributes *the_semaphore_attributes,
  uint32_t                         initial_value
)
{

  the_semaphore->Attributes = *the_semaphore_attributes;
  the_semaphore->count      = initial_value;

  _Thread_queue_Initialize(
    &the_semaphore->Wait_queue,
    _CORE_semaphore_Is_priority( the_semaphore_attributes ) ?
              THREAD_QUEUE_DISCIPLINE_PRIORITY : THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_SEMAPHORE,
    CORE_SEMAPHORE_TIMEOUT
  );
}
