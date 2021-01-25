/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicEvent
 *
 * @brief This header file provides the Event Manager API.
 */

/*
 * Copyright (C) 2014, 2020 embedded brains GmbH (http://www.embedded-brains.de)
 * Copyright (C) 1988, 2008 On-Line Applications Research Corporation (OAR)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/event/if/header */

#ifndef _RTEMS_RTEMS_EVENT_H
#define _RTEMS_RTEMS_EVENT_H

#include <stdint.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/event/if/group */

/**
 * @defgroup RTEMSAPIClassicEvent Event Manager
 *
 * @ingroup RTEMSAPIClassic
 *
 * @brief The Event Manager provides a high performance method of inter-task
 *   communication and synchronization.
 */

/* Generated from spec:/rtems/event/if/all-events */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents all events of an event set.
 *
 * This event set constant is equal to the bitwise or of #RTEMS_EVENT_0,
 * #RTEMS_EVENT_1, #RTEMS_EVENT_2, #RTEMS_EVENT_3, #RTEMS_EVENT_4,
 * #RTEMS_EVENT_5, #RTEMS_EVENT_6, #RTEMS_EVENT_7, #RTEMS_EVENT_8,
 * #RTEMS_EVENT_9, #RTEMS_EVENT_10, #RTEMS_EVENT_11, #RTEMS_EVENT_12,
 * #RTEMS_EVENT_13, #RTEMS_EVENT_14, #RTEMS_EVENT_15, #RTEMS_EVENT_16,
 * #RTEMS_EVENT_17, #RTEMS_EVENT_18, #RTEMS_EVENT_19, #RTEMS_EVENT_20,
 * #RTEMS_EVENT_21, #RTEMS_EVENT_22, #RTEMS_EVENT_23, #RTEMS_EVENT_24,
 * #RTEMS_EVENT_25, #RTEMS_EVENT_26, #RTEMS_EVENT_27, #RTEMS_EVENT_28,
 * #RTEMS_EVENT_29, #RTEMS_EVENT_30, and #RTEMS_EVENT_31.
 */
#define RTEMS_ALL_EVENTS 0xffffffff

/* Generated from spec:/rtems/event/if/event-00 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 0.
 */
#define RTEMS_EVENT_0 0x00000001

/* Generated from spec:/rtems/event/if/event-01 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 1.
 */
#define RTEMS_EVENT_1 0x00000002

/* Generated from spec:/rtems/event/if/event-02 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 2.
 */
#define RTEMS_EVENT_2 0x00000004

/* Generated from spec:/rtems/event/if/event-03 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 3.
 */
#define RTEMS_EVENT_3 0x00000008

/* Generated from spec:/rtems/event/if/event-04 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 4.
 */
#define RTEMS_EVENT_4 0x00000010

/* Generated from spec:/rtems/event/if/event-05 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 5.
 */
#define RTEMS_EVENT_5 0x00000020

/* Generated from spec:/rtems/event/if/event-06 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 6.
 */
#define RTEMS_EVENT_6 0x00000040

/* Generated from spec:/rtems/event/if/event-07 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 7.
 */
#define RTEMS_EVENT_7 0x00000080

/* Generated from spec:/rtems/event/if/event-08 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 8.
 */
#define RTEMS_EVENT_8 0x00000100

/* Generated from spec:/rtems/event/if/event-09 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 9.
 */
#define RTEMS_EVENT_9 0x00000200

/* Generated from spec:/rtems/event/if/event-10 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 10.
 */
#define RTEMS_EVENT_10 0x00000400

/* Generated from spec:/rtems/event/if/event-11 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 11.
 */
#define RTEMS_EVENT_11 0x00000800

/* Generated from spec:/rtems/event/if/event-12 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 12.
 */
#define RTEMS_EVENT_12 0x00001000

/* Generated from spec:/rtems/event/if/event-13 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 13.
 */
#define RTEMS_EVENT_13 0x00002000

/* Generated from spec:/rtems/event/if/event-14 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 14.
 */
#define RTEMS_EVENT_14 0x00004000

/* Generated from spec:/rtems/event/if/event-15 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 15.
 */
