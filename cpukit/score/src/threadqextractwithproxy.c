/**
 * @file
 *
 * @ingroup RTEMSScoreThreadQueue
 *
 * @brief This source file contains the implementation of
 *   _Thread_queue_Extract_with_proxy().
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadqimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/statesimpl.h>

void _Thread_queue_Extract_with_proxy(
  Thread_Control       *the_thread
)
{
  #if defined(RTEMS_MULTIPROCESSING)
    States_Control state;

    state = the_thread->current_state;
    if ( _States_Is_waiting_for_rpc_reply( state ) &&
         _States_Is_locally_blocked( state ) ) {
      Objects_Id                            id;
      Objects_Information                  *the_information;
      Objects_Thread_queue_Extract_callout  proxy_extract_callout;

      id = the_thread->Wait.remote_id;
      the_information = _Objects_Get_information_id( id );
      proxy_extract_callout = the_information->extract;

      if ( proxy_extract_callout != NULL )
        (*proxy_extract_callout)( the_thread, id );
    }
  #endif

  _Thread_queue_Extract( the_thread );
}
