/*  
 *  $Id$
 */

#ifndef __RTEMS_POSIX_PRIORITY_inl
#define __RTEMS_POSIX_PRIORITY_inl

STATIC INLINE boolean _POSIX_Priority_Is_valid(
  int priority
)
{
  return (boolean) priority >= 1 && priority <= 255;
}

STATIC INLINE Priority_Control _POSIX_Priority_To_core(
  int priority
)
{
  return (Priority_Control) 256 - priority;
}

STATIC INLINE int _POSIX_Priority_From_core(
  Priority_Control priority
)
{
  return 256 - priority;
}

#endif