#define RTEMS_EVENT_15 0x00008000

/* Generated from spec:/rtems/event/if/event-16 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 16.
 */
#define RTEMS_EVENT_16 0x00010000

/* Generated from spec:/rtems/event/if/event-17 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 17.
 */
#define RTEMS_EVENT_17 0x00020000

/* Generated from spec:/rtems/event/if/event-18 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 18.
 */
#define RTEMS_EVENT_18 0x00040000

/* Generated from spec:/rtems/event/if/event-19 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 19.
 */
#define RTEMS_EVENT_19 0x00080000

/* Generated from spec:/rtems/event/if/event-20 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 20.
 */
#define RTEMS_EVENT_20 0x00100000

/* Generated from spec:/rtems/event/if/event-21 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 21.
 */
#define RTEMS_EVENT_21 0x00200000

/* Generated from spec:/rtems/event/if/event-22 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 22.
 */
#define RTEMS_EVENT_22 0x00400000

/* Generated from spec:/rtems/event/if/event-23 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 23.
 */
#define RTEMS_EVENT_23 0x00800000

/* Generated from spec:/rtems/event/if/event-24 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 24.
 */
#define RTEMS_EVENT_24 0x01000000

/* Generated from spec:/rtems/event/if/event-25 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 25.
 */
#define RTEMS_EVENT_25 0x02000000

/* Generated from spec:/rtems/event/if/event-26 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 26.
 */
#define RTEMS_EVENT_26 0x04000000

/* Generated from spec:/rtems/event/if/event-27 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 27.
 */
#define RTEMS_EVENT_27 0x08000000

/* Generated from spec:/rtems/event/if/event-28 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 28.
 */
#define RTEMS_EVENT_28 0x10000000

/* Generated from spec:/rtems/event/if/event-29 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 29.
 */
#define RTEMS_EVENT_29 0x20000000

/* Generated from spec:/rtems/event/if/event-30 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 30.
 */
#define RTEMS_EVENT_30 0x40000000

/* Generated from spec:/rtems/event/if/event-31 */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant represents the bit in the event set
 *   associated with event 31.
 */
#define RTEMS_EVENT_31 0x80000000

/* Generated from spec:/rtems/event/if/pending-events */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This event set constant indicates that rtems_event_receive() shall
 *   return the set of pending events.
 */
#define RTEMS_PENDING_EVENTS 0

/* Generated from spec:/rtems/event/if/set */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief This integer type represents a bit field which can hold exactly 32
 *   individual events.
 */
typedef uint32_t rtems_event_set;

/* Generated from spec:/rtems/event/if/system-network-close */

/**
 * @brief This event set constant represents the reserved system event for a
 *   network socket close.
 */
#define RTEMS_EVENT_SYSTEM_NETWORK_CLOSE RTEMS_EVENT_26

/* Generated from spec:/rtems/event/if/system-network-sbwait */

/**
 * @brief This event set constant represents the reserved system event for a
 *   network socket buffer wait usage.
 */
#define RTEMS_EVENT_SYSTEM_NETWORK_SBWAIT RTEMS_EVENT_24

/* Generated from spec:/rtems/event/if/system-network-sosleep */

/**
 * @brief This event set constant represents the reserved system event for a
 *   network socket sleep.
 */
#define RTEMS_EVENT_SYSTEM_NETWORK_SOSLEEP RTEMS_EVENT_25

/* Generated from spec:/rtems/event/if/system-receive */

/**
 * @brief Receives or gets a system event set from the executing task.
 *
 * @param event_in is the event set of interest.  Use #RTEMS_PENDING_EVENTS to
 *   get the pending events.
 *
 * @param option_set is the option set.
 *
 * @param ticks is the timeout in clock ticks if the #RTEMS_WAIT option was
 *   set.  Use #RTEMS_NO_TIMEOUT to wait potentially forever.
 *
 * @param event_out is the pointer to an event set.  The received or pending
 *   events are stored in the referenced event set if the operation was
 *   successful.
 *
 * This directive performs the same actions as the rtems_event_receive()
 * directive except that it operates with a different set of events for each
 * task.
 */
