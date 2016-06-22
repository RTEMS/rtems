/*
 * Copyright (c) 2014, 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERNODE_H
#define _RTEMS_SCORE_SCHEDULERNODE_H

#include <rtems/score/basedefs.h>
#include <rtems/score/chain.h>
#include <rtems/score/priority.h>
#include <rtems/score/smplockseq.h>

struct _Thread_Control;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(RTEMS_SMP)
/**
 * @brief State to indicate potential help for other threads.
 *
 * @dot
 * digraph state {
 *   y [label="HELP YOURSELF"];
 *   ao [label="HELP ACTIVE OWNER"];
 *   ar [label="HELP ACTIVE RIVAL"];
 *
 *   y -> ao [label="obtain"];
 *   y -> ar [label="wait for obtain"];
 *   ao -> y [label="last release"];
 *   ao -> r [label="wait for obtain"];
 *   ar -> r [label="timeout"];
 *   ar -> ao [label="timeout"];
 * }
 * @enddot
 */
typedef enum {
  /**
   * @brief This scheduler node is solely used by the owner thread.
   *
   * This thread owns no resources using a helping protocol and thus does not
   * take part in the scheduler helping protocol.  No help will be provided for
   * other thread.
   */
  SCHEDULER_HELP_YOURSELF,

  /**
   * @brief This scheduler node is owned by a thread actively owning a resource.
   *
   * This scheduler node can be used to help out threads.
   *
   * In case this scheduler node changes its state from ready to scheduled and
   * the thread executes using another node, then an idle thread will be
   * provided as a user of this node to temporarily execute on behalf of the
   * owner thread.  Thus lower priority threads are denied access to the
   * processors of this scheduler instance.
   *
   * In case a thread actively owning a resource performs a blocking operation,
   * then an idle thread will be used also in case this node is in the
   * scheduled state.
   */
  SCHEDULER_HELP_ACTIVE_OWNER,

  /**
   * @brief This scheduler node is owned by a thread actively obtaining a
   * resource currently owned by another thread.
   *
   * This scheduler node can be used to help out threads.
   *
   * The thread owning this node is ready and will give away its processor in
   * case the thread owning the resource asks for help.
   */
  SCHEDULER_HELP_ACTIVE_RIVAL,

  /**
   * @brief This scheduler node is owned by a thread obtaining a
   * resource currently owned by another thread.
   *
   * This scheduler node can be used to help out threads.
   *
   * The thread owning this node is blocked.
   */
  SCHEDULER_HELP_PASSIVE
} Scheduler_Help_state;
#endif

/**
 * @brief Scheduler node for per-thread data.
 */
typedef struct Scheduler_Node {
#if defined(RTEMS_SMP)
  /**
   * @brief Chain node for usage in various scheduler data structures.
   *
   * Strictly this is the wrong place for this field since the data structures
   * to manage scheduler nodes belong to the particular scheduler
   * implementation.  Currently all SMP scheduler implementations use chains.
   * The node is here to simplify things, just like the object node in the
   * thread control block.  It may be replaced with a union to add a red-black
   * tree node in the future.
   */
  Chain_Node Node;

  /**
   * @brief The thread using this node.
   */
  struct _Thread_Control *user;

  /**
   * @brief The help state of this node.
   */
  Scheduler_Help_state help_state;

  /**
   * @brief The idle thread claimed by this node in case the help state is
   * SCHEDULER_HELP_ACTIVE_OWNER.
   *
   * Active owners will lend their own node to an idle thread in case they
   * execute currently using another node or in case they perform a blocking
   * operation.  This is necessary to ensure the priority ceiling protocols
   * work across scheduler boundaries.
   */
  struct _Thread_Control *idle;

  /**
   * @brief The thread accepting help by this node in case the help state is
   * not SCHEDULER_HELP_YOURSELF.
   */
  struct _Thread_Control *accepts_help;
#endif

  /**
   * @brief Thread wait support block.
   */
  struct {
    Priority_Aggregation Priority;
  } Wait;

  /**
   * @brief The thread owning this node.
   */
  struct _Thread_Control *owner;

  /**
   * @brief The thread priority information used by the scheduler.
   *
   * The thread priority is manifest in two independent areas.  One area is the
   * user visible thread priority along with a potential thread queue.  The
   * other is the scheduler.  During a thread priority change, the user visible
   * thread priority and the thread queue are first updated and the thread
   * priority value here is changed.  Once this is done the scheduler is
   * notified via the update priority operation, so that it can update its
   * internal state and honour a new thread priority value.
   */
  struct {
    /**
     * @brief The thread priority value of this scheduler node.
     *
     * The producer of this value is _Thread_Change_priority().  The consumer
     * is the scheduler via the unblock and update priority operations.
     */
    Priority_Control value;

#if defined(RTEMS_SMP)
    /**
     * @brief Sequence lock to synchronize priority value updates.
     */
    SMP_sequence_lock_Control Lock;
#endif

    /**
     * @brief In case a priority update is necessary and this is true, then
     * enqueue the thread as the first of its priority group, otherwise enqueue
     * the thread as the last of its priority group.
     */
    bool prepend_it;
  } Priority;
} Scheduler_Node;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERNODE_H */
