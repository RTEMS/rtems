/*  tasks.inl
 *
 *  This file contains the macro implementation of all inlined
 *  routines in the with RTEMS Tasks Manager.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_TASKS_inl
#define __RTEMS_TASKS_inl

/*PAGE
 *
 *  _RTEMS_tasks_Allocate
 *
 */

#define _RTEMS_tasks_Allocate() \
  (Thread_Control *) _Objects_Allocate( &_RTEMS_tasks_Information )

/*PAGE
 *
 *  _RTEMS_tasks_Free
 *
 */

#define _RTEMS_tasks_Free( _the_task ) \
  _Objects_Free( &_RTEMS_tasks_Information, &(_the_task)->Object )

/*PAGE
 *
 *  _RTEMS_tasks_Priority_to_Core
 */
 
#define _RTEMS_tasks_Priority_to_Core( _priority ) \
  ((Priority_Control) (_priority))

/*PAGE
 *
 *  _RTEMS_tasks_Priority_is_valid
 *
 */
 
#define _RTEMS_tasks_Priority_is_valid( _the_priority ) \
  ( ((_the_priority) >= RTEMS_MINIMUM_PRIORITY) && \
    ((_the_priority) <= RTEMS_MAXIMUM_PRIORITY) )
 

#endif
/* end of include file */
