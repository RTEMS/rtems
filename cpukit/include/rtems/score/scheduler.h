/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreScheduler which are used by the implementation and the
 *   @ref RTEMSImplApplConfig.
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

#include <rtems/score/thread.h>
#include <rtems/score/status.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Per_CPU_Control;

/**
 * @addtogroup RTEMSScoreScheduler
 *
 * @{
 */

typedef struct _Scheduler_Control Scheduler_Control;

/**
 * @brief The scheduler operations.
 */
typedef struct {
  /** @see _Scheduler_Handler_initialization() */
  void ( *initialize )( const Scheduler_Control * );

  /** @see _Scheduler_Schedule() */
  void ( *schedule )( const Scheduler_Control *, Thread_Control *);

  /** @see _Scheduler_Yield() */
  void ( *yield )(
    const Scheduler_Control *,
    Thread_Control *,
    Scheduler_Node *
  );

  /** @see _Scheduler_Block() */
  void ( *block )(
    const Scheduler_Control *,
    Thread_Control *,
    Scheduler_Node *
  );

  /** @see _Scheduler_Unblock() */
  void ( *unblock )(
    const Scheduler_Control *,
    Thread_Control *,
    Scheduler_Node *
  );

  /** @see _Scheduler_Update_priority() */
  void ( *update_priority )(
    const Scheduler_Control *,
    Thread_Control *,
    Scheduler_Node *
  );

  /** @see _Scheduler_Map_priority() */
  Priority_Control ( *map_priority )(
    const Scheduler_Control *,
    Priority_Control
  );

  /** @see _Scheduler_Unmap_priority() */
  Priority_Control ( *unmap_priority )(
    const Scheduler_Control *,
    Priority_Control
  );

#if defined(RTEMS_SMP)
  /**
   * @brief Ask for help operation.
   *
   * @param[in] scheduler The scheduler instance to ask for help.
   * @param[in] the_thread The thread needing help.
   * @param[in] node The scheduler node.
   *
   * @retval true Ask for help was successful.
   * @retval false Otherwise.
   */
  bool ( *ask_for_help )(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node
  );

  /**
   * @brief Reconsider help operation.
   *
   * @param[in] scheduler The scheduler instance to reconsider the help
   *   request.
   * @param[in] the_thread The thread reconsidering a help request.
   * @param[in] node The scheduler node.
   */
  void ( *reconsider_help_request )(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node
  );

  /**
   * @brief Withdraw node operation.
   *
   * @param[in] scheduler The scheduler instance to withdraw the node.
   * @param[in] the_thread The thread using the node.
   * @param[in] node The scheduler node to withdraw.
   * @param[in] next_state The next thread scheduler state in case the node is
   *   scheduled.
   */
  void ( *withdraw_node )(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node,
    Thread_Scheduler_state   next_state
  );

  /**
   * @brief Pin thread operation.
   *
   * @param[in] scheduler The scheduler instance of the specified processor.
   * @param[in] the_thread The thread to pin.
   * @param[in] node The scheduler node of the thread.
   * @param[in] cpu The processor to pin the thread.
   */
  void ( *pin )(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node,
    struct Per_CPU_Control  *cpu
  );

  /**
   * @brief Unpin thread operation.
   *
   * @param[in] scheduler The scheduler instance of the specified processor.
   * @param[in] the_thread The thread to unpin.
   * @param[in] node The scheduler node of the thread.
   * @param[in] cpu The processor to unpin the thread.
   */
  void ( *unpin )(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node,
    struct Per_CPU_Control  *cpu
  );

  /**
   * @brief Add processor operation.
   *
   * @param[in] scheduler The scheduler instance to add the processor.
   * @param[in] idle The idle thread of the processor to add.
   */
  void ( *add_processor )(
    const Scheduler_Control *scheduler,
    Thread_Control          *idle
  );

  /**
   * @brief Remove processor operation.
   *
   * @param[in] scheduler The scheduler instance to remove the processor.
   * @param[in] cpu The processor to remove.
   *
   * @return The idle thread of the removed processor.
   */
  Thread_Control *( *remove_processor )(
    const Scheduler_Control *scheduler,
    struct Per_CPU_Control  *cpu
  );
#endif

  /** @see _Scheduler_Node_initialize() */
  void ( *node_initialize )(
    const Scheduler_Control *,
    Scheduler_Node *,
    Thread_Control *,
    Priority_Control
  );

  /** @see _Scheduler_Node_destroy() */
  void ( *node_destroy )( const Scheduler_Control *, Scheduler_Node * );

  /** @see _Scheduler_Release_job() */
  void ( *release_job ) (
    const Scheduler_Control *,
    Thread_Control *,
    Priority_Node *,
    uint64_t,
    Thread_queue_Context *
  );

  /** @see _Scheduler_Cancel_job() */
  void ( *cancel_job ) (
    const Scheduler_Control *,
    Thread_Control *,
    Priority_Node *,
    Thread_queue_Context *
  );

  /** @see _Scheduler_Tick() */
  void ( *tick )( const Scheduler_Control *, Thread_Control * );

  /** @see _Scheduler_Start_idle() */
  void ( *start_idle )(
    const Scheduler_Control *,
    Thread_Control *,
    struct Per_CPU_Control *
  );

#if defined(RTEMS_SMP)
  /** @see _Scheduler_Set_affinity() */
  Status_Control ( *set_affinity )(
    const Scheduler_Control *,
    Thread_Control *,
    Scheduler_Node *,
    const Processor_mask *
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
  /**
   * @brief Lock to protect this scheduler instance.
   */
  ISR_LOCK_MEMBER( Lock )

#if defined(RTEMS_SMP)
  /**
   * @brief The set of processors owned by this scheduler instance.
   */
  Processor_mask Processors;
#endif
} Scheduler_Context;

/**
 * @brief Scheduler control.
 */
struct _Scheduler_Control {
  /**
   * @brief Reference to a statically allocated scheduler context.
   */
  Scheduler_Context *context;

  /**
   * @brief The scheduler operations.
   */
  Scheduler_Operations Operations;

  /**
   * @brief The maximum priority value of this scheduler.
   *
   * It defines the lowest (least important) thread priority for this
   * scheduler.  For example the idle threads have this priority.
   */
  Priority_Control maximum_priority;

  /**
   * @brief The scheduler name.
   */
  uint32_t name;

#if defined(RTEMS_SMP)
  /**
   * @brief True if the non-preempt mode for threads is supported by the
   * scheduler, otherwise false.
   */
  bool is_non_preempt_mode_supported;
#endif
};

/**
 * @brief This table contains the configured schedulers.
 *
 * The table is defined by <rtems/confdefs.h> through the
 * #CONFIGURE_SCHEDULER_TABLE_ENTRIES application configuration option.
 *
 * @see _Scheduler_Count.
 */
extern const Scheduler_Control _Scheduler_Table[];

/**
 * @brief This constant contains the count of configured schedulers.
 *
 * In SMP configurations, the constant is defined by <rtems/confdefs.h> through
 * the count of entries of the #CONFIGURE_SCHEDULER_TABLE_ENTRIES application
 * configuration option.
 *
 * In uniprocessor configurations, this is a compile time constant set to one.
 * This is important so that the compiler can optimize some loops away.
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
  extern const Scheduler_Assignment _Scheduler_Initial_assignments[];
#endif

/**
 * @brief Returns the scheduler internal thread priority mapped by
 * SCHEDULER_PRIORITY_MAP().
 *
 * @param scheduler Unused.
 * @param priority The user visible thread priority.
 *
 * @return The scheduler internal thread priority.
 */
Priority_Control _Scheduler_default_Map_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
);

