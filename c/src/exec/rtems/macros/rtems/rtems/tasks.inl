/*  tasks.inl
 *
 *  This file contains the macro implementation of all inlined
 *  routines in the with RTEMS Tasks Manager.
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

#ifndef __RTEMS_TASKS_inl
#define __RTEMS_TASKS_inl

#include <rtems/msgmp.h>
#include <rtems/partmp.h>
#include <rtems/regionmp.h>
#include <rtems/semmp.h>

/*PAGE
 *
 *  _RTEMS_tasks_Allocate
 *
 */

#define _RTEMS_tasks_Allocate() \
  (Thread_Control *) _Objects_Allocate( &_RTEMS_tasks_Information )

/*PAGE
 *
 *  _RTEMS_tasks_Free
 *
 */

#define _RTEMS_tasks_Free( _the_task ) \
  _Objects_Free( &_RTEMS_tasks_Information, &(_the_task)->Object )

/*PAGE
 *
 *  _RTEMS_tasks_Cancel_wait
 *
 */

#define _RTEMS_tasks_Cancel_wait( _the_thread ) \
  { \
    States_Control _state; \
    States_Control _remote_state; \
    \
    _state = (_the_thread)->current_state; \
    \
    if ( _States_Is_waiting_on_thread_queue( _state ) ) { \
      if ( _States_Is_waiting_for_rpc_reply( _state ) && \
            _States_Is_locally_blocked( _state ) ) { \
        _remote_state = _States_Clear( \
                       STATES_WAITING_FOR_RPC_REPLY + STATES_TRANSIENT, \
                       _state \
                     ); \
        \
        switch ( _remote_state ) { \
          \
          case STATES_WAITING_FOR_BUFFER: \
            _Partition_MP_Send_extract_proxy( (_the_thread) ); \
            break; \
          case STATES_WAITING_FOR_SEGMENT: \
            _Region_MP_Send_extract_proxy( (_the_thread) ); \
            break; \
          case STATES_WAITING_FOR_SEMAPHORE: \
            _Semaphore_MP_Send_extract_proxy( (_the_thread) ); \
            break; \
          case STATES_WAITING_FOR_MESSAGE: \
            _Message_queue_MP_Send_extract_proxy( (_the_thread) ); \
            break; \
        } \
      } \
      _Thread_queue_Extract( (_the_thread)->Wait.queue, (_the_thread) ); \
    } \
    else if ( _Watchdog_Is_active( &(_the_thread)->Timer ) ) \
      (void) _Watchdog_Remove( &(_the_thread)->Timer ); \
  }

#endif
/* end of include file */
