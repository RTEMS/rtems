/*  tod.inl
 *
 *  This file contains the macro implementation of the inlined routines
 *  from the Time of Day Handler.
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

#ifndef __TIME_OF_DAY_inl
#define __TIME_OF_DAY_inl

/*PAGE
 *
 *  _TOD_Tickle_ticks
 *
 */

#define _TOD_Tickle_ticks() \
  _TOD_Current.ticks++; \
  _Watchdog_Ticks_since_boot++

/*PAGE
 *
 *  _TOD_Deactivate
 *
 */

#define _TOD_Deactivate() \
   _Watchdog_Remove( &_TOD_Seconds_watchdog )

/*PAGE
 *
 *  _TOD_Activate
 *
 */

#define _TOD_Activate( _ticks ) \
   _Watchdog_Insert_ticks( &_TOD_Seconds_watchdog, (_ticks) )

#endif
/* end of include file */
