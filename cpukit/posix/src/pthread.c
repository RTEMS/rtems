/**
 * @file
 *
 * @ingroup POSIX_PTHREADS Private Threads
 *
 * @brief Private Support Information for POSIX Threads
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/pthread.h>

const size_t _POSIX_Threads_Minimum_stack_size;

THREAD_INFORMATION_DEFINE_ZERO(
  _POSIX_Threads,
  OBJECTS_POSIX_API,
  OBJECTS_POSIX_THREADS
);
