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
#include <rtems/config.h>

/*PAGE
 *
 *  _Thread_Stack_Allocate
 *
 *  Allocate the requested stack space for the thread.
 *  return the actual size allocated after any adjustment
 *  or return zero if the allocation failed.
 *  Set the Start.stack field to the address of the stack
 */

size_t _Thread_Stack_Allocate(
  Thread_Control *the_thread,
  size_t          stack_size
)
{
  void *stack_addr = 0;
  size_t the_stack_size = stack_size;

  the_stack_size = _Stack_Ensure_minimum( stack_size );

  /*
   * Call ONLY the CPU table stack allocate hook, _or_ the
   * the RTEMS workspace allocate.  This is so the stack free
   * routine can call the correct deallocation routine.
   */

  if ( Configuration.stack_allocate_hook ) {
    stack_addr = (*Configuration.stack_allocate_hook)( the_stack_size );
  } else {

    /*
     *  First pad the requested size so we allocate enough memory
     *  so the context initialization can align it properly.  The address
     *  returned the workspace allocate must be directly stored in the
     *  stack control block because it is later used in the free sequence.
     *
     *  Thus it is the responsibility of the CPU dependent code to
     *  get and keep the stack adjust factor, the stack alignment, and
     *  the context initialization sequence in sync.
     */

    the_stack_size = _Stack_Adjust_size( the_stack_size );
    stack_addr = _Workspace_Allocate( the_stack_size );
  }

  if ( !stack_addr )
    the_stack_size = 0;

  the_thread->Start.stack = stack_addr;

  return the_stack_size;
}
