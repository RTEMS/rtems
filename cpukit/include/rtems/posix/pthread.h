/**
 * @file
 *
 * @brief POSIX Threads Private Support
 *
 * This include file contains all the private support information for
 * POSIX threads.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_PTHREAD_H
#define _RTEMS_POSIX_PTHREAD_H

#include <rtems/posix/threadsup.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIX_PTHREAD POSIX Threads Support
 *
 * @ingroup POSIXAPI
 *
 * @brief Private Support Information for POSIX Threads
 *
 */
/**@{**/

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

extern const size_t _POSIX_Threads_Minimum_stack_size;

/**
 * @brief Initialization table for the first user POSIX thread.
 *
 * This table is used by _POSIX_Threads_Initialize_user_thread() and
 * initialized via <rtems/confdefs.h>.
 */
extern const posix_initialization_threads_table
  _POSIX_Threads_User_thread_table;

/**
 * @brief System initialization handler to create the first user POSIX thread.
 */
extern void _POSIX_Threads_Initialize_user_thread( void );

/**
 * The following defines the information control block used to manage
 * this class of objects.
 */
extern Thread_Information _POSIX_Threads_Information;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
