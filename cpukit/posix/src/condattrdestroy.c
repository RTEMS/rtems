/**
 * @file
 *
 * @brief Destroy Condition Attribute
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/condimpl.h>
#include <rtems/posix/time.h>
#include <rtems/posix/muteximpl.h>

/*
 *  11.4.1 Condition Variable Initialization Attributes,
 *            P1003.1c/Draft 10, p. 96
 */

int pthread_condattr_destroy(
  pthread_condattr_t *attr
)
{
  if ( !attr || attr->is_initialized == false )
    return EINVAL;

  attr->is_initialized = false;
  return 0;
}
