/*  threadmp.h
 *
 *  This include file contains the specification for all routines
 *  and data specific to the multiprocessing portion of the thread package.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_THREAD_MP_h
#define __RTEMS_THREAD_MP_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  _Thread_MP_Handler_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the multiprocessing portion of the Thread Handler.
 */

void _Thread_MP_Handler_initialization (
  unsigned32 maximum_proxies
);

/*
 *  _Thread_MP_Allocate_proxy
 *
 *  DESCRIPTION:
 *
 *  This  allocates a proxy control block from
 *  the inactive chain of free proxy control blocks.
 *
 *  NOTE: This function returns a thread control pointer
 *        because proxies are substitutes for remote threads.
 */

Thread_Control *_Thread_MP_Allocate_proxy (
  States_Control the_state
);

/*
 *  _Thread_MP_Find_proxy
 *
 *  DESCRIPTION:
 *
 *  This function removes the proxy control block for the specified
 *  id from the active chain of proxy control blocks.
 */

Thread_Control *_Thread_MP_Find_proxy (
  Objects_Id the_id
);

/*
 *  The following is used to determine when the multiprocessing receive
 *  thread is executing so that a proxy can be allocated instead of
 *  blocking the multiprocessing receive thread.
 */

SCORE_EXTERN Thread_Control *_Thread_MP_Receive;

/*
 * The following chains are used to manage proxies.
 */

SCORE_EXTERN Chain_Control _Thread_MP_Active_proxies;
SCORE_EXTERN Chain_Control _Thread_MP_Inactive_proxies;

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/threadmp.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
