/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

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
  _Objects_Close( information, &the_thread->Object );
 
  _Thread_Set_state( the_thread, STATES_TRANSIENT );
 
  if ( !_Thread_queue_Extract_with_proxy( the_thread ) ) {
    if ( _Watchdog_Is_active( &the_thread->Timer ) )
      (void) _Watchdog_Remove( &the_thread->Timer );
  }

  _User_extensions_Thread_delete( the_thread );
 
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
  if ( _Thread_Is_allocated_fp( the_thread ) )
    _Thread_Deallocate_fp();
#endif
  the_thread->fp_context = NULL;

  if ( the_thread->Start.fp_context )
    (void) _Workspace_Free( the_thread->Start.fp_context );
#endif

  _Thread_Stack_Free( the_thread );

  if ( the_thread->extensions )
    (void) _Workspace_Free( the_thread->extensions );

  the_thread->Start.stack = NULL;
  the_thread->extensions = NULL;
}
