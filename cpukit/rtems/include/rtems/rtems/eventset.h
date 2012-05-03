/**
 * @file rtems/rtems/eventset.h
 *
 *  This include file contains the information pertaining to the
 *  Event Sets Handler.  This handler provides methods for the manipulation
 *  of event sets which will be sent and received by tasks.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_EVENTSET_H
#define _RTEMS_RTEMS_EVENTSET_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicEventSet Event Set
 *
 *  @ingroup ClassicEvent
 *
 *  @{
 */

/**
 *  @brief Integer type to hold an event set of up to 32 events represented as
 *  a bit field.
 */
typedef uint32_t   rtems_event_set;

/**
 *  @brief Constant used to send or receive all events.
 */
#define RTEMS_ALL_EVENTS  0xFFFFFFFF

/** @brief Defines the bit in the event set associated with event 0. */
#define RTEMS_EVENT_0     0x00000001
/** @brief Defines the bit in the event set associated with event 1. */
#define RTEMS_EVENT_1     0x00000002
/** @brief Defines the bit in the event set associated with event 2. */
#define RTEMS_EVENT_2     0x00000004
/** @brief Defines the bit in the event set associated with event 3. */
#define RTEMS_EVENT_3     0x00000008
/** @brief Defines the bit in the event set associated with event 4. */
#define RTEMS_EVENT_4     0x00000010
/** @brief Defines the bit in the event set associated with event 5. */
#define RTEMS_EVENT_5     0x00000020
/** @brief Defines the bit in the event set associated with event 6. */
#define RTEMS_EVENT_6     0x00000040
/** @brief Defines the bit in the event set associated with event 7. */
#define RTEMS_EVENT_7     0x00000080
/** @brief Defines the bit in the event set associated with event 8. */
#define RTEMS_EVENT_8     0x00000100
/** @brief Defines the bit in the event set associated with event 9. */
#define RTEMS_EVENT_9     0x00000200
/** @brief Defines the bit in the event set associated with event 10. */
#define RTEMS_EVENT_10    0x00000400
/** @brief Defines the bit in the event set associated with event 11. */
#define RTEMS_EVENT_11    0x00000800
/** @brief Defines the bit in the event set associated with event 12. */
#define RTEMS_EVENT_12    0x00001000
/** @brief Defines the bit in the event set associated with event 13. */
#define RTEMS_EVENT_13    0x00002000
/** @brief Defines the bit in the event set associated with event 14. */
#define RTEMS_EVENT_14    0x00004000
/** @brief Defines the bit in the event set associated with event 15. */
#define RTEMS_EVENT_15    0x00008000
/** @brief Defines the bit in the event set associated with event 16. */
#define RTEMS_EVENT_16    0x00010000
/** @brief Defines the bit in the event set associated with event 17. */
#define RTEMS_EVENT_17    0x00020000
/** @brief Defines the bit in the event set associated with event 18. */
#define RTEMS_EVENT_18    0x00040000
/** @brief Defines the bit in the event set associated with event 19. */
#define RTEMS_EVENT_19    0x00080000
/** @brief Defines the bit in the event set associated with event 20. */
#define RTEMS_EVENT_20    0x00100000
/** @brief Defines the bit in the event set associated with event 21. */
#define RTEMS_EVENT_21    0x00200000
/** @brief Defines the bit in the event set associated with event 22. */
#define RTEMS_EVENT_22    0x00400000
/** @brief Defines the bit in the event set associated with event 23. */
#define RTEMS_EVENT_23    0x00800000
/** @brief Defines the bit in the event set associated with event 24. */
#define RTEMS_EVENT_24    0x01000000
/** @brief Defines the bit in the event set associated with event 25. */
#define RTEMS_EVENT_25    0x02000000
/** @brief Defines the bit in the event set associated with event 26. */
#define RTEMS_EVENT_26    0x04000000
/** @brief Defines the bit in the event set associated with event 27. */
#define RTEMS_EVENT_27    0x08000000
/** @brief Defines the bit in the event set associated with event 29. */
#define RTEMS_EVENT_28    0x10000000
/** @brief Defines the bit in the event set associated with event 29. */
#define RTEMS_EVENT_29    0x20000000
/** @brief Defines the bit in the event set associated with event 30. */
#define RTEMS_EVENT_30    0x40000000
/** @brief Defines the bit in the event set associated with event 31. */
#define RTEMS_EVENT_31    0x80000000

/** @} */

/**
 *  @brief Constant used to receive the set of currently pending events in
 *  rtems_event_receive().
 *
 *  @ingroup ClassicEvent
 */
#define RTEMS_PENDING_EVENTS      0

/**
 *  @addtogroup ScoreEvent
 *
 *  @{
 */

/**
 *  The following constant is the value of an event set which
 *  has no events pending.
 */
#define EVENT_SETS_NONE_PENDING 0

/** @} */

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/eventset.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
