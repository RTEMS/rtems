/*
 *
 *
 *  $Id$
 */

#ifndef __RTEMS_POSIX_PRIORITY_h
#define __RTEMS_POSIX_PRIORITY_h

#include <rtems/score/priority.h>

/*
 *  1003.1b-1993,2.2.2.80 definition of priority, p. 19
 *
 *  "Numericallly higher values represent higher priorities."
 *
 *  Thus, RTEMS Core has priorities run in the opposite sense of the POSIX API.
 */

#define POSIX_SCHEDULER_MAXIMUM_PRIORITY (255)
  
#define POSIX_SCHEDULER_MINIMUM_PRIORITY (1)

STATIC INLINE boolean _POSIX_Priority_Is_valid(
  int priority
);

STATIC INLINE Priority_Control _POSIX_Priority_To_core(
  int priority
);

STATIC INLINE int _POSIX_Priority_From_core(
  Priority_Control priority
);

#include <rtems/posix/priority.inl>

#endif
