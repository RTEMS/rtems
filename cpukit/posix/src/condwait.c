/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Waiting on a Condition
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

#include <rtems/posix/condimpl.h>

#include <string.h>

bool _POSIX_Condition_variables_Auto_initialization(
  POSIX_Condition_variables_Control *the_cond
)
{
  POSIX_Condition_variables_Control zero;
  unsigned long                     flags;

  memset( &zero, 0, sizeof( zero ) );

  if ( memcmp( the_cond, &zero, sizeof( *the_cond ) ) != 0 ) {
    return false;
  }

  flags = (uintptr_t) the_cond ^ POSIX_CONDITION_VARIABLES_MAGIC;
  flags &= ~POSIX_CONDITION_VARIABLES_FLAGS_MASK;
  the_cond->flags = flags;
  return true;
}

/*
 *  11.4.4 Waiting on a Condition, P1003.1c/Draft 10, p. 105
 */

int pthread_cond_wait(
  pthread_cond_t     *cond,
  pthread_mutex_t    *mutex
)
{
  return _POSIX_Condition_variables_Wait_support(
    cond,
    mutex,
    NULL
  );
}
