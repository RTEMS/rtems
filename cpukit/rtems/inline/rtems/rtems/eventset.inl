/*  inline/eventset.inl
 *
 *  This include file contains the information pertaining to event sets.
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

#ifndef __INLINE_EVENT_SET_inl
#define __INLINE_EVENT_SET_inl

/*PAGE
 *
 *  _Event_sets_Is_empty
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if on events are posted in the event_set,
 *  and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Event_sets_Is_empty(
  rtems_event_set the_event_set
)
{
  return ( the_event_set == 0 );
}

/*PAGE
 *
 *  _Event_sets_Post
 *
 *  DESCRIPTION:
 *
 *  This routine posts the given new_events into the event_set
 *  passed in.  The result is returned to the user in event_set.
 */

RTEMS_INLINE_ROUTINE void _Event_sets_Post(
  rtems_event_set  the_new_events,
  rtems_event_set *the_event_set
)
{
  ISR_Level level;

  _ISR_Disable( level );
    *the_event_set |= the_new_events;
  _ISR_Enable( level );
}

/*PAGE
 *
 *  _Event_sets_Get
 *
 *  DESCRIPTION:
 *
 *  This function returns the events in event_condition which are
 *  set in event_set.
 */

RTEMS_INLINE_ROUTINE rtems_event_set _Event_sets_Get(
  rtems_event_set the_event_set,
  rtems_event_set the_event_condition
)
{
   return ( the_event_set & the_event_condition );
}

/*PAGE
 *
 *  _Event_sets_Clear
 *
 *  DESCRIPTION:
 *
 *  This function removes the events in mask from the event_set
 *  passed in.  The result is returned to the user in event_set.
 */

RTEMS_INLINE_ROUTINE rtems_event_set _Event_sets_Clear(
 rtems_event_set the_event_set,
 rtems_event_set the_mask
)
{
   return ( the_event_set & ~(the_mask) );
}

#endif
/* end of include file */
