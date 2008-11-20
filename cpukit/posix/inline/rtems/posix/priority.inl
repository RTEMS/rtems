/**
 * @file rtems/posix/priority.inl
 */

/*  
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_POSIX_PRIORITY_H
# error "Never use <rtems/posix/priority.inl> directly; include <rtems/posix/priority.h> instead."
#endif

#ifndef _RTEMS_POSIX_PRIORITY_INL
#define _RTEMS_POSIX_PRIORITY_INL

/*
 *  1003.1b-1993,2.2.2.80 definition of priority, p. 19
 *
 *  "Numerically higher values represent higher priorities."
 *
 *  Thus, RTEMS Core has priorities run in the opposite sense of the POSIX API.
 */

RTEMS_INLINE_ROUTINE bool _POSIX_Priority_Is_valid(
  int priority
)
{
  return ((priority >= POSIX_SCHEDULER_MINIMUM_PRIORITY) &&
          (priority <= POSIX_SCHEDULER_MAXIMUM_PRIORITY));

}

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
