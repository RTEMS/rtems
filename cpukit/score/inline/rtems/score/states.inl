/*  states.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines associated with thread state information.
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

#ifndef __STATES_inl
#define __STATES_inl

/*PAGE
 *
 *  _States_Set
 *
 *  DESCRIPTION:
 *
 *  This function sets the given states_to_set into the current_state
 *  passed in.  The result is returned to the user in current_state.
 */

RTEMS_INLINE_ROUTINE States_Control _States_Set (
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
 *  DESCRIPTION:
 *
 *  This function clears the given states_to_clear into the current_state
 *  passed in.  The result is returned to the user in current_state.
 */

RTEMS_INLINE_ROUTINE States_Control _States_Clear (
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
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_states indicates that the
 *  state is READY, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_ready (
  States_Control the_states
)
{
   return (the_states == STATES_READY);
}

/*PAGE
 *
 *  _States_Is_only_dormant
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the DORMANT state is the ONLY state
 *  set in the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_only_dormant (
  States_Control the_states
)
{
   return (the_states == STATES_DORMANT);
}

/*PAGE
 *
 *  _States_Is_dormant
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the DORMANT state is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_dormant (
  States_Control the_states
)
{
   return (the_states & STATES_DORMANT);
}

/*PAGE
 *
 *  _States_Is_suspended
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the SUSPENDED state is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_suspended (
  States_Control the_states
)
{
   return (the_states & STATES_SUSPENDED);
}

/*PAGE
 *
 *  _States_Is_Transient
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the TRANSIENT state is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_transient (
  States_Control the_states
)
{
   return (the_states & STATES_TRANSIENT);
}

/*PAGE
 *
 *  _States_Is_delaying
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the DELAYING state is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_delaying (
  States_Control the_states
)
{
   return (the_states & STATES_DELAYING);
}

/*PAGE
 *
 *  _States_Is_waiting_for_buffer
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_BUFFER state is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_for_buffer (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_BUFFER);
}

/*PAGE
 *
 *  _States_Is_waiting_for_segment
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_SEGMENT state is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_for_segment (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_SEGMENT);
}

/*PAGE
 *
 *  _States_Is_waiting_for_message
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_MESSAGE state is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_for_message (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_MESSAGE);
}

/*PAGE
 *
 *  _States_Is_waiting_for_event
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_EVENT state is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_for_event (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_EVENT);
}

/*PAGE
 *
 *  _States_Is_waiting_for_mutex
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_MUTEX state
 *  is set in the_states, and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_for_mutex (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_MUTEX);
}

/*PAGE
 *
 *  _States_Is_waiting_for_semaphore
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_SEMAPHORE state
 *  is set in the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_for_semaphore (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_SEMAPHORE);
}

/*PAGE
 *
 *  _States_Is_waiting_for_time
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_TIME state is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_for_time (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_TIME);
}

/*PAGE
 *
 *  _States_Is_waiting_for_rpc_reply
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_TIME state is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_for_rpc_reply (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_RPC_REPLY);
}

/*PAGE
 *
 *  _States_Is_waiting_for_period
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the WAITING_FOR_PERIOD state is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_for_period (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_PERIOD);
}

/*PAGE
 *
 *  _States_Is_locally_blocked
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if one of the states which indicates
 *  that a task is blocked waiting for a local resource is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_locally_blocked (
  States_Control the_states
)
{
   return (the_states & STATES_LOCALLY_BLOCKED);
}

/*PAGE
 *
 *  _States_Is_waiting_on_thread_queue
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if one of the states which indicates
 *  that a task is blocked waiting for a local resource is set in
 *  the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_waiting_on_thread_queue (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_ON_THREAD_QUEUE);
}

/*PAGE
 *
 *  _States_Is_blocked
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if one of the states which indicates
 *  that a task is blocked is set in the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Is_blocked (
  States_Control the_states
)
{
   return (the_states & STATES_BLOCKED);
}

/*PAGE
 *
 *
 *  _States_Are_set
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if any of the states in the mask
 *  are set in the_states, and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _States_Are_set (
  States_Control the_states,
  States_Control mask
)
{
   return ( (the_states & mask) != STATES_READY);
}

#endif
/* end of include file */
