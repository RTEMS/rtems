/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/time.h>
#include <rtems/seterr.h>

#if defined(RTEMS_MULTIPROCESSING)
/*
 *  _POSIX_Semaphore_MP_Send_process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  process operation can be performed on another node.
 */

void _POSIX_Semaphore_MP_Send_process_packet(
  POSIX_Semaphore_MP_Remote_operations  operation,
  Objects_Id                            semaphore_id,
  Objects_Name                          name,
  Objects_Id                            proxy_id
)
{
  POSIX_MP_NOT_IMPLEMENTED();
}

/*
 *  _POSIX_Semaphore_MP_Send_request_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive operation can be initiated on another node.
 */

int _POSIX_Semaphore_MP_Send_request_packet(
  POSIX_Semaphore_MP_Remote_operations  operation,
  Objects_Id                            semaphore_id,
  boolean                               wait,  /* XXX options */
  Watchdog_Interval                     timeout
)
{
  POSIX_MP_NOT_IMPLEMENTED();
  return 0;
}

/*
 *  _POSIX_Semaphore_MP_Send_response_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs a remote procedure call so that a
 *  directive can be performed on another node.
 */

void _POSIX_Semaphore_MP_Send_response_packet(
  POSIX_Semaphore_MP_Remote_operations  operation,
  Objects_Id                        semaphore_id,
  Thread_Control                   *the_thread
)
{
  POSIX_MP_NOT_IMPLEMENTED();
}

/*
 *
 *  _POSIX_Semaphore_MP_Process_packet
 *
 *  DESCRIPTION:
 *
 *  This routine performs the actions specific to this package for
 *  the request from another node.
 */

void _POSIX_Semaphore_MP_Process_packet(
  MP_packet_Prefix *the_packet_prefix
)
{
  POSIX_MP_NOT_IMPLEMENTED();
}

/*
 *  _POSIX_Semaphore_MP_Send_object_was_deleted
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked indirectly by the thread queue
 *  when a proxy has been removed from the thread queue and
 *  the remote node must be informed of this.
 */

void _POSIX_Semaphore_MP_Send_object_was_deleted(
  Thread_Control *the_proxy
)
{
  POSIX_MP_NOT_IMPLEMENTED();
}

/*
 *  _POSIX_Semaphore_MP_Send_extract_proxy
 *
 *  DESCRIPTION:
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 */

void _POSIX_Semaphore_MP_Send_extract_proxy(
  Thread_Control *the_thread
)
{
  POSIX_MP_NOT_IMPLEMENTED();
}

/*
 *  _POSIX_Semaphore_MP_Get_packet
 *
 *  DESCRIPTION:
 *
 *  This function is used to obtain a semaphore mp packet.
 */

POSIX_Semaphore_MP_Packet *_POSIX_Semaphore_MP_Get_packet( void )
{
  POSIX_MP_NOT_IMPLEMENTED();
  return NULL;
}

#endif /* endif RTEMS_MULTIPROCESSING */
