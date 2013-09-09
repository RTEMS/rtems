/**
 *  @file  rtems/score/threadmp.h
 *
 *  @brief Multiprocessing Portion of the Thread Package
 *
 *  This include file contains the specification for all routines
 *  and data specific to the multiprocessing portion of the thread package.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREADMP_H
#define _RTEMS_SCORE_THREADMP_H

#ifndef _RTEMS_SCORE_THREADIMPL_H
# error "Never use <rtems/score/threadmp.h> directly; include <rtems/score/threadimpl.h> instead."
#endif

#include <rtems/score/mpciimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ScoreThreadMP Thread Handler Multiprocessing Support
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which is related to managing
 *  threads in a multiprocessor system configuration.  This handler must
 *  manage proxies which represent remote threads blocking on local
 *  operations.
 */
/**@{*/

/**
 *  @brief Initialize MP thread handler.
 *
 *  This routine initializes the multiprocessing portion of the Thread Handler.
 */
void _Thread_MP_Handler_initialization (
  uint32_t   maximum_proxies
);

/**
 *  @brief Allocate a MP proxy control block from
 *  the inactive chain of free proxy control blocks.
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

/**
 *  @brief Removes the MP proxy control block for the specified
 *  id from the active chain of proxy control blocks.
 *
 *  This function removes the proxy control block for the specified
 *  id from the active chain of proxy control blocks.
 */
Thread_Control *_Thread_MP_Find_proxy (
  Objects_Id the_id
);

/**
 *  @brief Manage the active set MP proxies.
 *
 * The following chain is used to manage the active set proxies.
 */
SCORE_EXTERN Chain_Control _Thread_MP_Active_proxies;

/**
 *  @brief Manage the inactive set of MP proxies.
 *
 * The following chain is used to manage the inactive set of proxies.
 */
SCORE_EXTERN Chain_Control _Thread_MP_Inactive_proxies;

/**
 * This function returns true if the thread in question is the
 * multiprocessing receive thread.
 *
 * @note This is a macro to avoid needing a prototype for
 *       _MPCI_Receive_server_tcb until it is used.
 */
#define _Thread_MP_Is_receive(_the_thread) \
  ((_the_thread) == _MPCI_Receive_server_tcb)

/**
 * This routine frees a proxy control block to the
 * inactive chain of free proxy control blocks.
 */

RTEMS_INLINE_ROUTINE void _Thread_MP_Free_proxy (
  Thread_Control *the_thread
)
{
  Thread_Proxy_control *the_proxy;

  the_proxy = (Thread_Proxy_control *) the_thread;

  _Chain_Extract( &the_proxy->Active );

  _Chain_Append( &_Thread_MP_Inactive_proxies, &the_thread->Object.Node );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
