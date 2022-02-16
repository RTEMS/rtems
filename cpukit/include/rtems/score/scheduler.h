/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
   * @brief Makes the node sticky.
   *
   * This operation is used by _Thread_Priority_update_and_make_sticky().  It
   * is only called for the scheduler node of the home scheduler.
   *
   * Uniprocessor schedulers schould provide
   * _Scheduler_default_Sticky_do_nothing() for this operation.
   *
   * SMP schedulers should provide this operation using
   * _Scheduler_SMP_Make_sticky().
   *
   * The make and clean sticky operations are an optimization to simplify the
   * control flow in the update priority operation.  The update priority
   * operation is used for all scheduler nodes and not just the scheduler node
   * of home schedulers.  The update priority operation is a commonly used
   * operations together with block and unblock.  The make and clean sticky
   * operations are used only in specific scenarios.
   *
   * @param scheduler is the scheduler of the node.
   *
   * @param[in, out] the_thread is the thread owning the node.
   *
   * @param[in, out] node is the scheduler node to make sticky.
   */
  void ( *make_sticky )(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node
  );

  /**
   * @brief Cleans the sticky property from the node.
   *
   * This operation is used by _Thread_Priority_update_and_clean_sticky().  It
   * is only called for the scheduler node of the home scheduler.
   *
   * Uniprocessor schedulers schould provide
   * _Scheduler_default_Sticky_do_nothing() for this operation.
   *
   * SMP schedulers should provide this operation using
   * _Scheduler_SMP_Clean_sticky().
   *
   * @param scheduler is the scheduler of the node.
   *
   * @param[in, out] the_thread is the thread owning the node.
   *
   * @param[in, out] node is the scheduler node to clean the sticky property.
   */
  void ( *clean_sticky )(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node
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
#if defined(RTEMS_SMP)
  /**
   * @brief Lock to protect this scheduler instance.
   */
  ISR_lock_Control Lock;

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
   * This default implementation for the make and clean sticky operations
   * should be used by uniprocessor schedulers if SMP support is enabled.
   *
   * @param scheduler is an unused parameter.
   *
   * @param the_thread is an unused parameter.
   *
   * @param node is an unused parameter.
   */
  void _Scheduler_default_Sticky_do_nothing(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node
  );

  /**
   * @brief Does nothing.
   *
   * This default implementation for the thread pin or unpin operations should
   * be used by uniprocessor schedulers if SMP support is enabled.
   *
   * @param scheduler This parameter is unused.
   * @param the_thread This parameter is unused.
   * @param node This parameter is unused.
   * @param cpu This parameter is unused.
   */
  void _Scheduler_default_Pin_or_unpin_do_nothing(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node,
    struct Per_CPU_Control  *cpu
  );

  /**
   * @brief Does nothing in a single processor system, otherwise a fatal error
   * is issued.
   *
   * This default implementation for the thread pin or unpin operations should
   * be used by SMP schedulers which do not support thread pinning.
   *
   * @param scheduler This parameter is unused.
   * @param the_thread This parameter is unused.
   * @param node This parameter is unused.
   * @param cpu This parameter is unused.
   */
  void _Scheduler_default_Pin_or_unpin_not_supported(
    const Scheduler_Control *scheduler,
    Thread_Control          *the_thread,
    Scheduler_Node          *node,
    struct Per_CPU_Control  *cpu
  );
#endif

/**
 * @brief This define provides a set of default implementations for
 *   SMP-specific scheduler operations.
 *
 * The default implementations are intended for uniprocessor schedulers.  SMP
 * schedulers shall implement the operations properly.
 *
 * If SMP support is disabled, the define evaluates to nothing.
 *
 * If SMP support is enabled and the system has exactly one processor, then it
 * may use an uniprocessor scheduler.  The ask for help, reconsider help
 * request, and withdraw node operations are NULL, since they are only used if
 * a thread has at least one helping scheduler node.  At least two schedulers
 * are required to get a helping node and each scheduler involved must own at
 * least one processor.  This is not possible on a system with exactly one
 * processor.  The processor add operation is NULL, since there is no other
 * processor to add.  The processor remove operation is NULL, since the one and
 * only processor cannot be removed.
 */
#if defined(RTEMS_SMP)
  #define SCHEDULER_DEFAULT_SMP_OPERATIONS \
    NULL, \
    NULL, \
    NULL, \
    _Scheduler_default_Sticky_do_nothing, \
    _Scheduler_default_Sticky_do_nothing, \
    _Scheduler_default_Pin_or_unpin_do_nothing, \
    _Scheduler_default_Pin_or_unpin_do_nothing, \
    NULL, \
    NULL,
#else
  #define SCHEDULER_DEFAULT_SMP_OPERATIONS
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
#endif

/**
 * @brief This define provides the default implementation for the
 *   SMP-specific set affinity operation.
 *
 * The default implementation _Scheduler_default_Set_affinity() is intended for
 * uniprocessor schedulers and SMP schedulers which only support an affinity to
 * all online processors.
 *
 * If SMP support is disabled, the define evaluates to nothing.
 */
#if defined(RTEMS_SMP)
  #define SCHEDULER_DEFAULT_SET_AFFINITY_OPERATION \
    , _Scheduler_default_Set_affinity
#else
  #define SCHEDULER_DEFAULT_SET_AFFINITY_OPERATION
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