/**
 * @brief Returns the user visible thread priority unmapped by
 * SCHEDULER_PRIORITY_UNMAP().
 *
 * @param scheduler Unused.
 * @param priority The scheduler internal thread priority.
 *
 * @return priority The user visible thread priority.
 */
Priority_Control _Scheduler_default_Unmap_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
);

#if defined(RTEMS_SMP)
  /**
   * @brief Does nothing.
   *
   * @param scheduler This parameter is unused.
   * @param the_thread This parameter is unused.
   * @param node This parameter is unused.
   *
   * @return Always returns false.
   */
  bool _Scheduler_default_Ask_for_help(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node
  );

  /**
   * @brief Does nothing.
   *
   * @param scheduler This parameter is unused.
   * @param the_thread This parameter is unused.
   * @param node This parameter is unused.
   */
  void _Scheduler_default_Reconsider_help_request(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node
  );

  /**
   * @brief Does nothing.
   *
   * @param scheduler This parameter is unused.
   * @param the_thread This parameter is unused..
   * @param node This parameter is unused.
   * @param next_state This parameter is unused.
   */
  void _Scheduler_default_Withdraw_node(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node,
    Thread_Scheduler_state   next_state
  );

  /**
   * @brief Does nothing in a single processor system, otherwise a fatal error
   * is issued.
   *
   * @param scheduler This parameter is unused.
   * @param the_thread This parameter is unused.
   * @param node This parameter is unused.
   * @param cpu This parameter is unused.
   */
  void _Scheduler_default_Pin_or_unpin(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node,
    struct Per_CPU_Control  *cpu
  );

  #define SCHEDULER_OPERATION_DEFAULT_ASK_FOR_HELP \
    _Scheduler_default_Ask_for_help, \
    _Scheduler_default_Reconsider_help_request, \
    _Scheduler_default_Withdraw_node, \
    _Scheduler_default_Pin_or_unpin, \
    _Scheduler_default_Pin_or_unpin, \
    NULL, \
    NULL,
