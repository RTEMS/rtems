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

/**
 *  11.4.2 Initializing and Destroying a Condition Variable,
 *         P1003.1c/Draft 10, p. 87
 */
int pthread_cond_init(
  pthread_cond_t           *cond,
  const pthread_condattr_t *attr
)
{
  POSIX_Condition_variables_Control   *the_cond;
  const pthread_condattr_t            *the_attr;

  if ( attr ) the_attr = attr;
  else        the_attr = &_POSIX_Condition_variables_Default_attributes;

  /*
   *  Be careful about attributes when global!!!
   */
  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED )
    return EINVAL;

  if ( !the_attr->is_initialized )
    return EINVAL;

  the_cond = _POSIX_Condition_variables_Allocate();

  if ( !the_cond ) {
    _Objects_Allocator_unlock();
    return ENOMEM;
  }

  _POSIX_Condition_variables_Initialize( the_cond );

  _Objects_Open_u32(
    &_POSIX_Condition_variables_Information,
    &the_cond->Object,
    0
  );

  *cond = the_cond->Object.id;

  _Objects_Allocator_unlock();

  return 0;
}
