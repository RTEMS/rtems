/*  timer.inl
 *
 *  This file contains the macro implementation of the inlined routines
 *  from the POSIX Timer Manager.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __POSIX_TIMER_inl
#define __POSIX_TIMER_inl

/*PAGE
 *
 *  _POSIX_Timer_Allocate
 */

#define _POSIX_Timer_Allocate() \
  (POSIX_Timer_Control *) _Objects_Allocate( &_POSIX_Timer_Information )

/*PAGE
 *
 *  _POSIX_Timer_Free
 */

#define _POSIX_Timer_Free( _the_timer ) \
  _Objects_Free( &_POSIX_Timer_Information, &(_the_timer)->Object );

/*PAGE
 *
 *  _POSIX_Timer_Get
 */

#define _POSIX_Timer_Get( _the_timer ) \
  (POSIX_Timer_Control *) \
    _Objects_Get( &_POSIX_Timer_Information, (_id), (_location) )

/*PAGE
 *
 *  _POSIX_Timer_Is_null
 */

#define _POSIX_Timer_Is_null( _the_timer ) \
  (!(_the_timer))

#endif
/* end of include file */
