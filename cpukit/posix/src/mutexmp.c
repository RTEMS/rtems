/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <errno.h>
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/watchdog.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/posix/mutex.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/time.h>

/*
 *  TEMPORARY
 */

#if defined(RTEMS_MULTIPROCESSING)
void _POSIX_Mutex_MP_Send_process_packet (
  POSIX_Mutex_MP_Remote_operations  operation,
  Objects_Id                        mutex_id,
  Objects_Name                      name,
  Objects_Id                        proxy_id
)
{
  (void) POSIX_MP_NOT_IMPLEMENTED();
}

void _POSIX_Mutex_MP_Send_object_was_deleted (
  Thread_Control *the_proxy
)
{
  (void) POSIX_MP_NOT_IMPLEMENTED();
}

int _POSIX_Mutex_MP_Send_request_packet (
  POSIX_Mutex_MP_Remote_operations  operation,
  Objects_Id                        mutex_id,
  boolean                           wait,  /* XXX options */
  Watchdog_Interval                 timeout
)
{
  return POSIX_MP_NOT_IMPLEMENTED();
}

void _POSIX_Threads_mutex_MP_support(
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  (void) POSIX_MP_NOT_IMPLEMENTED();   /* XXX: should never get here */
}
#endif

/*
 *  END OF TEMPORARY
 */
