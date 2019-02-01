/**
 * @file
 *
 * @brief Condition variable Attributes structure
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007, 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>


/*
 *  The default condition variable attributes structure.
 */

const pthread_condattr_t _POSIX_Condition_variables_Default_attributes = {
  .is_initialized = 1,
  .clock          = CLOCK_REALTIME,
#if defined(_POSIX_THREAD_PROCESS_SHARED)
  .process_shared = PTHREAD_PROCESS_PRIVATE
#endif
};
