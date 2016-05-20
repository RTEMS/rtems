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

#include <rtems/rtems/semimpl.h>
#include <rtems/rtems/attrimpl.h>

rtems_status_code rtems_semaphore_delete(
  rtems_id   id
)
{
  Semaphore_Control *the_semaphore;
  ISR_lock_Context   lock_context;
  rtems_attribute    attribute_set;

  _Objects_Allocator_lock();
  the_semaphore = _Semaphore_Get( id, &lock_context );

  if ( the_semaphore == NULL ) {
    _Objects_Allocator_unlock();

#if defined(RTEMS_MULTIPROCESSING)
    if ( _Semaphore_MP_Is_remote( id ) ) {
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
    }
#endif

    return RTEMS_INVALID_ID;
  }

  attribute_set = the_semaphore->attribute_set;

#if defined(RTEMS_SMP)
  if ( _Attributes_Is_multiprocessor_resource_sharing( attribute_set ) ) {
    MRSP_Status mrsp_status;

    _MRSP_Acquire_critical(
      &the_semaphore->Core_control.mrsp,
      &lock_context
    );
    mrsp_status = _MRSP_Can_destroy( &the_semaphore->Core_control.mrsp );
    if ( mrsp_status != MRSP_SUCCESSFUL ) {
      _MRSP_Release(
        &the_semaphore->Core_control.mrsp,
        &lock_context
      );
      _Objects_Allocator_unlock();
      return _Semaphore_Translate_MRSP_status_code( mrsp_status );
    }
  } else
#endif
  if ( !_Attributes_Is_counting_semaphore( attribute_set ) ) {
    _CORE_mutex_Acquire_critical(
      &the_semaphore->Core_control.mutex,
      &lock_context
    );

    if (
      _CORE_mutex_Is_locked( &the_semaphore->Core_control.mutex )
        && !_Attributes_Is_simple_binary_semaphore( attribute_set )
    ) {
      _CORE_mutex_Release(
        &the_semaphore->Core_control.mutex,
        &lock_context
      );
      _Objects_Allocator_unlock();
      return RTEMS_RESOURCE_IN_USE;
    }
  } else {
    _CORE_semaphore_Acquire_critical(
      &the_semaphore->Core_control.semaphore,
      &lock_context
    );
  }

  _Objects_Close( &_Semaphore_Information, &the_semaphore->Object );

#if defined(RTEMS_SMP)
  if ( _Attributes_Is_multiprocessor_resource_sharing( attribute_set ) ) {
    _MRSP_Destroy( &the_semaphore->Core_control.mrsp, &lock_context );
  } else
#endif
  if ( !_Attributes_Is_counting_semaphore( attribute_set ) ) {
    _CORE_mutex_Flush(
      &the_semaphore->Core_control.mutex,
      _CORE_mutex_Was_deleted,
      _Semaphore_MP_Send_object_was_deleted,
      id,
      &lock_context
    );
    _CORE_mutex_Destroy( &the_semaphore->Core_control.mutex );
  } else {
    _CORE_semaphore_Destroy(
      &the_semaphore->Core_control.semaphore,
      _Semaphore_MP_Send_object_was_deleted,
      id,
      &lock_context
    );
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Attributes_Is_global( attribute_set ) ) {

    _Objects_MP_Close( &_Semaphore_Information, id );

    _Semaphore_MP_Send_process_packet(
      SEMAPHORE_MP_ANNOUNCE_DELETE,
      id,
      0,                         /* Not used */
      0                          /* Not used */
    );
  }
#endif

  _Semaphore_Free( the_semaphore );
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}
