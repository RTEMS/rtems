/**
 *  @file
 *
 *  @brief RTEMS Delete Semaphore
 *  @ingroup ClassicSem
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/semimpl.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/coresemimpl.h>
#include <rtems/score/thread.h>

#include <rtems/score/interr.h>

#if defined(RTEMS_MULTIPROCESSING)
#define SEMAPHORE_MP_OBJECT_WAS_DELETED _Semaphore_MP_Send_object_was_deleted
#else
#define SEMAPHORE_MP_OBJECT_WAS_DELETED NULL
#endif

rtems_status_code rtems_semaphore_delete(
  rtems_id   id
)
{
  Semaphore_Control          *the_semaphore;
  Objects_Locations           location;
  rtems_attribute             attribute_set;

  _Objects_Allocator_lock();

  the_semaphore = _Semaphore_Get( id, &location );
  switch ( location ) {

    case OBJECTS_LOCAL:
      attribute_set = the_semaphore->attribute_set;
#if defined(RTEMS_SMP)
      if ( _Attributes_Is_multiprocessor_resource_sharing( attribute_set ) ) {
        MRSP_Status mrsp_status = _MRSP_Destroy(
          &the_semaphore->Core_control.mrsp
        );
        if ( mrsp_status != MRSP_SUCCESSFUL ) {
          _Objects_Put( &the_semaphore->Object );
          _Objects_Allocator_unlock();
          return _Semaphore_Translate_MRSP_status_code( mrsp_status );
        }
      } else
#endif
      if ( !_Attributes_Is_counting_semaphore( attribute_set ) ) {
        if ( _CORE_mutex_Is_locked( &the_semaphore->Core_control.mutex ) &&
             !_Attributes_Is_simple_binary_semaphore( attribute_set ) ) {
          _Objects_Put( &the_semaphore->Object );
          _Objects_Allocator_unlock();
          return RTEMS_RESOURCE_IN_USE;
        }
        _CORE_mutex_Flush(
          &the_semaphore->Core_control.mutex,
          SEMAPHORE_MP_OBJECT_WAS_DELETED,
          CORE_MUTEX_WAS_DELETED
        );
      } else {
        _CORE_semaphore_Flush(
          &the_semaphore->Core_control.semaphore,
          SEMAPHORE_MP_OBJECT_WAS_DELETED,
          CORE_SEMAPHORE_WAS_DELETED
        );
      }

      _Objects_Close( &_Semaphore_Information, &the_semaphore->Object );

#if defined(RTEMS_MULTIPROCESSING)
      if ( _Attributes_Is_global( attribute_set ) ) {

        _Objects_MP_Close( &_Semaphore_Information, the_semaphore->Object.id );

        _Semaphore_MP_Send_process_packet(
          SEMAPHORE_MP_ANNOUNCE_DELETE,
          the_semaphore->Object.id,
          0,                         /* Not used */
          0                          /* Not used */
        );
      }
#endif

      _Objects_Put( &the_semaphore->Object );
      _Semaphore_Free( the_semaphore );
      _Objects_Allocator_unlock();
      return RTEMS_SUCCESSFUL;

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      _Objects_Allocator_unlock();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      break;
  }

  _Objects_Allocator_unlock();
  return RTEMS_INVALID_ID;
}
