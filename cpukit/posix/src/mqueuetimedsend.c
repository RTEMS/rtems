/**
 *  @file
 *
 *  @brief Send a Message to a Message Queue
 *  @ingroup POSIX_MQUEUE
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/mqueueimpl.h>

int mq_timedsend(
  mqd_t                  mqdes,
  const char            *msg_ptr,
  size_t                 msg_len,
  unsigned int           msg_prio,
  const struct timespec *abstime
)
{
  return _POSIX_Message_queue_Send_support(
    mqdes,
    msg_ptr,
    msg_len,
    msg_prio,
    abstime,
    _Thread_queue_Add_timeout_realtime_timespec
  );
}
