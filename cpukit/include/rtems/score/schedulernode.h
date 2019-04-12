/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief Handles Scheduler Nodes.
 */

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

/**
 * @addtogroup RTEMSScoreScheduler
 *
 * @{
 */

struct _Thread_Control;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(RTEMS_SMP)
/**
 * @brief The scheduler node requests.
 */
typedef enum {
  /**
   * @brief The scheduler node is not on the list of pending requests.
   */
  SCHEDULER_NODE_REQUEST_NOT_PENDING,

  /**
   * @brief There is a pending scheduler node request to add this scheduler
   * node to the Thread_Control::Scheduler::Scheduler_nodes chain.
   */
  SCHEDULER_NODE_REQUEST_ADD,

  /**
   * @brief There is a pending scheduler node request to remove this scheduler
   * node from the Thread_Control::Scheduler::Scheduler_nodes chain.
   */
  SCHEDULER_NODE_REQUEST_REMOVE,

  /**
   * @brief The scheduler node is on the list of pending requests, but nothing
   * should change.
   */
  SCHEDULER_NODE_REQUEST_NOTHING,

} Scheduler_Node_request;
#endif

typedef struct Scheduler_Node Scheduler_Node;

/**
 * @brief Scheduler node for per-thread data.
 */
struct Scheduler_Node {
#if defined(RTEMS_SMP)
  /**
   * @brief Chain node for usage in various scheduler data structures.
   *
   * Strictly, this is the wrong place for this field since the data structures
   * to manage scheduler nodes belong to the particular scheduler
   * implementation.  Currently, all SMP scheduler implementations use chains
   * or red-black trees.  The node is here to simplify things, just like the
   * object node in the thread control block.
   */
  union {
    Chain_Node Chain;
    RBTree_Node RBTree;
  } Node;

  /**
   * @brief The sticky level determines if this scheduler node should use an
   * idle thread in case this node is scheduled and the owner thread is
   * blocked.
   */
  int sticky_level;

  /**
   * @brief The thread using this node.
   *
   * This is either the owner or an idle thread.
   */
  struct _Thread_Control *user;

  /**
   * @brief The idle thread claimed by this node in case the sticky level is
   * greater than zero and the thread is block or is scheduled on another
   * scheduler instance.
   *
   * This is necessary to ensure the priority ceiling protocols work across
   * scheduler boundaries.
   */
  struct _Thread_Control *idle;
#endif

  /**
   * @brief The thread owning this node.
   */
  struct _Thread_Control *owner;

#if defined(RTEMS_SMP)
  /**
   * @brief Block to register and manage this scheduler node in the thread
   * control block of the owner of this scheduler node.
   */
  struct {
    /**
     * @brief Node to add this scheduler node to
     * Thread_Control::Scheduler::Wait_nodes.
     */
    Chain_Node Wait_node;

    /**
     * @brief Node to add this scheduler node to
     * Thread_Control::Scheduler::Scheduler_nodes or a temporary remove list.
     */
    union {
      /**
       * @brief The node for Thread_Control::Scheduler::Scheduler_nodes.
       */
      Chain_Node Chain;

      /**
       * @brief The next pointer for a temporary remove list.
       *
       * @see _Thread_Scheduler_process_requests().
       */
      Scheduler_Node *next;
    } Scheduler_node;

    /**
     * @brief Link to the next scheduler node in the
     * Thread_Control::Scheduler::requests list.
     */
    Scheduler_Node *next_request;

    /**
     * @brief The current scheduler node request.
     */
    Scheduler_Node_request request;
  } Thread;
#endif

  /**
   * @brief Thread wait support block.
   */
  struct {
    Priority_Aggregation Priority;
  } Wait;

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
     *
     * This priority control consists of two parts.  One part is the plain
     * priority value (most-significant 63 bits).  The other part is the
     * least-significant bit which indicates if the thread should be appended
     * (bit set) or prepended (bit cleared) to its priority group, see
     * SCHEDULER_PRIORITY_APPEND().
     */
    Priority_Control value;

#if defined(RTEMS_SMP)
    /**
     * @brief Sequence lock to synchronize priority value updates.
     */
    SMP_sequence_lock_Control Lock;
#endif
  } Priority;
};

#if defined(RTEMS_SMP)
/**
 * @brief The size of a scheduler node.
 *
 * This value is provided via <rtems/confdefs.h>.
 */
extern const size_t _Scheduler_Node_size;
#endif

#if defined(RTEMS_SMP)
#define SCHEDULER_NODE_OF_THREAD_WAIT_NODE( node ) \
  RTEMS_CONTAINER_OF( node, Scheduler_Node, Thread.Wait_node )

#define SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE( node ) \
  RTEMS_CONTAINER_OF( node, Scheduler_Node, Thread.Scheduler_node.Chain )
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

/** @} */

#endif /* _RTEMS_SCORE_SCHEDULERNODE_H */
