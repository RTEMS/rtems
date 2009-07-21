/**
 * @file rtems/posix/cancel.h
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_POSIX_CANCEL_H
#define _RTEMS_POSIX_CANCEL_H

#include <rtems/posix/threadsup.h>

typedef struct {
  Chain_Node  Node;
  void      (*routine)( void * );
  void       *arg;
}  POSIX_Cancel_Handler_control;

/*
 *  _POSIX_Threads_cancel_run
 *
 *  DESCRIPTION:
 *
 *  This support routine runs through the chain of cancel handlers that
 *  have been registered and executes them.
 */

void _POSIX_Threads_cancel_run(
  Thread_Control *the_thread
);

/*
 *  _POSIX_Thread_Evaluate_cancellation_and_enable_dispatch
 *
 *  DESCRIPTION:
 *  
 *  This routine separates a piece of code that existed as part of 
 *  another routine, but had to be separated to improve coverage.
 */

void _POSIX_Thread_Evaluate_cancellation_and_enable_dispatch (
  POSIX_API_Control *thread_support
);

#endif
/* end of include file */
