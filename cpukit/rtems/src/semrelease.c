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
#include <rtems/rtems/statusimpl.h>

rtems_status_code rtems_semaphore_release( rtems_id id )
{
  Semaphore_Control    *the_semaphore;
  Thread_queue_Context  queue_context;
  rtems_attribute       attribute_set;
  Status_Control        status;

  the_semaphore = _Semaphore_Get( id, &queue_context );

  if ( the_semaphore == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Semaphore_MP_Release( id );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  _Thread_queue_Context_set_MP_callout(
    &queue_context,
    _Semaphore_Core_mutex_mp_support
  );

  attribute_set = the_semaphore->attribute_set;
#if defined(RTEMS_SMP)
  if ( _Attributes_Is_multiprocessor_resource_sharing( attribute_set ) ) {
    status = _MRSP_Surrender(
      &the_semaphore->Core_control.mrsp,
      _Thread_Executing,
      &queue_context
    );
  } else
#endif
  if ( !_Attributes_Is_counting_semaphore( attribute_set ) ) {
    status = _CORE_mutex_Surrender(
      &the_semaphore->Core_control.mutex,
      &queue_context
    );
  } else {
    status = _CORE_semaphore_Surrender(
      &the_semaphore->Core_control.semaphore,
      UINT32_MAX,
      &queue_context
    );
  }

  return _Status_Get( status );
}
