/*  eventset.h
 *
 *  This include file contains the information pertaining to the
 *  Event Sets Handler.  This handler provides methods for the manipulation
 *  of event sets which will be sent and received by tasks.
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

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/eventset.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
