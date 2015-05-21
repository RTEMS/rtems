/**
 * @file
 *
 * @brief Inlined Routines Associated with Thread State Information
 *
 * This file contains the static inline implementation of the inlined
 * routines associated with thread state information.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_STATESIMPL_H
#define _RTEMS_SCORE_STATESIMPL_H

#include <rtems/score/states.h>
#include <rtems/score/basedefs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreStates
 */
/**@{**/

/*
 *  The following constants define the individual states which may be
 *  be used to compose and manipulate a thread's state.
 */

/** This macro corresponds to a task being ready. */
#define STATES_READY                           0x00000
/** This macro corresponds to a task being created but not yet started. */
#define STATES_DORMANT                         0x00001
/** This macro corresponds to a task being suspended. */
#define STATES_SUSPENDED                       0x00002
/** This macro corresponds to a task which is waiting for a timeout. */
#define STATES_DELAYING                        0x00008
/** This macro corresponds to a task waiting until a specific TOD. */
#define STATES_WAITING_FOR_TIME                0x00010
/** This macro corresponds to a task waiting for a variable length buffer. */
#define STATES_WAITING_FOR_BUFFER              0x00020
/** This macro corresponds to a task waiting for a fixed size segment. */
#define STATES_WAITING_FOR_SEGMENT             0x00040
/** This macro corresponds to a task waiting for a message. */
#define STATES_WAITING_FOR_MESSAGE             0x00080
/** This macro corresponds to a task waiting for an event. */
#define STATES_WAITING_FOR_EVENT               0x00100
/** This macro corresponds to a task waiting for a semaphore. */
#define STATES_WAITING_FOR_SEMAPHORE           0x00200
/** This macro corresponds to a task waiting for a mutex. */
#define STATES_WAITING_FOR_MUTEX               0x00400
/** This macro corresponds to a task waiting for a condition variable. */
#define STATES_WAITING_FOR_CONDITION_VARIABLE  0x00800
/** This macro corresponds to a task waiting for a join while exiting. */
#define STATES_WAITING_FOR_JOIN_AT_EXIT        0x01000
/** This macro corresponds to a task waiting for a reply to an MPCI request. */
#define STATES_WAITING_FOR_RPC_REPLY           0x02000
/** This macro corresponds to a task waiting for a period. */
#define STATES_WAITING_FOR_PERIOD              0x04000
/** This macro corresponds to a task waiting for a signal. */
#define STATES_WAITING_FOR_SIGNAL              0x08000
/** This macro corresponds to a task waiting for a barrier. */
#define STATES_WAITING_FOR_BARRIER             0x10000
/** This macro corresponds to a task waiting for a RWLock. */
#define STATES_WAITING_FOR_RWLOCK              0x20000
/** This macro corresponds to a task waiting for a system event. */
#define STATES_WAITING_FOR_SYSTEM_EVENT        0x40000
/** This macro corresponds to a task waiting for BSD wakeup. */
#define STATES_WAITING_FOR_BSD_WAKEUP          0x80000
/** This macro corresponds to a task waiting for a task termination. */
#define STATES_WAITING_FOR_TERMINATION         0x100000
/** This macro corresponds to a task being a zombie. */
#define STATES_ZOMBIE                          0x200000
/** This macro corresponds to a task migrating to another scheduler. */
#define STATES_MIGRATING                       0x400000
/** This macro corresponds to a task restarting. */
#define STATES_RESTARTING                      0x800000
/** This macro corresponds to a task waiting for a join. */
#define STATES_WAITING_FOR_JOIN                0x1000000

/** This macro corresponds to a task which is in an interruptible
 *  blocking state.
 */
#define STATES_INTERRUPTIBLE_BY_SIGNAL         0x10000000

/** This macro corresponds to a task waiting for a local object operation. */
#define STATES_LOCALLY_BLOCKED ( STATES_WAITING_FOR_BUFFER             | \
                                 STATES_WAITING_FOR_SEGMENT            | \
                                 STATES_WAITING_FOR_MESSAGE            | \
                                 STATES_WAITING_FOR_SEMAPHORE          | \
                                 STATES_WAITING_FOR_MUTEX              | \
                                 STATES_WAITING_FOR_CONDITION_VARIABLE | \
                                 STATES_WAITING_FOR_JOIN               | \
                                 STATES_WAITING_FOR_SIGNAL             | \
                                 STATES_WAITING_FOR_BARRIER            | \
                                 STATES_WAITING_FOR_BSD_WAKEUP         | \
                                 STATES_WAITING_FOR_RWLOCK             )

