/*  states.h
 *
 *  This include file contains thread execution state information.
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

#ifndef __RTEMS_STATES_h
#define __RTEMS_STATES_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following type defines the control block used to manage a
 *  thread's state.
 */

typedef unsigned32 States_Control;

/*
 *  The following constants define the individual states which may be
 *  be used to compose and manipulate a thread's state.
 */

#define STATES_ALL_SET                         0xffff /* all states */
#define STATES_READY                           0x0000 /* ready to run */
#define STATES_DORMANT                         0x0001 /* created not started */
#define STATES_SUSPENDED                       0x0002 /* waiting for resume */
#define STATES_TRANSIENT                       0x0004 /* thread in transition */
#define STATES_DELAYING                        0x0008 /* wait for timeout */
#define STATES_WAITING_FOR_TIME                0x0010 /* wait for TOD */
#define STATES_WAITING_FOR_BUFFER              0x0020 
#define STATES_WAITING_FOR_SEGMENT             0x0040
#define STATES_WAITING_FOR_MESSAGE             0x0080
#define STATES_WAITING_FOR_EVENT               0x0100
#define STATES_WAITING_FOR_SEMAPHORE           0x0200
#define STATES_WAITING_FOR_MUTEX               0x0400
#define STATES_WAITING_FOR_CONDITION_VARIABLE  0x0800
#define STATES_WAITING_FOR_RPC_REPLY           0x1000
#define STATES_WAITING_FOR_PERIOD              0x2000

#define STATES_LOCALLY_BLOCKED ( STATES_WAITING_FOR_BUFFER             | \
                                 STATES_WAITING_FOR_SEGMENT            | \
                                 STATES_WAITING_FOR_MESSAGE            | \
                                 STATES_WAITING_FOR_MUTEX              | \
                                 STATES_WAITING_FOR_CONDITION_VARIABLE | \
                                 STATES_WAITING_FOR_SEMAPHORE   )

#define STATES_WAITING_ON_THREAD_QUEUE \
                               ( STATES_LOCALLY_BLOCKED         | \
                                 STATES_WAITING_FOR_RPC_REPLY   )

#define STATES_BLOCKED         ( STATES_DELAYING                | \
                                 STATES_WAITING_FOR_TIME        | \
                                 STATES_WAITING_FOR_PERIOD      | \
                                 STATES_WAITING_FOR_EVENT       | \
                                 STATES_WAITING_ON_THREAD_QUEUE )

/*
 *  _States_Set
 *
 *  DESCRIPTION:
 *
 *  This function sets the given states_to_set into the current_state
 *  passed in.  The result is returned to the user in current_state.
 */

STATIC INLINE States_Control _States_Set (
  States_Control states_to_set,
  States_Control current_state
);

/*
 *  _States_Clear
 *
 *  DESCRIPTION:
 *
 *  This function clears the given states_to_clear into the current_state
 *  passed in.  The result is returned to the user in current_state.
 */

STATIC INLINE States_Control _States_Clear (
  States_Control states_to_clear,
  States_Control current_state
);

/*
 *  _States_Is_ready
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_states indicates that the
 *  state is READY, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_ready (
  States_Control the_states
);

/*
 *  _States_Is_only_dormant
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the DORMANT state is the ONLY state
 *  set in the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_only_dormant (
  States_Control the_states
);

/*
 *  _States_Is_dormant
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the DORMANT state is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_dormant (
  States_Control the_states
);

/*
 *  _States_Is_suspended
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the SUSPENDED state is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_suspended (
  States_Control the_states
);

/*
 *  _States_Is_Transient
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the TRANSIENT state is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_transient (
  States_Control the_states
);

/*
 *  _States_Is_delaying
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the DELAYING state is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_delaying (
  States_Control the_states
);

/*
 *  _States_Is_waiting_for_buffer
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_BUFFER state is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_waiting_for_buffer (
  States_Control the_states
);

/*
 *  _States_Is_waiting_for_segment
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_SEGMENT state is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_waiting_for_segment (
  States_Control the_states
);

/*
 *  _States_Is_waiting_for_message
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_MESSAGE state is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_waiting_for_message (
  States_Control the_states
);

/*
 *  _States_Is_waiting_for_event
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_EVENT state is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_waiting_for_event (
  States_Control the_states
);

/*
 *  _States_Is_waiting_for_mutex
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_MUTEX state
 *  is set in the_states, and FALSE otherwise.
 */
 
STATIC INLINE boolean _States_Is_waiting_for_mutex (
  States_Control the_states
);

/*
 *  _States_Is_waiting_for_semaphore
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_SEMAPHORE state
 *  is set in the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_waiting_for_semaphore (
  States_Control the_states
);

/*
 *  _States_Is_waiting_for_time
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_TIME state is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_waiting_for_time (
  States_Control the_states
);

/*
 *  _States_Is_waiting_for_rpc_reply
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_TIME state is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_waiting_for_rpc_reply (
  States_Control the_states
);

/*
 *  _States_Is_waiting_for_period
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_PERIOD state is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_waiting_for_period (
  States_Control the_states
);

/*
 *  _States_Is_locally_blocked
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if one of the states which indicates
 *  that a task is blocked waiting for a local resource is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_locally_blocked (
  States_Control the_states
);

/*
 *  _States_Is_waiting_on_thread_queue
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if one of the states which indicates
 *  that a task is blocked waiting for a local resource is set in
 *  the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_waiting_on_thread_queue (
  States_Control the_states
);

/*
 *  _States_Is_blocked
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if one of the states which indicates
 *  that a task is blocked is set in the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Is_blocked (
  States_Control the_states
);

/*
 *  _States_Are_set
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if any of the states in the mask
 *  are set in the_states, and FALSE otherwise.
 */

STATIC INLINE boolean _States_Are_set (
  States_Control the_states,
  States_Control mask
);

#include <rtems/score/states.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
