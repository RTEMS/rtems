/*
 *  Semaphore Manager
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the Semaphore Manager.
 *  This manager utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
 *  Directives provided are:
 *
 *     + create a semaphore
 *     + get an ID of a semaphore
 *     + delete a semaphore
 *     + acquire a semaphore
 *     + release a semaphore
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/attr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/sem.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/coresem.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/score/sysstate.h>

#include <rtems/score/interr.h>

/*PAGE
 *
 *  rtems_semaphore_delete
 *
 *  This directive allows a thread to delete a semaphore specified by
 *  the semaphore id.  The semaphore is freed back to the inactive
 *  semaphore chain.
 *
 *  Input parameters:
 *    id - semaphore id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_semaphore_delete(
  Objects_Id id
)
{
  register Semaphore_Control *the_semaphore;
  Objects_Locations           location;

  the_semaphore = _Semaphore_Get( id, &location );
  switch ( location ) {

    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      if ( _Attributes_Is_binary_semaphore( the_semaphore->attribute_set) ) { 
        if ( _CORE_mutex_Is_locked( &the_semaphore->Core_control.mutex ) ) {
          _Thread_Enable_dispatch();
          return RTEMS_RESOURCE_IN_USE;
        }
        else
          _CORE_mutex_Flush(
            &the_semaphore->Core_control.mutex, 
#if defined(RTEMS_MULTIPROCESSING)
            _Semaphore_MP_Send_object_was_deleted,
#else
            NULL,
#endif
            CORE_MUTEX_WAS_DELETED
          );
      }
      else
        _CORE_semaphore_Flush(
          &the_semaphore->Core_control.semaphore, 
#if defined(RTEMS_MULTIPROCESSING)
          _Semaphore_MP_Send_object_was_deleted,
#else
          NULL,
#endif
          CORE_SEMAPHORE_WAS_DELETED
        );

      _Objects_Close( &_Semaphore_Information, &the_semaphore->Object );

      _Semaphore_Free( the_semaphore );

#if defined(RTEMS_MULTIPROCESSING)
      if ( _Attributes_Is_global( the_semaphore->attribute_set ) ) {

        _Objects_MP_Close( &_Semaphore_Information, the_semaphore->Object.id );

        _Semaphore_MP_Send_process_packet(
          SEMAPHORE_MP_ANNOUNCE_DELETE,
          the_semaphore->Object.id,
          0,                         /* Not used */
          0                          /* Not used */
        );
      }
#endif
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
}
