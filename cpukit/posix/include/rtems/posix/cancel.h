/**
 * @file
 * 
 * @brief POSIX Thread Cancelation Support
 *
 * This file contains the prototypes and data types used to implement
 * POSIX thread cancelation.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_CANCEL_H
#define _RTEMS_POSIX_CANCEL_H

#include <rtems/posix/threadsup.h>

#ifndef HAVE_STRUCT__PTHREAD_CLEANUP_CONTEXT
/**
 * This structure is used to manage the cancelation handlers.
 */
typedef struct {
  /** This field is the Chain Node so we can put these on lists. */
  Chain_Node  Node;
  /** This field is the cancelation routine. */
  void      (*routine)( void * );
  /** This field is the argument to the cancelation routine. */
  void       *arg;
}  POSIX_Cancel_Handler_control;
#endif /* HAVE_STRUCT__PTHREAD_CLEANUP_CONTEXT */

/**
 * @brief POSIX run thread cancelation.
 *
 * This support routine runs through the chain of cancel handlers that
 * have been registered and executes them.
 *
 * @param[in] the_thread is a pointer to the thread whose cancelation handlers
 *            should be run
 */
void _POSIX_Threads_cancel_run(
  Thread_Control *the_thread
);

/**
 * @brief POSIX evaluate thread cancelation and enable dispatch. 
 *
 * This routine separates a piece of code that existed as part of
 * another routine, but had to be separated to improve coverage.
 *
 * @param[in] the_thread is a pointer to the thread to evaluate canceling
 */
void _POSIX_Thread_Evaluate_cancellation_and_enable_dispatch (
  Thread_Control *the_thread
);

#endif
/* end of include file */
