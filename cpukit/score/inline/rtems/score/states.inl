/*  states.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines associated with thread state information.
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

#ifndef __STATES_inl
#define __STATES_inl

/*PAGE
 *
 *  _States_Set
 *
 */

STATIC INLINE States_Control _States_Set (
  States_Control states_to_set,
  States_Control current_state
)
{
   return (current_state | states_to_set);
}

/*PAGE
 *
 *  _States_Clear
 *
 */

STATIC INLINE States_Control _States_Clear (
  States_Control states_to_clear,
  States_Control current_state
)
{
   return (current_state & ~states_to_clear);
}

/*PAGE
 *
 *  _States_Is_ready
 *
 */

STATIC INLINE boolean _States_Is_ready (
  States_Control the_states
)
{
   return (the_states == STATES_READY);
}

/*PAGE
 *
 *  _States_Is_only_dormant
 *
 */

STATIC INLINE boolean _States_Is_only_dormant (
  States_Control the_states
)
{
   return (the_states == STATES_DORMANT);
}

/*PAGE
 *
 *  _States_Is_dormant
 *
 */

STATIC INLINE boolean _States_Is_dormant (
  States_Control the_states
)
{
   return (the_states & STATES_DORMANT);
}

/*PAGE
 *
 *  _States_Is_suspended
 *
 */

STATIC INLINE boolean _States_Is_suspended (
  States_Control the_states
)
{
   return (the_states & STATES_SUSPENDED);
}

/*PAGE
 *
 *  _States_Is_Transient
 *
 */

STATIC INLINE boolean _States_Is_transient (
  States_Control the_states
)
{
   return (the_states & STATES_TRANSIENT);
}

/*PAGE
 *
 *  _States_Is_delaying
 *
 */

STATIC INLINE boolean _States_Is_delaying (
  States_Control the_states
)
{
   return (the_states & STATES_DELAYING);
}

/*PAGE
 *
 *  _States_Is_waiting_for_buffer
 *
 */

STATIC INLINE boolean _States_Is_waiting_for_buffer (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_BUFFER);
}

/*PAGE
 *
 *  _States_Is_waiting_for_segment
 *
 */

STATIC INLINE boolean _States_Is_waiting_for_segment (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_SEGMENT);
}

/*PAGE
 *
 *  _States_Is_waiting_for_message
 *
 */

STATIC INLINE boolean _States_Is_waiting_for_message (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_MESSAGE);
}

/*PAGE
 *
 *  _States_Is_waiting_for_event
 *
 */

STATIC INLINE boolean _States_Is_waiting_for_event (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_EVENT);
}

/*PAGE
 *
 *  _States_Is_waiting_for_mutex
 *
 */
 
STATIC INLINE boolean _States_Is_waiting_for_mutex (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_MUTEX);
}

/*PAGE
 *
 *  _States_Is_waiting_for_semaphore
 *
 */

STATIC INLINE boolean _States_Is_waiting_for_semaphore (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_SEMAPHORE);
}

/*PAGE
 *
 *  _States_Is_waiting_for_time
 *
 */

STATIC INLINE boolean _States_Is_waiting_for_time (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_TIME);
}

/*PAGE
 *
 *  _States_Is_waiting_for_rpc_reply
 *
 */

STATIC INLINE boolean _States_Is_waiting_for_rpc_reply (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_RPC_REPLY);
}

/*PAGE
 *
 *  _States_Is_waiting_for_period
 *
 */

STATIC INLINE boolean _States_Is_waiting_for_period (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_PERIOD);
}

/*PAGE
 *
 *  _States_Is_locally_blocked
 *
 */

STATIC INLINE boolean _States_Is_locally_blocked (
  States_Control the_states
)
{
   return (the_states & STATES_LOCALLY_BLOCKED);
}

/*PAGE
 *
 *  _States_Is_waiting_on_thread_queue
 *
 */

STATIC INLINE boolean _States_Is_waiting_on_thread_queue (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_ON_THREAD_QUEUE);
}

/*PAGE
 *
 *  _States_Is_blocked
 *
 */

STATIC INLINE boolean _States_Is_blocked (
  States_Control the_states
)
{
   return (the_states & STATES_BLOCKED);
}

/*PAGEPAGE
 *
 *
 *  _States_Are_set
 *
 */

STATIC INLINE boolean _States_Are_set (
  States_Control the_states,
  States_Control mask
)
{
   return ( (the_states & mask) != STATES_READY);
}

#endif
/* end of include file */
