/*  eventset.h
 *
 *  This include file contains the information pertaining to the
 *  Event Sets Handler.  This handler provides methods for the manipulation
 *  of event sets which will be sent and received by tasks.
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

#ifndef __RTEMS_EVENT_SET_h
#define __RTEMS_EVENT_SET_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following defines the type used to control event sets.
 */

typedef unsigned32 rtems_event_set;

/*
 *  The following constants define the individual events which may
 *  be used to compose an event set.
 */

#define RTEMS_PENDING_EVENTS      0           /* receive pending events   */
#define RTEMS_ALL_EVENTS  0xFFFFFFFF

#define RTEMS_EVENT_0     0x00000001
#define RTEMS_EVENT_1     0x00000002
#define RTEMS_EVENT_2     0x00000004
#define RTEMS_EVENT_3     0x00000008
#define RTEMS_EVENT_4     0x00000010
#define RTEMS_EVENT_5     0x00000020
#define RTEMS_EVENT_6     0x00000040
#define RTEMS_EVENT_7     0x00000080
#define RTEMS_EVENT_8     0x00000100
#define RTEMS_EVENT_9     0x00000200
#define RTEMS_EVENT_10    0x00000400
#define RTEMS_EVENT_11    0x00000800
#define RTEMS_EVENT_12    0x00001000
#define RTEMS_EVENT_13    0x00002000
#define RTEMS_EVENT_14    0x00004000
#define RTEMS_EVENT_15    0x00008000
#define RTEMS_EVENT_16    0x00010000
#define RTEMS_EVENT_17    0x00020000
#define RTEMS_EVENT_18    0x00040000
#define RTEMS_EVENT_19    0x00080000
#define RTEMS_EVENT_20    0x00100000
#define RTEMS_EVENT_21    0x00200000
#define RTEMS_EVENT_22    0x00400000
#define RTEMS_EVENT_23    0x00800000
#define RTEMS_EVENT_24    0x01000000
#define RTEMS_EVENT_25    0x02000000
#define RTEMS_EVENT_26    0x04000000
#define RTEMS_EVENT_27    0x08000000
#define RTEMS_EVENT_28    0x10000000
#define RTEMS_EVENT_29    0x20000000
#define RTEMS_EVENT_30    0x40000000
#define RTEMS_EVENT_31    0x80000000


/*
 *  The following constant is the value of an event set which
 *  has no events pending.
 */

#define EVENT_SETS_NONE_PENDING 0

/*
 *  _Event_sets_Is_empty
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if on events are posted in the event_set,
 *  and FALSE otherwise.
 */

STATIC INLINE boolean _Event_sets_Is_empty(
  rtems_event_set the_event_set
);

/*
 *  _Event_sets_Post
 *
 *  DESCRIPTION:
 *
 *  This routine posts the given new_events into the event_set
 *  passed in.  The result is returned to the user in event_set.
 */

STATIC INLINE void _Event_sets_Post(
  rtems_event_set  the_new_events,
  rtems_event_set *the_event_set
);

/*
 *  _Event_sets_Get
 *
 *  DESCRIPTION:
 *
 *  This function returns the events in event_condition which are
 *  set in event_set.
 */

STATIC INLINE rtems_event_set _Event_sets_Get(
  rtems_event_set the_event_set,
  rtems_event_set the_event_condition
);

/*
 *  _Event_sets_Clear
 *
 *  DESCRIPTION:
 *
 *  This function removes the events in mask from the event_set
 *  passed in.  The result is returned to the user in event_set.
 */

STATIC INLINE rtems_event_set _Event_sets_Clear(
 rtems_event_set the_event_set,
 rtems_event_set the_mask
);

#include <rtems/rtems/eventset.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
