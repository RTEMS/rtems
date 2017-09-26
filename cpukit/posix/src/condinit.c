/**
 *  @file
 *
 *  @brief Initialize a Condition Variable
 *  @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/condimpl.h>
#include <rtems/posix/posixapi.h>

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Condition_variables_Control, flags )
    == offsetof( pthread_cond_t, _flags ),
  POSIX_CONDITION_VARIABLES_CONTROL_FLAGS
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Condition_variables_Control, mutex )
    == offsetof( pthread_cond_t, _mutex ),
  POSIX_CONDITION_VARIABLES_CONTROL_COUNT
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Condition_variables_Control, Queue )
    == offsetof( pthread_cond_t, _Queue ),
  POSIX_CONDITION_VARIABLES_CONTROL_QUEUE
);

RTEMS_STATIC_ASSERT(
  sizeof( POSIX_Condition_variables_Control ) == sizeof( pthread_cond_t ),
  POSIX_CONDITION_VARIABLES_CONTROL_SIZE
);

/**
 *  11.4.2 Initializing and Destroying a Condition Variable,
 *         P1003.1c/Draft 10, p. 87
 */
int pthread_cond_init(
  pthread_cond_t           *cond,
  const pthread_condattr_t *attr
)
{
  POSIX_Condition_variables_Control *the_cond;

  the_cond = _POSIX_Condition_variables_Get( cond );

  if ( the_cond == NULL ) {
    return EINVAL;
  }

  if ( attr == NULL ) {
    attr = &_POSIX_Condition_variables_Default_attributes;
  }

  if ( !attr->is_initialized ) {
    return EINVAL;
  }

  if ( !_POSIX_Is_valid_pshared( attr->process_shared ) ) {
    return EINVAL;
  }

  _POSIX_Condition_variables_Initialize( the_cond, attr );
  return 0;
}
