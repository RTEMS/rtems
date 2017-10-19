/**
 *  @file
 *
 *  @brief Receive a Message From a Message Queue 
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/mqueueimpl.h>

ssize_t mq_receive(
  mqd_t         mqdes,
  char         *msg_ptr,
  size_t        msg_len,
  unsigned int *msg_prio
)
{
  return _POSIX_Message_queue_Receive_support(
    mqdes,
    msg_ptr,
    msg_len,
    msg_prio,
    NULL,
    _Thread_queue_Enqueue_do_nothing_extra
  );
}
