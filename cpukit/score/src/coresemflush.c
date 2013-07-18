/**
 *  @file
 *
 *  @brief Core Semaphore Flush
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

void _CORE_semaphore_Flush(
  CORE_semaphore_Control     *the_semaphore,
  Thread_queue_Flush_callout  remote_extract_callout,
  uint32_t                    status
)
{

  _Thread_queue_Flush(
    &the_semaphore->Wait_queue,
    remote_extract_callout,
    status
  );

}