/** This macro corresponds to a task waiting which is blocked. */
#define STATES_BLOCKED         ( STATES_DELAYING                | \
                                 STATES_LOCALLY_BLOCKED         | \
                                 STATES_WAITING_FOR_TIME        | \
                                 STATES_WAITING_FOR_PERIOD      | \
                                 STATES_WAITING_FOR_EVENT       | \
                                 STATES_WAITING_FOR_RPC_REPLY   | \
                                 STATES_WAITING_FOR_SYSTEM_EVENT | \
                                 STATES_INTERRUPTIBLE_BY_SIGNAL )

/** All state bits set to one (provided for _Thread_Ready()) */
#define STATES_ALL_SET 0xffffffff

/**
 * This function sets the given states_to_set into the current_state
 * passed in.  The result is returned to the user in current_state.
 *
 * @param[in] states_to_set is the state bits to set
 * @param[in] current_state is the state set to add them to
 *
 * @return This method returns the updated states value.
 */
RTEMS_INLINE_ROUTINE States_Control _States_Set (
  States_Control states_to_set,
  States_Control current_state
)
{
   return (current_state | states_to_set);
}

/**
 * This function clears the given states_to_clear into the current_state
 * passed in.  The result is returned to the user in current_state.
 *
 * @param[in] states_to_clear is the state bits to clean
 * @param[in] current_state is the state set to remove them from
 *
 * @return This method returns the updated states value.
 */
RTEMS_INLINE_ROUTINE States_Control _States_Clear (
  States_Control states_to_clear,
  States_Control current_state
)
{
   return (current_state & ~states_to_clear);
}

/**
 * This function returns true if the_states indicates that the
 * state is READY, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_ready (
  States_Control the_states
)
{
   return (the_states == STATES_READY);
}

/**
 * This function returns true if the DORMANT state is the ONLY state
 * set in the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_only_dormant (
  States_Control the_states
)
{
   return (the_states == STATES_DORMANT);
}

/**
 * This function returns true if the DORMANT state is set in
 * the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_dormant (
  States_Control the_states
)
{
   return (the_states & STATES_DORMANT);
}

/**
 * This function returns true if the SUSPENDED state is set in
 * the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_suspended (
  States_Control the_states
)
{
   return (the_states & STATES_SUSPENDED);
}

/**
 * This function returns true if the DELAYING state is set in
 * the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_delaying (
  States_Control the_states
)
{
   return (the_states & STATES_DELAYING);
}

/**
 * This function returns true if the WAITING_FOR_BUFFER state is set in
 * the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_buffer (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_BUFFER);
}

/**
 * This function returns true if the WAITING_FOR_SEGMENT state is set in
 * the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_segment (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_SEGMENT);
}

/**
 * This function returns true if the WAITING_FOR_MESSAGE state is set in
 * the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_message (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_MESSAGE);
}

/**
 * This function returns true if the WAITING_FOR_EVENT state is set in
 * the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_event (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_EVENT);
}

/**
 * This function returns true if the WAITING_FOR_SYSTEM_EVENT state is set in
 * the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_system_event (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_SYSTEM_EVENT);
}

/**
 * This function returns true if the WAITING_FOR_MUTEX state
 * is set in the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_mutex (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_MUTEX);
}

/**
 * This function returns true if the WAITING_FOR_SEMAPHORE state
 * is set in the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_semaphore (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_SEMAPHORE);
}

/**
 * This function returns true if the WAITING_FOR_TIME state is set in
 * the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_time (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_TIME);
}

/**
 * This function returns true if the WAITING_FOR_TIME state is set in
 * the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_rpc_reply (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_RPC_REPLY);
}

/**
 * This function returns true if the WAITING_FOR_PERIOD state is set in
 * the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_period (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_PERIOD);
}

/**
 * This function returns true if the task's state is set in
 * way that allows it to be interrupted by a signal.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_interruptible_by_signal (
  States_Control the_states
)
{
   return (the_states & STATES_INTERRUPTIBLE_BY_SIGNAL);

}
/**
 * This function returns true if one of the states which indicates
 * that a task is blocked waiting for a local resource is set in
 * the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the desired state condition is set.
 */

RTEMS_INLINE_ROUTINE bool _States_Is_locally_blocked (
  States_Control the_states
)
{
   return (the_states & STATES_LOCALLY_BLOCKED);
}

/**
 * This function returns true if one of the states which indicates
 * that a task is blocked is set in the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 *
 * @return This method returns true if the state indicates that the
 *         assocated thread is blocked.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_blocked (
  States_Control the_states
)
{
   return (the_states & STATES_BLOCKED);
}

/**
 * This function returns true if any of the states in the mask
 * are set in the_states, and false otherwise.
 *
 * @param[in] the_states is the task state set to test
 * @param[in] mask is the state bits to test for
 *
 * @return This method returns true if the indicates state condition is set.
 */
RTEMS_INLINE_ROUTINE bool _States_Are_set (
  States_Control the_states,
  States_Control mask
)
{
   return ( (the_states & mask) != STATES_READY);
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
