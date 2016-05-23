/**
 * @file
 *
 * @brief RTEMS Semaphore Release
 * @ingroup ClassicSem Semaphores
 *
 * This file contains the implementation of the Classic API directive
 * rtems_semaphore_release().
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

rtems_status_code rtems_semaphore_release( rtems_id id )
{
  Semaphore_Control     *the_semaphore;
  CORE_mutex_Status      mutex_status;
  CORE_semaphore_Status  semaphore_status;
  rtems_attribute        attribute_set;
  ISR_lock_Context       lock_context;

  the_semaphore = _Semaphore_Get( id, &lock_context );

  if ( the_semaphore == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Semaphore_MP_Release( id );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  attribute_set = the_semaphore->attribute_set;
#if defined(RTEMS_SMP)
  if ( _Attributes_Is_multiprocessor_resource_sharing( attribute_set ) ) {
    MRSP_Status mrsp_status;

    mrsp_status = _MRSP_Surrender(
      &the_semaphore->Core_control.mrsp,
      _Thread_Executing,
      &lock_context
    );
    return _Semaphore_Translate_MRSP_status_code( mrsp_status );
  } else
#endif
  if ( !_Attributes_Is_counting_semaphore( attribute_set ) ) {
    mutex_status = _CORE_mutex_Surrender(
      &the_semaphore->Core_control.mutex,
      _Semaphore_Core_mutex_mp_support,
      &lock_context
    );
    return _Semaphore_Translate_core_mutex_return_code( mutex_status );
  } else {
    semaphore_status = _CORE_semaphore_Surrender(
      &the_semaphore->Core_control.semaphore,
      _Semaphore_Core_mutex_mp_support,
      &lock_context
    );
    return _Semaphore_Translate_core_semaphore_return_code( semaphore_status );
  }
}
