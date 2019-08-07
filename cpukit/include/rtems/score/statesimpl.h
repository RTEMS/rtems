/**
 * @file
 *
 * @ingroup RTEMSScoreStates
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
 * @addtogroup RTEMSScoreStates
 *
 * @{
 */

/*
 *  The following constants define the individual states which may be
 *  be used to compose and manipulate a thread's state.  More frequently used
 *  states should use lower value bits to ease the use of immediate values on
 *  RISC architectures.
 */

/** This macro corresponds to a task being ready. */
#define STATES_READY                           0x00000000

/** This macro corresponds to a task waiting for a mutex. */
#define STATES_WAITING_FOR_MUTEX               0x00000001

/** This macro corresponds to a task waiting for a semaphore. */
#define STATES_WAITING_FOR_SEMAPHORE           0x00000002

/** This macro corresponds to a task waiting for an event. */
#define STATES_WAITING_FOR_EVENT               0x00000004

/** This macro corresponds to a task waiting for a system event. */
#define STATES_WAITING_FOR_SYSTEM_EVENT        0x00000008

/** This macro corresponds to a task waiting for a message. */
#define STATES_WAITING_FOR_MESSAGE             0x00000010

/** This macro corresponds to a task waiting for a condition variable. */
#define STATES_WAITING_FOR_CONDITION_VARIABLE  0x00000020

/** This macro corresponds to a task waiting for a futex. */
#define STATES_WAITING_FOR_FUTEX               0x00000040

/** This macro corresponds to a task waiting for BSD wakeup. */
#define STATES_WAITING_FOR_BSD_WAKEUP          0x00000080

/**
 * @brief This macro corresponds to a task which is waiting for a relative or
 * absolute timeout.
 */
#define STATES_WAITING_FOR_TIME                0x00000100

/** This macro corresponds to a task waiting for a period. */
#define STATES_WAITING_FOR_PERIOD              0x00000200

/** This macro corresponds to a task waiting for a signal. */
#define STATES_WAITING_FOR_SIGNAL              0x00000400

/** This macro corresponds to a task waiting for a barrier. */
#define STATES_WAITING_FOR_BARRIER             0x00000800

/** This macro corresponds to a task waiting for a RWLock. */
#define STATES_WAITING_FOR_RWLOCK              0x00001000

/** This macro corresponds to a task waiting for a join while exiting. */
#define STATES_WAITING_FOR_JOIN_AT_EXIT        0x00002000

/** This macro corresponds to a task waiting for a join. */
#define STATES_WAITING_FOR_JOIN                0x00004000

/** This macro corresponds to a task being suspended. */
#define STATES_SUSPENDED                       0x00008000

/** This macro corresponds to a task waiting for a fixed size segment. */
#define STATES_WAITING_FOR_SEGMENT             0x00010000

/** This macro corresponds to a task those life is changing. */
#define STATES_LIFE_IS_CHANGING                0x00020000

/** This macro corresponds to a task being held by the debugger. */
#define STATES_DEBUGGER                        0x08000000

/** This macro corresponds to a task which is in an interruptible
 *  blocking state.
 */
#define STATES_INTERRUPTIBLE_BY_SIGNAL         0x10000000

/** This macro corresponds to a task waiting for a reply to an MPCI request. */
#define STATES_WAITING_FOR_RPC_REPLY           0x20000000

/** This macro corresponds to a task being a zombie. */
#define STATES_ZOMBIE                          0x40000000

/** This macro corresponds to a task being created but not yet started. */
#define STATES_DORMANT                         0x80000000

/** This macro corresponds to a task waiting for a local object operation. */
#define STATES_LOCALLY_BLOCKED ( STATES_WAITING_FOR_SEGMENT            | \
                                 STATES_WAITING_FOR_MESSAGE            | \
                                 STATES_WAITING_FOR_SEMAPHORE          | \
                                 STATES_WAITING_FOR_MUTEX              | \
                                 STATES_WAITING_FOR_CONDITION_VARIABLE | \
                                 STATES_WAITING_FOR_JOIN               | \
                                 STATES_WAITING_FOR_SIGNAL             | \
                                 STATES_WAITING_FOR_BARRIER            | \
                                 STATES_WAITING_FOR_BSD_WAKEUP         | \
                                 STATES_WAITING_FOR_FUTEX              | \
                                 STATES_WAITING_FOR_RWLOCK             )

