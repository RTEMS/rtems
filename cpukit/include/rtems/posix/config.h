/**
 * @file
 *
 * @brief User Defined Configuration Parameters Specific For The POSIX API
 * 
 * This include file contains the table of user defined configuration
 * parameters specific for the POSIX API.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_CONFIG_H
#define _RTEMS_POSIX_CONFIG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup PosixConfig Configuration
 *
 *  @ingroup POSIXAPI
 *
 *  This encapsulates functionality related to the application's configuration
 *  of the Classic API including the maximum number of each class of objects.
 */
/**@{*/

/**
 *  For now, we are only allowing the user to specify the entry point
 *  and stack size for POSIX initialization threads.
 */
typedef struct {
  /** This is the entry point for a POSIX initialization thread. */
  void       *(*thread_entry)(void *);
  /** This is the stack size for a POSIX initialization thread. */
  int       stack_size;
} posix_initialization_threads_table;

extern posix_initialization_threads_table * const
  _Configuration_POSIX_Initialization_threads;

extern const size_t _Configuration_POSIX_Initialization_thread_count;

/**@}*/
#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
