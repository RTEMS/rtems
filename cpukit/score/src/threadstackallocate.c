/**
 * @file
 * 
 * @brief Stack Allocate Helper
 * @ingroup ScoreThread
 */


/*
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

#include <rtems/score/threadimpl.h>
#include <rtems/score/stackimpl.h>
#include <rtems/config.h>

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
