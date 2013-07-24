/**
 *  @file
 *
 *  @brief Flush Pending Messages
 *  @ingroup ScoreMessageQueue
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
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/coremsgimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

uint32_t   _CORE_message_queue_Flush(
  CORE_message_queue_Control *the_message_queue
)
{
  if ( the_message_queue->number_of_pending_messages != 0 )
    return _CORE_message_queue_Flush_support( the_message_queue );
  else
    return 0;
}
