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
 *  http://www.rtems.org/license/LICENSE.
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
 *  id from the active red-black tree of proxy control blocks.
 */
Thread_Control *_Thread_MP_Find_proxy (
  Objects_Id the_id
);

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
void _Thread_MP_Free_proxy( Thread_Control *the_thread );

RTEMS_INLINE_ROUTINE bool _Thread_MP_Is_remote( Objects_Id id )
{
  Objects_Information *information;

  information = _Thread_Get_objects_information( id );
  if ( information == NULL ) {
    return false;
  }

  return _Objects_MP_Is_remote( id, information );
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
