/**
 *  @file  rtems/score/scheduler.h
 *
 *  @brief Constants and Structures Associated with the Scheduler
 *
 *  This include file contains all the constants and structures associated
 *  with the scheduler.
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULER_H
#define _RTEMS_SCORE_SCHEDULER_H

#include <rtems/score/priority.h>
#include <rtems/score/thread.h>
#if defined(__RTEMS_HAVE_SYS_CPUSET_H__) && defined(RTEMS_SMP)
  #include <sys/cpuset.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct Per_CPU_Control;

/**
 *  @defgroup ScoreScheduler Scheduler Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality related to managing sets of threads
 *  that are ready for execution.
 */
/**@{*/

typedef struct Scheduler_Control Scheduler_Control;

typedef struct Scheduler_Node Scheduler_Node;

#if defined(RTEMS_SMP)
  typedef Thread_Control * Scheduler_Void_or_thread;

  #define SCHEDULER_RETURN_VOID_OR_NULL return NULL
#else
  typedef void Scheduler_Void_or_thread;

  #define SCHEDULER_RETURN_VOID_OR_NULL return
#endif

/**
 * @brief The scheduler operations.
 */
typedef struct {
  /** @see _Scheduler_Handler_initialization() */
  void ( *initialize )( const Scheduler_Control * );

  /** @see _Scheduler_Schedule() */
  void ( *schedule )( const Scheduler_Control *, Thread_Control *);

  /** @see _Scheduler_Yield() */
  Scheduler_Void_or_thread ( *yield )(
    const Scheduler_Control *,
    Thread_Control *
  );

  /** @see _Scheduler_Block() */
  void ( *block )(
    const Scheduler_Control *,
    Thread_Control *
  );

  /** @see _Scheduler_Unblock() */
  Scheduler_Void_or_thread ( *unblock )(
    const Scheduler_Control *,
    Thread_Control *
  );

  /** @see _Scheduler_Change_priority() */
  Scheduler_Void_or_thread ( *change_priority )(
    const Scheduler_Control *,
    Thread_Control *,
    Priority_Control,
    bool
  );

#if defined(RTEMS_SMP)
  /**
   * Ask for help operation.
   *
   * @param[in] scheduler The scheduler of the thread offering help.
   * @param[in] offers_help The thread offering help.
   * @param[in] needs_help The thread needing help.
   *
   * @retval needs_help It was not possible to schedule the thread needing
   *   help, so it is returned to continue the search for help.
   * @retval next_needs_help It was possible to schedule the thread needing
   *   help, but this displaced another thread eligible to ask for help.  So
   *   this thread is returned to start a new search for help.
   * @retval NULL It was possible to schedule the thread needing help, and no
   *   other thread needs help as a result.
   *
   * @see _Scheduler_Ask_for_help().
   */
  Thread_Control *( *ask_for_help )(
    const Scheduler_Control *scheduler,
    Thread_Control          *offers_help,
    Thread_Control          *needs_help
  );
#endif

  /** @see _Scheduler_Node_initialize() */
  void ( *node_initialize )( const Scheduler_Control *, Thread_Control * );

  /** @see _Scheduler_Node_destroy() */
  void ( *node_destroy )( const Scheduler_Control *, Thread_Control * );

  /** @see _Scheduler_Update_priority() */
  void ( *update_priority )(
    const Scheduler_Control *,
    Thread_Control *,
    Priority_Control
  );

  /** @see _Scheduler_Priority_compare() */
  int ( *priority_compare )(
    Priority_Control,
    Priority_Control
  );

  /** @see _Scheduler_Release_job() */
  void ( *release_job ) (
    const Scheduler_Control *,
    Thread_Control *,
    uint32_t
  );

  /** @see _Scheduler_Tick() */
  void ( *tick )( const Scheduler_Control *, Thread_Control * );

  /** @see _Scheduler_Start_idle() */
  void ( *start_idle )(
    const Scheduler_Control *,
    Thread_Control *,
    struct Per_CPU_Control *
  );

#if defined(__RTEMS_HAVE_SYS_CPUSET_H__) && defined(RTEMS_SMP)
  /** @see _Scheduler_Get_affinity() */
  bool ( *get_affinity )(
    const Scheduler_Control *,
    Thread_Control *,
    size_t,
    cpu_set_t *
  );
  
  /** @see _Scheduler_Set_affinity() */
  bool ( *set_affinity )(
    const Scheduler_Control *,
    Thread_Control *,
    size_t,
    const cpu_set_t *
  );
#endif
} Scheduler_Operations;

