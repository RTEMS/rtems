/*  eventset.inl
 *
 *  This include file contains the macro implementation of inlined
 *  routines in the event set object.
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

#ifndef __EVENT_SET_inl
#define __EVENT_SET_inl

/*PAGE
 *
 *  _Event_sets_Is_empty
 */

#define _Event_sets_Is_empty( _the_event_set )  \
  ((_the_event_set) == 0 )

/*PAGE
 *
 *  _Event_sets_Is_empty
 */

#define _Event_sets_Post( _the_new_events, _the_event_set ) \
   do { \
     ISR_Level level; \
     \
     _ISR_Disable( level ); \
       *(_the_event_set) |= (_the_new_events); \
     _ISR_Enable( level ); \
   } while (0);

/*PAGE
 *
 *  _Event_sets_Is_empty
 */

#define _Event_sets_Get( _the_event_set, _the_event_condition ) \
   ((_the_event_set) & (_the_event_condition))

/*PAGE
 *
 *  _Event_sets_Clear
 */

#define _Event_sets_Clear( _the_event_set, _the_mask ) \
   ((_the_event_set) & ~(_the_mask))

#endif
/* end of include file */
