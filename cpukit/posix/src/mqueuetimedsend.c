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

int mq_timedsend(
  mqd_t                  mqdes,
  const char            *msg_ptr,
  size_t                 msg_len,
  unsigned int           msg_prio,
  const struct timespec *abstime
)
{
  Watchdog_Interval                            ticks;
  bool                                         do_wait = true;
  POSIX_Absolute_timeout_conversion_results_t  status;

  /*
   *  POSIX requires that blocking calls with timeouts that take
   *  an absolute timeout must ignore issues with the absolute
   *  time provided if the operation would otherwise succeed.
   *  So we check the abstime provided, and hold on to whether it
   *  is valid or not.  If it isn't correct and in the future,
   *  then we do a polling operation and convert the UNSATISFIED
   *  status into the appropriate error.
   *
   *  If the status is POSIX_ABSOLUTE_TIMEOUT_INVALID,
   *  POSIX_ABSOLUTE_TIMEOUT_IS_IN_PAST, or POSIX_ABSOLUTE_TIMEOUT_IS_NOW,
   *  then we should not wait.
   */
  status = _POSIX_Absolute_timeout_to_ticks( abstime, &ticks );
  if ( status != POSIX_ABSOLUTE_TIMEOUT_IS_IN_FUTURE )
    do_wait = false;

  return _POSIX_Message_queue_Send_support(
    mqdes,
    msg_ptr,
    msg_len,
    msg_prio,
    do_wait,
    ticks
  );
}
