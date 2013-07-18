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
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>

#include <pthread.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>

#include <rtems/system.h>
#include <rtems/score/watchdog.h>
#include <rtems/seterr.h>
#include <rtems/posix/mqueueimpl.h>
#include <rtems/posix/time.h>

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
    true,
    THREAD_QUEUE_WAIT_FOREVER
  );
}