/**
 * @brief Scheduler context.
 *
 * The scheduler context of a particular scheduler implementation must place
 * this structure at the begin of its context structure.
 */
typedef struct Scheduler_Context {
#if defined(RTEMS_SMP)
  /**
   * @brief Count of processors owned by this scheduler instance.
   */
  uint32_t processor_count;
#endif
} Scheduler_Context;

/**
 * @brief Scheduler control.
 */
struct Scheduler_Control {
  /**
   * @brief Reference to a statically allocated scheduler context.
   */
  Scheduler_Context *context;

  /**
   * @brief The scheduler operations.
   */
  Scheduler_Operations Operations;

  /**
   * @brief The scheduler name.
   */
  uint32_t name;
};

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
struct Scheduler_Node {
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
  Thread_Control *user;

  /**
   * @brief The help state of this node.
   */
  Scheduler_Help_state help_state;

  /**
   * @brief The thread owning this node.
   */
  Thread_Control *owner;

  /**
   * @brief The idle thread claimed by this node in case the help state is
   * SCHEDULER_HELP_ACTIVE_OWNER.
   *
   * Active owners will lend their own node to an idle thread in case they
   * execute currently using another node or in case they perform a blocking
   * operation.  This is necessary to ensure the priority ceiling protocols
   * work across scheduler boundaries.
   */
  Thread_Control *idle;

  /**
   * @brief The thread accepting help by this node in case the help state is
   * not SCHEDULER_HELP_YOURSELF.
   */
  Thread_Control *accepts_help;
#endif
};

/**
 * @brief Registered schedulers.
 *
 * Application provided via <rtems/confdefs.h>.
 *
 * @see _Scheduler_Count.
 */
extern const Scheduler_Control _Scheduler_Table[];

/**
 * @brief Count of registered schedulers.
 *
 * Application provided via <rtems/confdefs.h> on SMP configurations.
 *
 * It is very important that this is a compile-time constant on uni-processor
 * configurations (in this case RTEMS_SMP is not defined) so that the compiler
 * can optimize the some loops away
 *
 * @see _Scheduler_Table.
 */
#if defined(RTEMS_SMP)
  extern const size_t _Scheduler_Count;
#else
  #define _Scheduler_Count ( (size_t) 1 )
#endif

#if defined(RTEMS_SMP)
  /**
   * @brief The scheduler assignment default attributes.
   */
  #define SCHEDULER_ASSIGN_DEFAULT UINT32_C(0x0)

  /**
   * @brief The presence of this processor is optional.
   */
  #define SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL SCHEDULER_ASSIGN_DEFAULT

  /**
   * @brief The presence of this processor is mandatory.
   */
  #define SCHEDULER_ASSIGN_PROCESSOR_MANDATORY UINT32_C(0x1)

  /**
   * @brief Scheduler assignment.
   */
  typedef struct {
    /**
     * @brief The scheduler for this processor.
     */
    const Scheduler_Control *scheduler;

    /**
     * @brief The scheduler assignment attributes.
     *
     * Use @ref SCHEDULER_ASSIGN_DEFAULT to select default attributes.
     *
     * The presence of a processor can be
     * - @ref SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL, or
     * - @ref SCHEDULER_ASSIGN_PROCESSOR_MANDATORY.
     */
    uint32_t attributes;
  } Scheduler_Assignment;

  /**
   * @brief The scheduler assignments.
   *
   * The length of this array must be equal to the maximum processors.
   *
   * Application provided via <rtems/confdefs.h>.
   *
   * @see _Scheduler_Table and rtems_configuration_get_maximum_processors().
   */
  extern const Scheduler_Assignment _Scheduler_Assignments[];
