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

#define _POSIX_Priority_Is_valid( _priority ) \
  ((boolean) ((_priority) >= 1 && (_priority) <= 254))

#define _POSIX_Priority_To_core( _priority ) \
  ((Priority_Control) (255 - (_priority)))

#define _POSIX_Priority_From_core( _priority ) \
  (255 - (_priority))

#endif
