/** 
 *  @file threadmp.h
 *
 *  This include file contains the specification for all routines
 *  and data specific to the multiprocessing portion of the thread package.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_THREAD_MP_h
#define __RTEMS_THREAD_MP_h

/**
 *  @defgroup ScoreThreadMP Thread Handler Multiprocessing Support
 *
 *  This group contains functionality which XXX
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

/** @brief _Thread_MP_Handler_initialization
 *
 *  This routine initializes the multiprocessing portion of the Thread Handler.
 */
void _Thread_MP_Handler_initialization (
  uint32_t   maximum_proxies
);
/** @brief _Thread_MP_Allocate_proxy
 *
 *  This  allocates a proxy control block from
 *  the inactive chain of free proxy control blocks.
 *
 *  @note This function returns a thread control pointer
 *        because proxies are substitutes for remote threads.
 */
Thread_Control *_Thread_MP_Allocate_proxy (
  States_Control the_state
);

/** @brief _Thread_MP_Find_proxy
 *
 *  This function removes the proxy control block for the specified
 *  id from the active chain of proxy control blocks.
 */
Thread_Control *_Thread_MP_Find_proxy (
  Objects_Id the_id
);

/** @brief Pointer to MP Thread Control Block
 *
 *  The following is used to determine when the multiprocessing receive
 *  thread is executing so that a proxy can be allocated instead of
 *  blocking the multiprocessing receive thread.
 */
SCORE_EXTERN Thread_Control *_Thread_MP_Receive;

/* @brief Active Proxy Set
 *
 * The following chain is used to manage the active set proxies.
 */
SCORE_EXTERN Chain_Control _Thread_MP_Active_proxies;

/** @brief Inactive Proxy Set
 *
 * The following chain is used to manage the inactive set of proxies.
 */
SCORE_EXTERN Chain_Control _Thread_MP_Inactive_proxies;

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/threadmp.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