#endif

#if defined(RTEMS_SMP)
  /**
   * @brief Does nothing.
   *
   * @param[in] scheduler Unused.
   * @param[in] offers_help Unused.
   * @param[in] needs_help Unused.
   *
   * @retval NULL Always.
   */
  Thread_Control *_Scheduler_default_Ask_for_help(
    const Scheduler_Control *scheduler,
    Thread_Control          *offers_help,
    Thread_Control          *needs_help
  );

  #define SCHEDULER_OPERATION_DEFAULT_ASK_FOR_HELP \
    _Scheduler_default_Ask_for_help,
#else
  #define SCHEDULER_OPERATION_DEFAULT_ASK_FOR_HELP
#endif

/**
 * @brief Does nothing.
 *
 * @param[in] scheduler Unused.
 * @param[in] the_thread Unused.
 */
void _Scheduler_default_Schedule(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 * @brief Does nothing.
 *
 * @param[in] scheduler Unused.
 * @param[in] the_thread Unused.
 */
void _Scheduler_default_Node_initialize(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 * @brief Does nothing.
 *
 * @param[in] scheduler Unused.
 * @param[in] the_thread Unused.
 */
void _Scheduler_default_Node_destroy(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 * @brief Does nothing.
 *
 * @param[in] scheduler Unused.
 * @param[in] the_thread Unused.
 * @param[in] new_priority Unused.
 */
void _Scheduler_default_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Control         new_priority
);

/**
 * @brief Does nothing.
 *
 * @param[in] scheduler Unused.
 * @param[in] the_thread Unused.
 * @param[in] deadline Unused.
 */
void _Scheduler_default_Release_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  uint32_t                 deadline
);

/**
 * @brief Performs tick operations depending on the CPU budget algorithm for
 * each executing thread.
 *
 * This routine is invoked as part of processing each clock tick.
 *
 * @param[in] scheduler The scheduler.
 * @param[in] executing An executing thread.
 */
void _Scheduler_default_Tick(
  const Scheduler_Control *scheduler,
  Thread_Control          *executing
);

/**
 * @brief Starts an idle thread.
 *
 * @param[in] scheduler The scheduler.
 * @param[in] the_thread An idle thread.
 * @param[in] cpu This parameter is unused.
 */
void _Scheduler_default_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  struct Per_CPU_Control  *cpu
);

#if defined(__RTEMS_HAVE_SYS_CPUSET_H__) && defined(RTEMS_SMP)
  /**
   * @brief Get affinity for the default scheduler.
   *
   * @param[in] scheduler The scheduler instance.
   * @param[in] thread The associated thread.
   * @param[in] cpusetsize The size of the cpuset.
   * @param[out] cpuset Affinity set containing all CPUs.
   *
   * @retval 0 Successfully got cpuset
   * @retval -1 The cpusetsize is invalid for the system
   */
  bool _Scheduler_default_Get_affinity(
    const Scheduler_Control *scheduler,
    Thread_Control          *thread,
    size_t                   cpusetsize,
    cpu_set_t               *cpuset
  );

  /** 
   * @brief Set affinity for the default scheduler.
   *
   * @param[in] scheduler The scheduler instance.
   * @param[in] thread The associated thread.
   * @param[in] cpusetsize The size of the cpuset.
   * @param[in] cpuset Affinity new affinity set.
   *
   * @retval 0 Successful
   *
   *  This method always returns successful and does not save
   *  the cpuset.
   */
  bool _Scheduler_default_Set_affinity(
    const Scheduler_Control *scheduler,
    Thread_Control          *thread,
    size_t                   cpusetsize,
    const cpu_set_t         *cpuset
  );

  #define SCHEDULER_OPERATION_DEFAULT_GET_SET_AFFINITY \
    , _Scheduler_default_Get_affinity \
    , _Scheduler_default_Set_affinity
#else
  #define SCHEDULER_OPERATION_DEFAULT_GET_SET_AFFINITY
#endif

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
