/*  
 *  $Id$
 */

#ifndef __RTEMS_POSIX_PRIORITY_inl
#define __RTEMS_POSIX_PRIORITY_inl

/*
 *  1003.1b-1993,2.2.2.80 definition of priority, p. 19
 *
 *  "Numericallly higher values represent higher priorities."
 *
 *  Thus, RTEMS Core has priorities run in the opposite sense of the POSIX API.
 */

RTEMS_INLINE_ROUTINE boolean _POSIX_Priority_Is_valid(
  int priority
)
{
  return (boolean) (priority >= 1 && priority <= 255);
}

RTEMS_INLINE_ROUTINE Priority_Control _POSIX_Priority_To_core(
  int priority
)
{
  return (Priority_Control) (256 - priority);
}

RTEMS_INLINE_ROUTINE int _POSIX_Priority_From_core(
  Priority_Control priority
)
{
  return 256 - priority;
}

#endif
