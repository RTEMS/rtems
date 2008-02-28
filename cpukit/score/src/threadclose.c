/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>

/*
 *  _Thread_Close
 *
 *  DESCRIPTION:
 *
 *  This routine frees all memory associated with the specified
 *  thread and removes it from the local object table so no further
 *  operations on this thread are allowed.
 */

void _Thread_Close(
  Objects_Information  *information,
  Thread_Control       *the_thread
)
{
  /*
   *  We assume the Allocator Mutex is locked when we get here.
   *  This provides sufficient protection to let the user extensions
   *  run but as soon as we get back, we will make the thread
   *  disappear and set a transient state on it.  So we temporarily
   *  unnest dispatching.
   */
  _Thread_Unnest_dispatch();

  _User_extensions_Thread_delete( the_thread );

  _Thread_Disable_dispatch();

  /*
   *  Now we are in a dispatching critical section again and we
   *  can take the thread OUT of the published set.  It is invalid
   *  to use this thread's Id after this call.
   */
  _Objects_Close( information, &the_thread->Object );

  /*
   *  By setting the transient state, the thread will not be considered
   *  for scheduling when we remove any blocking states.
   */
  _Thread_Set_state( the_thread, STATES_TRANSIENT );

  if ( !_Thread_queue_Extract_with_proxy( the_thread ) ) {
    if ( _Watchdog_Is_active( &the_thread->Timer ) )
      (void) _Watchdog_Remove( &the_thread->Timer );
  }

  /*
   *  The thread might have been FP.  So deal with that.
   */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
  if ( _Thread_Is_allocated_fp( the_thread ) )
    _Thread_Deallocate_fp();
#endif
  the_thread->fp_context = NULL;

  if ( the_thread->Start.fp_context )
    (void) _Workspace_Free( the_thread->Start.fp_context );
#endif

  /*
   *  Free the rest of the memory associated with this task
   *  and set the associated pointers to NULL for safety.
   */
  _Thread_Stack_Free( the_thread );
  the_thread->Start.stack = NULL;

  if ( the_thread->extensions )
    (void) _Workspace_Free( the_thread->extensions );
  the_thread->extensions = NULL;
}