/** This macro corresponds to a task waiting which is blocked. */
#define STATES_BLOCKED         ( STATES_LOCALLY_BLOCKED         | \
                                 STATES_WAITING_FOR_TIME        | \
                                 STATES_WAITING_FOR_PERIOD      | \
                                 STATES_WAITING_FOR_EVENT       | \
                                 STATES_WAITING_FOR_RPC_REPLY   | \
                                 STATES_WAITING_FOR_SYSTEM_EVENT | \
                                 STATES_INTERRUPTIBLE_BY_SIGNAL )

/** All state bits set to one (provided for _Thread_Start()) */
#define STATES_ALL_SET 0xffffffff

/**
 * @brief Returns the result of setting the states to set to the given state.
 *
 * This function sets the given @a states_to_set into the @a current_state
 * passed in.  The result is returned to the user in @a current_state.
 *
 * @param states_to_set The state bits to set.
 * @param current_state The state set to add them to.
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
 * @brief Clears the states into the passed current state and returns the result.
 *
 * This function clears the given @a states_to_clear into the @a current_state
 * passed in.  The result is returned to the user in @a current_state.
 *
 * @param states_to_clear The state bits to clear.
 * @param current_state The state set to remove them from.
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
 * @brief Checks if the state is ready.
 *
 * This function returns true if the_states indicates that the
 * state is READY, and false otherwise.
 *
 * @param the_states The task state set to test.
 *
 * @retval true The state is ready.
 * @retval false The state is not ready.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_ready (
  States_Control the_states
)
{
   return (the_states == STATES_READY);
}

/**
 * @brief Checks if DORMANT state is set.
 *
 * This function returns true if the DORMANT state is set in
 * @a the_states, and false otherwise.
 *
 * @param the_states The task state set to test.
 *
 * @retval true DORMANT state is set in @a the_states.
 * @retval false DORMANT state is not set in @a the_states.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_dormant (
  States_Control the_states
)
{
   return (the_states & STATES_DORMANT);
}

/**
 * @brief Checks if SUSPENDED state is set.
 *
 * This function returns true if the SUSPENDED state is set in
 * @a the_states, and false otherwise.
 *
 * @param the_states The task state set to test.
 *
 * @retval true SUSPENDED state is set in @a the_states.
 * @retval false SUSPENDED state is not set in @a the_states.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_suspended (
  States_Control the_states
)
{
   return (the_states & STATES_SUSPENDED);
}

/**
 * @brief Checks if WAITING_FOR_TIME state is set.
 *
 * This function returns true if the WAITING_FOR_TIME state is set in
 * @a the_states, and false otherwise.
 *
 * @param the_states The task state set to test.
 *
 * @retval true WAITING_FOR_TIME state is set in @a the_states.
 * @retval false WAITING_FOR_TIME state is not set in @a the_states.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_rpc_reply (
  States_Control the_states
)
{
   return (the_states & STATES_WAITING_FOR_RPC_REPLY);
}

/**
 * @brief Checks if WAITING_FOR_JOIN_AT_EXIT state is set.
 *
 * This function returns true if the WAITING_FOR_JOIN_AT_EXIT state is set in
 * @a the_states, and false otherwise.
 *
 * @param the_states The task state set to test.
 *
 * @retval true WAITING_FOR_JOIN_AT_EXIT state is set in @a the_states.
 * @retval false WAITING_FOR_JOIN_AT_EXIT state is not set in @a the_states.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_waiting_for_join_at_exit(
  States_Control the_states
)
{
   return ( the_states & STATES_WAITING_FOR_JOIN_AT_EXIT ) != 0;
}

/**
 * @brief Checks if the state is set to be interruptible.
 *
 * This function returns true if the task's state is set in
 * way that allows it to be interrupted by a signal.
 *
 * @param the_states The task state set to test.
 *
 * @retval true @a the_states is interruptible.
 * @retval false @a the_states is not interruptible.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_interruptible_by_signal (
  States_Control the_states
)
{
   return (the_states & STATES_INTERRUPTIBLE_BY_SIGNAL);
}

/**
 * @brief Checks if the state is blocked waiting on a local resource.
 *
 * This function returns true if one of the states which indicates
 * that a task is blocked waiting for a local resource is set in
 * the_states, and false otherwise.
 *
 * @param the_states The task state set to test.
 *
 * @retval true The state indicates that the task is blocked waiting on a local
 *      resource.
 * @retval false The state indicates that the task is not blocked waiting on a
 *      local resource.
 */
RTEMS_INLINE_ROUTINE bool _States_Is_locally_blocked (
  States_Control the_states
)
{
   return (the_states & STATES_LOCALLY_BLOCKED);
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
