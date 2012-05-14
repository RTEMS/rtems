/*
 *  Thread Handler - Stack Allocate Helper
 *
 *  COPYRIGHT (c) 1989-2010.
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
  size_t the_stack_size;
  rtems_stack_allocate_hook stack_allocate_hook =
    rtems_configuration_get_stack_allocate_hook();

  the_stack_size = _Stack_Ensure_minimum( stack_size );

  stack_addr = (*stack_allocate_hook)( the_stack_size );

  if ( !stack_addr )
    the_stack_size = 0;

  the_thread->Start.stack = stack_addr;

  return the_stack_size;
}
