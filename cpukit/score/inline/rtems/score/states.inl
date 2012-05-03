/** 
 *  @file  rtems/score/states.inl
 *
 *  This file contains the static inline implementation of the inlined
 *  routines associated with thread state information.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_STATES_H
# error "Never use <rtems/score/states.inl> directly; include <rtems/score/states.h> instead."
#endif

#ifndef _RTEMS_SCORE_STATES_INL
#define _RTEMS_SCORE_STATES_INL

#include <rtems/score/basedefs.h> /* RTEMS_INLINE_ROUTINE */

/**
 *  @addtogroup ScoreStates 
 *  @{
 */

/**
 *  This function sets the given states_to_set into the current_state
 *  passed in.  The result is returned to the user in current_state.
 *
 *  @param[in] states_to_set is the state bits to set
 *  @param[in] current_state is the state set to add them to
 *
 *  @return This method returns the updated states value.
 */
RTEMS_INLINE_ROUTINE States_Control _States_Set (
  States_Control states_to_set,
  States_Control current_state
)
{
   return (current_state | states_to_set);
}

/**
 *  This function clears the given states_to_clear into the current_state
 *  passed in.  The result is returned to the user in current_state.
 *
 *  @param[in] states_to_set is the state bits to clean
 *  @param[in] current_state is the state set to remove them from
 *
 *  @return This method returns the updated states value.
 */
RTEMS_INLINE_ROUTINE States_Control _States_Clear (
  States_Control states_to_clear,
  States_Control current_state
)
{
   return (current_state & ~states_to_clear);
}

/**
 *  This function returns true if the_states indicates that the
 *  state is READY, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_ready (
  States_Control the_states
)
{
   return (the_states == STATES_READY);
}

/**
 *  This function returns true if the DORMANT state is the ONLY state
 *  set in the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_only_dormant (
  States_Control the_states
)
{
   return (the_states == STATES_DORMANT);
}

/**
 *  This function returns true if the DORMANT state is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_dormant (
  States_Control the_states
)
{
   return (the_states & STATES_DORMANT);
}

/**
 *  This function returns true if the SUSPENDED state is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_suspended (
  States_Control the_states
)
{
   return (the_states & STATES_SUSPENDED);
}

/**
 *  This function returns true if the TRANSIENT state is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_transient (
  States_Control the_states
)
{
   return (the_states & STATES_TRANSIENT);
}

/**
 *  This function returns true if the DELAYING state is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_delaying (
  States_Control the_states
)
{
   return (the_states & STATES_DELAYING);
}

/**
 *  This function returns true if the WAITING_FOR_BUFFER state is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_buffer (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_BUFFER);
}

/**
 *  This function returns true if the WAITING_FOR_SEGMENT state is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_segment (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_SEGMENT);
}

/**
 *  This function returns true if the WAITING_FOR_MESSAGE state is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_message (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_MESSAGE);
}

/**
 *  This function returns true if the WAITING_FOR_EVENT state is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_event (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_EVENT);
}

/**
 *  This function returns true if the WAITING_FOR_MUTEX state
 *  is set in the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_mutex (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_MUTEX);
}

/**
 *  This function returns true if the WAITING_FOR_SEMAPHORE state
 *  is set in the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_semaphore (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_SEMAPHORE);
}

/**
 *  This function returns true if the WAITING_FOR_TIME state is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_time (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_TIME);
}

/**
 *  This function returns true if the WAITING_FOR_TIME state is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_rpc_reply (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_RPC_REPLY);
}

/**
 *  This function returns true if the WAITING_FOR_PERIOD state is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_period (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_PERIOD);
}

/**
 *  This function returns true if the task's state is set in
 *  way that allows it to be interrupted by a signal.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_interruptible_by_signal (
  States_Control the_states
)
{
   return (the_states & STATES_INTERRUPTIBLE_BY_SIGNAL);

}
/**
 *  This function returns true if one of the states which indicates
 *  that a task is blocked waiting for a local resource is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the desired state condition is set.
 */

RTEMS_INLINE_ROUTINE bool _States_Is_locally_blocked (
  States_Control the_states
)
{
   return (the_states & STATES_LOCALLY_BLOCKED);
}

/**
 *  This function returns true if one of the states which indicates
 *  that a task is blocked waiting for a local resource is set in
 *  the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the state indicates that the
 *          assocated thread is waiting on a thread queue.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_on_thread_queue (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_ON_THREAD_QUEUE);
}

/**
 *  This function returns true if one of the states which indicates
 *  that a task is blocked is set in the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *
 *  @return This method returns true if the state indicates that the
 *          assocated thread is blocked.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_blocked (
  States_Control the_states
)
{
   return (the_states & STATES_BLOCKED);
}

/**
 *  This function returns true if any of the states in the mask
 *  are set in the_states, and false otherwise.
 *
 *  @param[in] the_states is the task state set to test
 *  @param[in] mask is the state bits to test for
 *
 *  @return This method returns true if the indicates state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Are_set (
  States_Control the_states,
  States_Control mask
)
{
   return ( (the_states & mask) != STATES_READY);
}

/**@}*/

#endif
/* end of include file */
