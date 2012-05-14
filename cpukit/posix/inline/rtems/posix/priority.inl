/**
 * @file rtems/posix/priority.inl
 *
 * This defines the static inline methods related to POSIX priority management.
 */

/*  
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_PRIORITY_H
# error "Never use <rtems/posix/priority.inl> directly; include <rtems/posix/priority.h> instead."
#endif

#ifndef _RTEMS_POSIX_PRIORITY_INL
#define _RTEMS_POSIX_PRIORITY_INL

RTEMS_INLINE_ROUTINE Priority_Control _POSIX_Priority_To_core(
  int priority
)
{
  return (Priority_Control) (POSIX_SCHEDULER_MAXIMUM_PRIORITY - priority + 1);
}

RTEMS_INLINE_ROUTINE int _POSIX_Priority_From_core(
  Priority_Control priority
)
{
  return (POSIX_SCHEDULER_MAXIMUM_PRIORITY - priority + 1);
}

#endif
