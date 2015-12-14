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
