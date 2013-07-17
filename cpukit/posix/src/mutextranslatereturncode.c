/**
 *  @file
 *
 *  @brief POSIX Mutex Translate Core Mutex Return Code
 *  @ingroup POSIX_MUTEX
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/muteximpl.h>

const int _POSIX_Mutex_Return_codes[CORE_MUTEX_STATUS_LAST + 1] = {
  0,                      /* CORE_MUTEX_STATUS_SUCCESSFUL */
  EBUSY,                  /* CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT */
  EDEADLK,                /* CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED */
  EPERM,                  /* CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE */
  EINVAL,                 /* CORE_MUTEX_WAS_DELETED */
  ETIMEDOUT,              /* CORE_MUTEX_TIMEOUT */
#ifdef __RTEMS_STRICT_ORDER_MUTEX__
  EDEADLK,                /* CORE_MUTEX_RELEASE_NOT_ORDER */
#endif
  EINVAL                  /* CORE_MUTEX_STATUS_CEILING_VIOLATED */
};
