/*  eventset.inl
 *
 *  This include file contains the macro implementation of inlined
 *  routines in the event set object.
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
   *(_the_event_set) |= (_the_new_events)

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