rtems_status_code rtems_event_system_receive(
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
);

/* Generated from spec:/rtems/event/if/system-send */

/**
 * @brief Sends the system event set to the task.
 *
 * @param id is the identifier of the target task to receive the event set.
 *
 * @param event_in is the event set to send.
 */
rtems_status_code rtems_event_system_send(
  rtems_id        id,
  rtems_event_set event_in
);

/* Generated from spec:/rtems/event/if/system-server */

/**
 * @brief This event set constant represents the reserved system event for
 *   server thread usage, for example the timer or interrupt server.
 */
#define RTEMS_EVENT_SYSTEM_SERVER RTEMS_EVENT_30

/* Generated from spec:/rtems/event/if/system-server-resume */

/**
 * @brief This event set constant represents the reserved system event to
 *   resume a server thread, for example the timer or interrupt server.
 */
#define RTEMS_EVENT_SYSTEM_SERVER_RESUME RTEMS_EVENT_29

/* Generated from spec:/rtems/event/if/system-transient */

/**
 * @brief This event set constant represents the reserved system event for
 *   transient usage.
 */
#define RTEMS_EVENT_SYSTEM_TRANSIENT RTEMS_EVENT_31

/* Generated from spec:/rtems/event/if/transient-clear */

/**
 * @brief Clears the transient event.
 */
static inline void rtems_event_transient_clear( void )
{
  rtems_event_set event_out;

  (void) rtems_event_system_receive(
    RTEMS_EVENT_SYSTEM_TRANSIENT,
    RTEMS_EVENT_ALL | RTEMS_NO_WAIT,
    0,
    &event_out
  );
}

/* Generated from spec:/rtems/event/if/transient-receive */

/**
 * @brief Receives the transient event.
 *
 * @param option_set is the option set.
 *
 * @param ticks is the optional timeout in clock ticks.
 */
static inline rtems_status_code rtems_event_transient_receive(
  rtems_option   option_set,
  rtems_interval ticks
)
{
  rtems_event_set event_out;

  return rtems_event_system_receive(
    RTEMS_EVENT_SYSTEM_TRANSIENT,
    RTEMS_EVENT_ALL | option_set,
    ticks,
    &event_out
  );
}

/* Generated from spec:/rtems/event/if/transient-send */

/**
 * @brief Sends the transient event to the task.
 *
 * @param id is the identifier of the task to receive the transient event.
 */
static inline rtems_status_code rtems_event_transient_send( rtems_id id )
{
  return rtems_event_system_send( id, RTEMS_EVENT_SYSTEM_TRANSIENT );
}

/* Generated from spec:/rtems/event/if/send */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief Sends the event set to the task.
 *
 * @param id is the identifier of the target task to receive the event set.
 *
 * @param event_in is the event set to send.
 *
 * This directive sends the event set, ``event_in``, to the target task
 * identified by ``id``.  Based upon the state of the target task, one of the
 * following situations applies:
 *
 * * The target task is blocked waiting for events, then
 *
 *   * if the waiting task's input event condition is satisfied, then the task
 *     is made ready for execution, or
 *
 *   * otherwise, the event set is posted but left pending and the task remains
 *     blocked.
 *
 * * The target task is not waiting for events, then the event set is posted
 *   and left pending.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ID There was no task associated with the identifier
 *   specified by ``id``.
 *
 * @par Notes
 * @parblock
 * Events can be sent by tasks or an ISR.
 *
 * Specifying #RTEMS_SELF for ``id`` results in the event set being sent to the
 * calling task.
 *
 * The event set to send shall be built by a *bitwise or* of the desired
 * events. The set of valid events is #RTEMS_EVENT_0 through #RTEMS_EVENT_31.
 * If an event is not explicitly specified in the set, then it is not present.
 *
 * Identical events sent to a task are not queued.  In other words, the second,
 * and subsequent, posting of an event to a task before it can perform an
 * rtems_event_receive() has no effect.
 *
 * The calling task will be preempted if it has preemption enabled and a higher
 * priority task is unblocked as the result of this directive.
 *
 * Sending an event set to a global task which does not reside on the local
 * node will generate a request telling the remote node to send the event set
 * to the appropriate task.
 * @endparblock
 */
