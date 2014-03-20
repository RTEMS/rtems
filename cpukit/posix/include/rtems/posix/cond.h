/**
 * @file
 * 
 * @brief POSIX Condition Variables Private Support
 *
 * This include file contains all the private support information for
 * POSIX condition variables.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_COND_H
#define _RTEMS_POSIX_COND_H

#include <rtems/score/object.h>
#include <rtems/score/threadq.h>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup POSIX_COND_VARS POSIX Condition Variables
 *
 * @ingroup POSIXAPI
 * 
 */
/**@{**/

/*
 *  Data Structure used to manage a POSIX condition variable
 */

typedef struct {
   Objects_Control       Object;
   int                   process_shared;
   pthread_mutex_t       Mutex;
   Thread_queue_Control  Wait_queue;
}  POSIX_Condition_variables_Control;

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/*  end of include file */
