/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Initialization of Conditional Attributes
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/posix/condimpl.h>

/*
 *  11.4.1 Condition Variable Initialization Attributes,
 *            P1003.1c/Draft 10, p. 96
 */

int pthread_condattr_init(
  pthread_condattr_t *attr
)
{
  if ( attr == NULL )
    return EINVAL;

  *attr = _POSIX_Condition_variables_Default_attributes;
  return 0;
}
