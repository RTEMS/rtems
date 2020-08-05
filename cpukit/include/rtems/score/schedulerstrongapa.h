/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerStrongAPA
 *
 * @brief Strong APA Scheduler API
 */

/*	
 * Copyright (c) 2013, 2018 embedded brains GmbH, 2020 Richi Dubey.  
 *  All rights reserved.	
 *	
 *  embedded brains GmbH	
 *  Dornierstr. 4	
 *  82178 Puchheim	
 *  Germany	
 *  <rtems@embedded-brains.de>	
 *
 *  Richi Dubey: richidubey@gmail.com
 *	
 * The license and distribution terms for this file may be	
 * found in the file LICENSE in this distribution or at	
 * http://www.rtems.org/license/LICENSE.
 */
 
#ifndef _RTEMS_SCORE_SCHEDULERSTRONGAPA_H
#define _RTEMS_SCORE_SCHEDULERSTRONGAPA_H

#include <rtems/score/scheduler.h>
#include <rtems/score/schedulersmp.h>
#include <rtems/score/percpu.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup RTEMSScoreSchedulerStrongAPA Strong APA Scheduler
 *
 * @ingroup RTEMSScoreSchedulerSMP
 *
 * @brief Strong APA Scheduler
 *
 * This is an implementation of the Strong APA scheduler defined by
 * Cerqueira et al. in Linux's Processor Affinity API, Refined: 
 * Shifting Real-Time Tasks Towards Higher Schedulability.
 *
 * @{
 */

 /**
 * @brief Scheduler context for Strong APA scheduler.
 *
 * Has the structure for scheduler context
 * and Node defintion for Strong APA scheduler
 */
typedef struct {
 /**
   * @brief SMP Context to refer to SMP implementation
   * code.  
   */
  Scheduler_SMP_Context Base;
  
  /**
   * @brief Chain of all the nodes present in
   * the system. Accounts for ready and scheduled nodes.
   */
  Chain_Control All_nodes;
 
  /**
   * @brief Queue for this context
   */
  Scheduler_strong_APA_Queue *queue;
   
  /**
   * @brief Pointer to structure with array of
   * boolean visited values
   */ 
  Scheduler_strong_APA_Visited *visited;
   
  /**
   * @brief Pointer to structure with array of 
   * caller corresponding to a CPU
   */ 
  Scheduler_strong_APA_Caller *caller;
} Scheduler_strong_APA_Context;

/**
 * @brief Scheduler node specialization for Strong APA schedulers.
 */
typedef struct {
 /**
   * @brief Chain node for Scheduler_strong_APA_Context::allNodes
   */
  Chain_Node Node;
  
  /**
   * @brief SMP scheduler node.
   */
  Scheduler_SMP_Node Base;

  /**
   * @brief The associated affinity set of this node.
   */
  Processor_mask Affinity;
} Scheduler_strong_APA_Node;

/**
 * @brief CPU structure to be used while traversing in the FIFO Queue
 */
typedef struct
{
  /**
   * @brief Array of Cpu to be used for the queue operations 
   */	
  Per_CPU_Control Cpu[ RTEMS_ZERO_LENGTH_ARRAY ];
} Scheduler_strong_APA_Queue;

/**
 * @brief Caller corresponding to a Cpu in Scheduler_strong_APA_Queue
 */
typedef struct
{
  /**
   * @brief Array of caller each corresponding to the
   * Scheduler_strong_APA_Queue::Cpu at the same index 
   */	
  Scheduler_strong_APA_Node *caller[ RTEMS_ZERO_LENGTH_ARRAY ];
} Scheduler_strong_APA_Caller;

/**
 * @brief  to a Cpu in Scheduler_strong_APA_Queue
 */
typedef struct
{
  /**
   * @brief Array of boolean each corresponding to the visited status of 
   * Scheduler_strong_APA_Queue::Cpu at the same index 
   */	
  bool *visited[ RTEMS_ZERO_LENGTH_ARRAY ];
} Scheduler_strong_APA_Visited;

/**
 * @brief Entry points for the Strong APA Scheduler.
 */
#define SCHEDULER_STRONG_APA_ENTRY_POINTS \
  { \
    _Scheduler_strong_APA_Initialize, \
    _Scheduler_default_Schedule, \
    _Scheduler_strong_APA_Yield, \
    _Scheduler_strong_APA_Block, \
    _Scheduler_strong_APA_Unblock, \
    _Scheduler_strong_APA_Update_priority, \
    _Scheduler_default_Map_priority, \
    _Scheduler_default_Unmap_priority, \
    _Scheduler_strong_APA_Ask_for_help, \
    _Scheduler_strong_APA_Reconsider_help_request, \
    _Scheduler_strong_APA_Withdraw_node, \
    _Scheduler_default_Pin_or_unpin, \
    _Scheduler_default_Pin_or_unpin, \
    _Scheduler_strong_APA_Add_processor, \
    _Scheduler_strong_APA_Remove_processor, \
    _Scheduler_strong_APA_Node_initialize, \
    _Scheduler_default_Node_destroy, \
    _Scheduler_default_Release_job, \
    _Scheduler_default_Cancel_job, \
    _Scheduler_default_Tick, \
    _Scheduler_strong_APA_Start_idle, \
    _Scheduler_strong_APA_Set_affinity \
  }

/**
 * @brief Initializes the Strong_APA scheduler.
 *
 * Sets the chain containing all the nodes to empty 
 * and initializes the SMP scheduler.
 *
 * @param scheduler used to get reference to Strong APA scheduler context 
 * @retval void
 * @see _Scheduler_strong_APA_Node_initialize()
 */  
void _Scheduler_strong_APA_Initialize( 
  const Scheduler_Control *scheduler 
);
   
/**
 * @brief Called when a node yields the processor
 *
 * @param scheduler The scheduler control instance.
 * @param thread Thread corresponding to @node
 * @param node Node that yield the processor
 */
void _Scheduler_strong_APA_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

/**
 * @brief Blocks a node
 *
 * Changes the state of the node and extracts it from the queue
 * calls _Scheduler_SMP_Block().
 *
 * @param context The scheduler control instance.
 * @param thread Thread correspoding to the @node.
 * @param node node which is to be blocked
 */ 
void _Scheduler_strong_APA_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

/**
 * @brief Unblocks a node
 *
 * Changes the state of the node and calls _Scheduler_SMP_Unblock().
 *
 * @param scheduler The scheduler control instance.
 * @param thread Thread correspoding to the @node.
 * @param node node which is to be unblocked
 * @see _Scheduler_strong_APA_Enqueue() 
 * @see _Scheduler_strong_APA_Do_update()
 */ 
void _Scheduler_strong_APA_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

/**
 * @brief Updates the priority of the node
 *
 * @param scheduler The scheduler control instance.
 * @param thread Thread correspoding to the @node.
 * @param node Node whose priority has to be updated 
 */ 
void _Scheduler_strong_APA_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
);