#else
  #define SCHEDULER_OPERATION_DEFAULT_ASK_FOR_HELP
#endif

/**
 * @brief Does nothing.
 *
 * @param scheduler This parameter is unused.
 * @param the_thread This parameter is unused.
 */
void _Scheduler_default_Schedule(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
);

/**
 * @brief Performs the scheduler base node initialization.
 *
 * @param scheduler This parameter is unused.
 * @param[out] node The node to initialize.
 * @param the_thread This parameter is unused.
 * @param priority The thread priority.
 */
void _Scheduler_default_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
);

/**
 * @brief Does nothing.
 *
 * @param scheduler This parameter is unused.
 * @param node This parameter is unused.
 */
void _Scheduler_default_Node_destroy(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node
);

/**
 * @brief Does nothing.
 *
 * @param scheduler This parameter is unused.
 * @param the_thread This parameter is unused.
 * @param priority_node This parameter is unused.
 * @param deadline This parameter is unused.
 * @param queue_context This parameter is unused.
 *
 * @return Always returns NULL.
 */
void _Scheduler_default_Release_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Node           *priority_node,
  uint64_t                 deadline,
  Thread_queue_Context    *queue_context
);

/**
 * @brief Does nothing.
 *
 * @param scheduler This parameter is unused.
 * @param the_thread This parameter is unused.
 * @param priority_node This parameter is unused.
 * @param queue_context This parameter is unused.
 *
 * @return Always returns NULL.
 */
void _Scheduler_default_Cancel_job(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Priority_Node           *priority_node,
  Thread_queue_Context    *queue_context
);

/**
 * @brief Performs tick operations depending on the CPU budget algorithm for
 * each executing thread.
 *
 * This routine is invoked as part of processing each clock tick.
 *
 * @param scheduler The scheduler.
 * @param[in, out] executing An executing thread.
 */
void _Scheduler_default_Tick(
  const Scheduler_Control *scheduler,
  Thread_Control          *executing
);

/**
 * @brief Starts an idle thread.
 *
 * @param scheduler This parameter is unused.
 * @param[in, out] the_thread An idle thread.
 * @param cpu This parameter is unused.
 */
void _Scheduler_default_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  struct Per_CPU_Control  *cpu
);

#if defined(RTEMS_SMP)
  /** 
   * @brief Checks if the processor set of the scheduler is the subset of the affinity set.
   *
   * Default implementation of the set affinity scheduler operation.
   *
   * @param scheduler This parameter is unused.
   * @param thread This parameter is unused.
   * @param node This parameter is unused.
   * @param affinity The new processor affinity set for the thread.
   *
    * @retval STATUS_SUCCESSFUL The affinity is a subset of the online
    *   processors.
    *
    * @retval STATUS_INVALID_NUMBER The affinity is not a subset of the online
    *   processors.
   */
  Status_Control _Scheduler_default_Set_affinity(
    const Scheduler_Control *scheduler,
    Thread_Control          *thread,
    Scheduler_Node          *node,
    const Processor_mask    *affinity
  );

  #define SCHEDULER_OPERATION_DEFAULT_GET_SET_AFFINITY \
    , _Scheduler_default_Set_affinity
#else
  #define SCHEDULER_OPERATION_DEFAULT_GET_SET_AFFINITY
#endif

/**
 * @brief This defines the lowest (least important) thread priority of the
 * first scheduler instance.
 */
#define PRIORITY_MAXIMUM ( _Scheduler_Table[ 0 ].maximum_priority )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
