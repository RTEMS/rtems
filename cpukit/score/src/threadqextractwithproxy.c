/*
 *  Thread Queue Handler
 *
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tqdata.h>

/*
 *  _Thread_queue_Extract_with_proxy
 *
 *  This routine extracts the_thread from the_thread_queue
 *  and ensures that if there is a proxy for this task on
 *  another node, it is also dealt with.
 *
 *  XXX
 */

bool _Thread_queue_Extract_with_proxy(
  Thread_Control       *the_thread
)
{
  States_Control                state;

  state = the_thread->current_state;

  if ( _States_Is_waiting_on_thread_queue( state ) ) {
    #if defined(RTEMS_MULTIPROCESSING)
      if ( _States_Is_waiting_for_rpc_reply( state ) &&
           _States_Is_locally_blocked( state ) ) {
        Objects_Information                  *the_information;
        Objects_Thread_queue_Extract_callout  proxy_extract_callout;

        the_information = _Objects_Get_information_id( the_thread->Wait.id );
        proxy_extract_callout =
          (Objects_Thread_queue_Extract_callout) the_information->extract;

        if ( proxy_extract_callout )
          (*proxy_extract_callout)( the_thread );
      }
    #endif
    _Thread_queue_Extract( the_thread->Wait.queue, the_thread );

    return true;
  }
  return false;
}
