/*  inline/eventset.inl
 *
 *  This include file contains the information pertaining to event sets.
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

#ifndef __INLINE_EVENT_SET_inl
#define __INLINE_EVENT_SET_inl

/*PAGE
 *
 *  _Event_sets_Is_empty
 */

STATIC INLINE boolean _Event_sets_Is_empty(
  rtems_event_set the_event_set
)
{
  return ( the_event_set == 0 );
}

/*PAGE
 *
 *  _Event_sets_Post
 */

STATIC INLINE void _Event_sets_Post(
  rtems_event_set  the_new_events,
  rtems_event_set *the_event_set
)
{
   *the_event_set |= the_new_events;
}

/*PAGE
 *
 *  _Event_sets_Get
 */

STATIC INLINE rtems_event_set _Event_sets_Get(
  rtems_event_set the_event_set,
  rtems_event_set the_event_condition
)
{
   return ( the_event_set & the_event_condition );
}

/*PAGE
 *
 *  _Event_sets_Clear
 */

STATIC INLINE rtems_event_set _Event_sets_Clear(
 rtems_event_set the_event_set,
 rtems_event_set the_mask
)
{
   return ( the_event_set & ~(the_mask) );
}

#endif
/* end of include file */
