/**
 * @file rtems/posix/cancel.h
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
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_CANCEL_H
#define _RTEMS_POSIX_CANCEL_H

#include <rtems/posix/threadsup.h>

/**
 *  This structure is used to manage the cancelation handlers.
 */
typedef struct {
  /** This field is the Chain Node so we can put these on lists. */
  Chain_Node  Node;
  /** This field is the cancelation routine. */
  void      (*routine)( void * );
  /** This field is the argument to the cancelation routine. */
  void       *arg;
}  POSIX_Cancel_Handler_control;

/**
 *  @brief _POSIX_Threads_cancel_run
 *
 *  This support routine runs through the chain of cancel handlers that
 *  have been registered and executes them.
 *
 *  @param[in] the_thread is the thread whose cancelation handlers
 *             should be run
 */
void _POSIX_Threads_cancel_run(
  Thread_Control *the_thread
);

/**
 *  @brief _POSIX_Thread_Evaluate_cancellation_and_enable_dispatch
 *
 *  This routine separates a piece of code that existed as part of
 *  another routine, but had to be separated to improve coverage.
 *
 *  @param[in] the_thread is the thread to evaluate canceling
 */
void _POSIX_Thread_Evaluate_cancellation_and_enable_dispatch (
  Thread_Control *the_thread
);

#endif
/* end of include file */
