/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/mutex.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  The default mutex attributes structure.
 */

const pthread_mutexattr_t _POSIX_Mutex_Default_attributes = {
  TRUE,                                    /* is_initialized */
  PTHREAD_PROCESS_PRIVATE,                 /* process_shared */
  POSIX_SCHEDULER_MAXIMUM_PRIORITY,        /* prio_ceiling   */
  PTHREAD_PRIO_NONE,                       /* protocol       */
  FALSE                                    /* recursive      */
};