rtems_status_code rtems_event_send( rtems_id id, rtems_event_set event_in );

/* Generated from spec:/rtems/event/if/receive */

/**
 * @ingroup RTEMSAPIClassicEvent
 *
 * @brief Receives or gets an event set from the calling task.
 *
 * @param event_in is the event set of interest.  Use #RTEMS_PENDING_EVENTS to
 *   get the pending events.
 *
 * @param option_set is the option set.
 *
 * @param ticks is the timeout in clock ticks if the #RTEMS_WAIT option is set.
 *   Use #RTEMS_NO_TIMEOUT to wait potentially forever.
 *
 * @param event_out is the pointer to an event set.  The received or pending
 *   events are stored in the referenced event set if the operation was
 *   successful.
 *
 * This directive can be used to
 *
 * * get the pending events of the calling task, or
 *
 * * receive events.
 *
 * To **get the pending events** use the constant #RTEMS_PENDING_EVENTS for the
 * ``event_in`` parameter.  The pending events are returned to the calling task
 * but the event set of the task is left unaltered.  The ``option_set`` and
 * ``ticks`` parameters are ignored in this case.  The directive returns
 * immediately and does not block.
 *
 * To **receive events** you have to define an input event condition and some
 * options.  The **option set** specified in ``option_set`` defines
 *
 * * if the task will wait or poll for the events, and
 *
 * * if the task wants to receive all or any of the input events.
 *
 * The option set is built through a *bitwise or* of the option constants
 * described below.
 *
 * The task can **wait** or **poll** for the events.
 *
 * * **Waiting** for events is the default and can be emphasized through the
 *   use of the #RTEMS_WAIT option.  The ``ticks`` parameter defines how long
 *   the task is willing to wait.  Use #RTEMS_NO_TIMEOUT to wait potentially
 *   forever, otherwise set a timeout interval in clock ticks.
 *
 * * Not waiting for events (**polling**) is selected by the #RTEMS_NO_WAIT
 *   option.  If this option is defined, then the ``ticks`` parameter is
 *   ignored.
 *
 * The task can receive **all** or **any** of the input events specified in
 * ``event_in``.
 *
 * * Receiving **all** input events is the default and can be emphasized
 *   through the use of the #RTEMS_EVENT_ALL option.
 *
 * * Receiving **any** of the input events is selected by the #RTEMS_EVENT_ANY
 *   option.
 *
 * @retval ::RTEMS_SUCCESSFUL The requested operation was successful.
 *
 * @retval ::RTEMS_INVALID_ADDRESS The ``event_out`` parameter was NULL.
 *
 * @retval ::RTEMS_UNSATISFIED The events of interest were not immediately
 *   available.
 *
 * @retval ::RTEMS_TIMEOUT The events of interest were not available within the
 *   specified timeout interval.
 *
 * @par Notes
 * @parblock
 * This directive shall be called by a task.  Calling this directive from
 * interrupt context is undefined behaviour.
 *
 * This directive only affects the events specified in ``event_in``. Any
 * pending events that do not correspond to any of the events specified in
 * ``event_in`` will be left pending.
 *
 * To receive all events use the event set constant #RTEMS_ALL_EVENTS for the
 * ``event_in`` parameter.  Do not confuse this event set constant with the
 * directive option #RTEMS_EVENT_ALL.
 *
 * A task can **receive all of the pending events** by calling the directive
 * with a value of #RTEMS_ALL_EVENTS for the ``event_in`` parameter and the
 * bitwise or of the #RTEMS_NO_WAIT and #RTEMS_EVENT_ANY options for the
 * ``option_set`` parameter.  The pending events are returned and the event set
 * of the task is cleared.  If no events are pending then the
 * ::RTEMS_UNSATISFIED status code will be returned.
 * @endparblock
 */
rtems_status_code rtems_event_receive(
  rtems_event_set  event_in,
  rtems_option     option_set,
  rtems_interval   ticks,
  rtems_event_set *event_out
);

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_RTEMS_EVENT_H */
