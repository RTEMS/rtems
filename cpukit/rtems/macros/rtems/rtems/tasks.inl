/*  tasks.inl
 *
 *  This file contains the macro implementation of all inlined
 *  routines in the with RTEMS Tasks Manager.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
