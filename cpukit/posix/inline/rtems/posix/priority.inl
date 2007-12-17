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

#ifndef _RTEMS_POSIX_PRIORITY_INL
#define _RTEMS_POSIX_PRIORITY_INL

/*
 *  1003.1b-1993,2.2.2.80 definition of priority, p. 19
 *
 *  "Numerically higher values represent higher priorities."
 *
 *  Thus, RTEMS Core has priorities run in the opposite sense of the POSIX API.
 */

RTEMS_INLINE_ROUTINE boolean _POSIX_Priority_Is_valid(
  int priority
)
{
  return (boolean) (priority >= 1 && priority <= 254);
}

RTEMS_INLINE_ROUTINE Priority_Control _POSIX_Priority_To_core(
  int priority
)
{
  return (Priority_Control) (255 - priority);
}

RTEMS_INLINE_ROUTINE int _POSIX_Priority_From_core(
  Priority_Control priority
)
{
  return 255 - priority;
}

#endif
