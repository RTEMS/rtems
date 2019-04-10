/**
 * @file
 *
 * @ingroup RTEMSScorePriority
 *
 * @brief Priority Handler API
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2016, 2017 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_PRIORITY_H
#define _RTEMS_SCORE_PRIORITY_H

#include <rtems/score/chain.h>
#include <rtems/score/cpu.h>
#include <rtems/score/rbtree.h>

struct _Scheduler_Control;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScorePriority Priority Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief Priority Handler
 *
 * This handler encapsulates functionality which is used to manage thread
 * priorities.  The actual priority of a thread is an aggregation of priority
 * nodes.  The thread priority aggregation for the home scheduler instance of a
 * thread consists of at least one priority node, which is normally the real
 * priority of the thread.  The locking protocols (e.g. priority ceiling and
 * priority inheritance), rate-monotonic period objects and the POSIX sporadic
 * server add, change and remove priority nodes.
 *
 * @{
 */

/**
 * @brief The thread priority control.
 *
 * Lower values represent higher priorities.  So, a priority value of zero
 * represents the highest priority thread.  This value is reserved for internal
 * threads and the priority ceiling protocol.
 *
 * The format of the thread priority control depends on the context.  A thread
 * priority control may contain a user visible priority for API import/export.
 * It may also contain a scheduler internal priority value.  Values are
 * translated via the scheduler map/unmap priority operations.  The format of
 * scheduler interal values depend on the particular scheduler implementation.
 * It may for example encode a deadline in case of the EDF scheduler.
 *
 * The thread priority control value contained in the scheduler node
 * (Scheduler_Node::Priority::value) uses the least-significant bit to indicate
 * if the thread should be appended or prepended to its priority group, see
 * SCHEDULER_PRIORITY_APPEND().
 */
typedef uint64_t Priority_Control;

/**
 * @brief The highest (most important) thread priority value.
 */
#define PRIORITY_MINIMUM      0

/**
 * @brief The priority value of pseudo-ISR threads.
 *
 * Examples are the MPCI and timer server threads.
 */
#define PRIORITY_PSEUDO_ISR   PRIORITY_MINIMUM

/**
 * @brief The default lowest (least important) thread priority value.
 *
 * This value is CPU port dependent.
 */
#if defined (CPU_PRIORITY_MAXIMUM)
  #define PRIORITY_DEFAULT_MAXIMUM      CPU_PRIORITY_MAXIMUM
#else
  #define PRIORITY_DEFAULT_MAXIMUM      255
#endif

/**
 * @brief The priority node to build up a priority aggregation.
 */
typedef struct {
  /**
   * @brief Node component for a chain or red-black tree.
   */
  union {
    Chain_Node Chain;
    RBTree_Node RBTree;
  } Node;

  /**
   * @brief The priority value of this node.
   */
  Priority_Control priority;
} Priority_Node;

/**
 * @brief The priority action type.
 */
typedef enum {
  PRIORITY_ACTION_ADD,
  PRIORITY_ACTION_CHANGE,
  PRIORITY_ACTION_REMOVE,
  PRIORITY_ACTION_INVALID
} Priority_Action_type;

typedef struct Priority_Aggregation Priority_Aggregation;

/**
 * @brief The priority aggregation.
 *
 * This structure serves two purposes.  Firstly, it provides a place to
 * register priority nodes and reflects the overall priority of its
 * contributors.  Secondly, it provides an action block to signal addition,
 * change and removal of a priority node.
 */
struct Priority_Aggregation {
  /**
   * @brief This priority node reflects the overall priority of the aggregation.
   *
   * The overall priority of the aggregation is the minimum priority of the
   * priority nodes in the contributors tree.
   *
   * This priority node may be used to add this aggregation to another
   * aggregation to build up a recursive priority scheme.
   *
   * In case priority nodes of the contributors tree are added, changed or
   * removed the priority of this node may change.  To signal such changes to a
   * priority aggregation the action block may be used.
   */
  Priority_Node Node;

  /**
   * @brief A red-black tree to contain priority nodes contributing to the
   * overall priority of this priority aggregation.
   */
  RBTree_Control Contributors;

#if defined(RTEMS_SMP)
  /**
   * @brief The scheduler instance of this priority aggregation.
   */
  const struct _Scheduler_Control *scheduler;
#endif

  /**
   * @brief A priority action block to manage priority node additions, changes
   * and removals.
   */
  struct {
#if defined(RTEMS_SMP)
    /**
     * @brief The next priority aggregation in the action list.
     */
    Priority_Aggregation *next;
#endif

    /**
     * @brief The priority node of the action.
     */
    Priority_Node *node;

    /**
     * @brief The type of the action.
     */
    Priority_Action_type type;
  } Action;
};

/**
 * @brief A list of priority actions.
 *
 * Actions are only added to the list.  The action lists reside on the stack
 * and have a short life-time.  They are moved, processed or destroyed as a
 * whole.
 */
typedef struct {
  /**
   * @brief The first action of a priority action list.
   */
  Priority_Aggregation *actions;
} Priority_Actions;

#ifdef __cplusplus
}
#endif

/** @} */

#endif
/* end of include file */