/**
 * @brief Calls the SMP Ask_for_help
 *
 * @param scheduler The scheduler control instance.
 * @param thread Thread correspoding to the @node that asks for help.
 * @param node node associated with @thread
 */ 
bool _Scheduler_strong_APA_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief To Reconsider the help request
 *
 * @param scheduler The scheduler control instance.
 * @param thread Thread correspoding to the @node.
 * @param node Node corresponding to @thread which asks for 
 * reconsideration
 */ 
void _Scheduler_strong_APA_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
);

/**
 * @brief Withdraws a node 
 *
 * @param scheduler The scheduler control instance.
 * @param thread Thread correspoding to the @node.
 * @param node Node that has to be withdrawn
 * @param next_state the state that the node goes to
 */ 
void _Scheduler_strong_APA_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
);

/**
 * @brief Adds a processor to the scheduler instance
 *
 * and allocates an idle thread to the processor.
 *
 * @param scheduler The scheduler control instance.
 * @param idle Idle thread to be allocated to the processor
 */ 
void _Scheduler_strong_APA_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
);

/**
 * @brief Removes a processor from the scheduler instance
 *
 * @param scheduler The scheduler control instance.
 * @param cpu processor that is removed
 */ 
Thread_Control *_Scheduler_strong_APA_Remove_processor(
  const Scheduler_Control *scheduler,
  Per_CPU_Control         *cpu
);

/**
 * @brief Initializes the node with the given priority.
 *
 * @param scheduler The scheduler control instance.
 * @param[out] node The node to initialize.
 * @param the_thread The thread of the node to initialize.
 * @param priority The priority for @a node.
 */
void _Scheduler_strong_APA_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
);

/**
 * @brief Starts an idle thread on a CPU
 *
 * @param scheduler The scheduler control instance.
 * @param idle Idle Thread
 * @param cpu processor that gets the idle thread
 */ 
void _Scheduler_strong_APA_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle,
  Per_CPU_Control         *cpu
);

/**
 * @brief Sets the affinity of the @node_base to @affinity
 */
bool _Scheduler_strong_APA_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  const Processor_mask    *affinity
);
/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_SCORE_SCHEDULERSTRONGAPA_H */
