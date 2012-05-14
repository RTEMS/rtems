/*
 *  Thread Handler
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
#include <rtems/config.h>

/*
 *  _Thread_Stack_Free
 *
 *  Deallocate the Thread's stack.
 */

void _Thread_Stack_Free(
  Thread_Control *the_thread
)
{
  rtems_stack_free_hook stack_free_hook =
    rtems_configuration_get_stack_free_hook();

  #if defined(RTEMS_SCORE_THREAD_ENABLE_USER_PROVIDED_STACK_VIA_API)
    /*
     *  If the API provided the stack space, then don't free it.
     */
    if ( !the_thread->Start.core_allocated_stack )
      return;
  #endif

  /*
   * Call ONLY the CPU table stack free hook, or the
   * the RTEMS workspace free.  This is so the free
   * routine properly matches the allocation of the stack.
   */

  (*stack_free_hook)( the_thread->Start.Initial_stack.area );
}
