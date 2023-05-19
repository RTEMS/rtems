/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerStrongAPA
 *
 * @brief This source file contains the implementation of
 *   _Scheduler_strong_APA_Add_processor(),
 *   _Scheduler_strong_APA_Allocate_processor(),
 *   _Scheduler_strong_APA_Ask_for_help(), _Scheduler_strong_APA_Block(),
 *   _Scheduler_strong_APA_Do_ask_for_help(),
 *   _Scheduler_strong_APA_Do_enqueue(),
 *   _Scheduler_strong_APA_Do_set_affinity(),
 *   _Scheduler_strong_APA_Do_update(), _Scheduler_strong_APA_Enqueue(),
 *   _Scheduler_strong_APA_Enqueue_scheduled(),
 *   _Scheduler_strong_APA_Extract_from_ready(),
 *   _Scheduler_strong_APA_Extract_from_scheduled(),
 *   _Scheduler_strong_APA_Find_highest_ready(),
 *   _Scheduler_strong_APA_Get_highest_ready(),
 *   _Scheduler_strong_APA_Get_lowest_reachable(),
 *   _Scheduler_strong_APA_Get_lowest_scheduled(),
 *   _Scheduler_strong_APA_Has_ready(),
 *   _Scheduler_strong_APA_Initialize(), _Scheduler_strong_APA_Insert_ready(),
 *   _Scheduler_strong_APA_Move_from_ready_to_scheduled(),
 *   _Scheduler_strong_APA_Move_from_scheduled_to_ready(),
 *   _Scheduler_strong_APA_Node_initialize(),
 *   _Scheduler_strong_APA_Reconsider_help_request(),
 *   _Scheduler_strong_APA_Register_idle(),
 *   _Scheduler_strong_APA_Remove_processor(),
 *   _Scheduler_strong_APA_Set_affinity(),
 *   _Scheduler_strong_APA_Set_scheduled(), _Scheduler_strong_APA_Start_idle(),
 *   _Scheduler_strong_APA_Unblock(), _Scheduler_strong_APA_Update_priority(),
 *   _Scheduler_strong_APA_Withdraw_node(),
 *   _Scheduler_strong_APA_Make_sticky(), _Scheduler_strong_APA_Clean_sticky(),
 *   and _Scheduler_strong_APA_Yield().
 */

/*
 * Copyright (C) 2020 Richi Dubey
 * Copyright (C) 2013, 2016 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerstrongapa.h>
#include <rtems/score/schedulersmpimpl.h>
#include <rtems/score/assert.h>

#define STRONG_SCHEDULER_NODE_OF_CHAIN( node ) \
  RTEMS_CONTAINER_OF( node, Scheduler_strong_APA_Node, Ready_node )

static inline Scheduler_strong_APA_Context *
_Scheduler_strong_APA_Get_context( const Scheduler_Control *scheduler )
{
  return (Scheduler_strong_APA_Context *) _Scheduler_Get_context( scheduler );
}

static inline Scheduler_strong_APA_Context *
_Scheduler_strong_APA_Get_self( Scheduler_Context *context )
{
  return (Scheduler_strong_APA_Context *) context;
}

static inline Scheduler_strong_APA_Node *
_Scheduler_strong_APA_Node_downcast( Scheduler_Node *node )
{
  return (Scheduler_strong_APA_Node *) node;
}

static inline void _Scheduler_strong_APA_Do_update(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   new_priority
)
{
  Scheduler_SMP_Node *smp_node;
  (void) context;

  smp_node = _Scheduler_SMP_Node_downcast( node );
  _Scheduler_SMP_Node_update_priority( smp_node, new_priority );
}

/*
 * Returns true if the Strong APA scheduler has ready nodes
 * available for scheduling.
 */
static inline bool _Scheduler_strong_APA_Has_ready(
  Scheduler_Context *context
)
{
  Scheduler_strong_APA_Context *self;
  const Chain_Node             *tail;
  Chain_Node                   *next;
  Scheduler_strong_APA_Node    *node;

  self = _Scheduler_strong_APA_Get_self( context );
  tail = _Chain_Immutable_tail( &self->Ready );
  next = _Chain_First( &self->Ready );

  while ( next != tail ) {
    node = (Scheduler_strong_APA_Node *)STRONG_SCHEDULER_NODE_OF_CHAIN( next );

    if (
      _Scheduler_SMP_Node_state( &node->Base.Base ) ==
      SCHEDULER_SMP_NODE_READY
    ) {
      return true;
    }

    next = _Chain_Next( next );
  }

  return false;
}

static inline void _Scheduler_strong_APA_Set_scheduled(
  Scheduler_strong_APA_Context *self,
  Scheduler_Node                *executing,
  const Per_CPU_Control         *cpu
)
{
  self->CPU[ _Per_CPU_Get_index( cpu ) ].executing = executing;
}

static inline Scheduler_Node *_Scheduler_strong_APA_Get_scheduled(
  const Scheduler_strong_APA_Context *self,
  const Per_CPU_Control               *cpu
)
{
  return self->CPU[ _Per_CPU_Get_index( cpu ) ].executing;
}

static inline void _Scheduler_strong_APA_Allocate_processor(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_base,
  Per_CPU_Control   *cpu
)
{
  Scheduler_strong_APA_Node    *scheduled;
  Scheduler_strong_APA_Context *self;

  scheduled = _Scheduler_strong_APA_Node_downcast( scheduled_base );
  self = _Scheduler_strong_APA_Get_self( context );

  _Scheduler_strong_APA_Set_scheduled( self, scheduled_base, cpu );

  _Scheduler_SMP_Allocate_processor_exact(
    context,
    &( scheduled->Base.Base ),
    cpu
  );
}

/*
 * Finds and returns the highest ready node present by accessing the
 * _Strong_APA_Context->CPU with front and rear values.
 */
static inline Scheduler_Node * _Scheduler_strong_APA_Find_highest_ready(
  Scheduler_strong_APA_Context *self,
  uint32_t                      front,
  uint32_t                      rear
)
{
  Scheduler_Node              *highest_ready = NULL;
  Scheduler_strong_APA_CPU    *CPU;
  const Chain_Node            *tail;
  Chain_Node                  *next;
  Scheduler_strong_APA_Node   *node;
  Priority_Control             min_priority_num;
  Priority_Control             curr_priority;
  Per_CPU_Control             *assigned_cpu;
  Scheduler_SMP_Node_state     curr_state;
  Per_CPU_Control             *curr_CPU;

  CPU = self->CPU;
  /*
   * When the first task accessed has nothing to compare its priority against.
   * So, it is the task with the highest priority witnessed so far.
   */
  min_priority_num = UINT64_MAX;

  while ( front <= rear ) {
    curr_CPU = CPU[ front++ ].cpu;

    tail = _Chain_Immutable_tail( &self->Ready );
    next = _Chain_First( &self->Ready );

    while ( next != tail ) {
      node = (Scheduler_strong_APA_Node*) STRONG_SCHEDULER_NODE_OF_CHAIN( next );
      /*
       * Check if the curr_CPU is in the affinity set of the node.
       */
      if (
        _Processor_mask_Is_set( &node->Affinity, _Per_CPU_Get_index( curr_CPU ) )
      ) {
        curr_state = _Scheduler_SMP_Node_state( &node->Base.Base );

        if ( curr_state == SCHEDULER_SMP_NODE_SCHEDULED ) {
          assigned_cpu = _Thread_Get_CPU( node->Base.Base.user );

          if ( CPU[ _Per_CPU_Get_index( assigned_cpu ) ].visited == false ) {
            CPU[ ++rear ].cpu = assigned_cpu;
            CPU[ _Per_CPU_Get_index( assigned_cpu ) ].visited = true;
            /*
             * The curr CPU of the queue invoked this node to add its CPU
             * that it is executing on to the queue. So this node might get
             * preempted because of the invoker curr_CPU and this curr_CPU
             * is the CPU that node should preempt in case this node
             * gets preempted.
             */
            node->cpu_to_preempt = curr_CPU;
          }
        } else if ( curr_state == SCHEDULER_SMP_NODE_READY ) {
          curr_priority = _Scheduler_Node_get_priority( &node->Base.Base );
          curr_priority = SCHEDULER_PRIORITY_PURIFY( curr_priority );

          if (
            min_priority_num == UINT64_MAX ||
            curr_priority < min_priority_num
          ) {
            min_priority_num = curr_priority;
            highest_ready = &node->Base.Base;
            /*
             * In case curr_CPU is filter_CPU, we need to store the
             * cpu_to_preempt value so that we go back to SMP_*
             * function, rather than preempting the node ourselves.
             */
            node->cpu_to_preempt = curr_CPU;
          }
        }
      }
    next = _Chain_Next( next );
    }
  }

  /*
   * By definition, the system would always have a ready node,
   * hence highest_ready would not be NULL.
   */
  _Assert( highest_ready != NULL );

  return highest_ready;
}

static inline Scheduler_Node *_Scheduler_strong_APA_Get_idle( void *arg )
{
  Scheduler_strong_APA_Context *self;
  Scheduler_strong_APA_Node    *lowest_ready = NULL;
  Priority_Control              max_priority_num;
  const Chain_Node             *tail;
  Chain_Node                   *next;

  self = _Scheduler_strong_APA_Get_self( arg );
  tail = _Chain_Immutable_tail( &self->Ready );
  next = _Chain_First( &self->Ready );
  max_priority_num = 0;

  while ( next != tail ) {
    Scheduler_strong_APA_Node *node;
    Scheduler_SMP_Node_state   curr_state;

    node = (Scheduler_strong_APA_Node*) STRONG_SCHEDULER_NODE_OF_CHAIN( next );
    curr_state = _Scheduler_SMP_Node_state( &node->Base.Base );

    if ( curr_state == SCHEDULER_SMP_NODE_READY ) {
      Priority_Control curr_priority;

      curr_priority = _Scheduler_Node_get_priority( &node->Base.Base );

      if ( curr_priority > max_priority_num ) {
        max_priority_num = curr_priority;
        lowest_ready = node;
      }
    }

    next = _Chain_Next( next );
  }

  _Assert( lowest_ready != NULL );
  _Chain_Extract_unprotected( &lowest_ready->Ready_node );
  _Chain_Set_off_chain( &lowest_ready->Ready_node );

  return &lowest_ready->Base.Base;
}

static inline void _Scheduler_strong_APA_Release_idle(
  Scheduler_Node *node_base,
  void           *arg
)
{
  Scheduler_strong_APA_Context *self;
  Scheduler_strong_APA_Node    *node;

  self = _Scheduler_strong_APA_Get_self( arg );
  node = _Scheduler_strong_APA_Node_downcast( node_base );

  if ( _Chain_Is_node_off_chain( &node->Ready_node ) ) {
    _Chain_Append_unprotected( &self->Ready, &node->Ready_node );
  }
}

static inline void  _Scheduler_strong_APA_Move_from_ready_to_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *ready_to_scheduled
)
{
  Priority_Control insert_priority;

  insert_priority = _Scheduler_SMP_Node_priority( ready_to_scheduled );
  insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );
  _Scheduler_SMP_Insert_scheduled(
    context,
    ready_to_scheduled,
    insert_priority
  );
}

static inline void _Scheduler_strong_APA_Insert_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_base,
  Priority_Control   insert_priority
)
{
  Scheduler_strong_APA_Context *self;
  Scheduler_strong_APA_Node    *node;

  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( node_base );

  if( _Chain_Is_node_off_chain( &node->Ready_node ) ) {
    _Chain_Append_unprotected( &self->Ready, &node->Ready_node );
  }  else {
    _Chain_Extract_unprotected( &node->Ready_node );
    _Chain_Set_off_chain( &node->Ready_node );
    _Chain_Append_unprotected( &self->Ready, &node->Ready_node );
  }
}

static inline void _Scheduler_strong_APA_Move_from_scheduled_to_ready(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_to_ready
)
{
  Priority_Control insert_priority;

  if( !_Chain_Is_node_off_chain(  &scheduled_to_ready->Node.Chain ) ) {
    _Scheduler_SMP_Extract_from_scheduled( context, scheduled_to_ready );
  }

  insert_priority = _Scheduler_SMP_Node_priority( scheduled_to_ready );

  _Scheduler_strong_APA_Insert_ready(
    context,
    scheduled_to_ready,
    insert_priority
  );
}

/*
 * Implement the BFS Algorithm for task departure to get the highest ready task
 * for a particular CPU, returns the highest ready Scheduler_Node
 * Scheduler_Node filter here points to the victim node that is blocked
 * resulting which this function is called.
 */
static inline Scheduler_Node *_Scheduler_strong_APA_Get_highest_ready(
  Scheduler_Context *context,
  Scheduler_Node    *filter
)
{
  Scheduler_strong_APA_Context *self;
  Per_CPU_Control              *filter_cpu;
  Scheduler_strong_APA_Node    *node;
  Scheduler_Node               *highest_ready;
  Scheduler_Node               *curr_node;
  Scheduler_Node               *next_node;
  Scheduler_strong_APA_CPU     *CPU;
  uint32_t                      front;
  uint32_t                      rear;
  uint32_t                      cpu_max;
  uint32_t                      cpu_index;

  self = _Scheduler_strong_APA_Get_self( context );
  /*
   * Denotes front and rear of the queue
   */
  front = 0;
  rear = -1;

  filter_cpu = _Thread_Get_CPU( filter->user );
  CPU = self->CPU;
  cpu_max = _SMP_Get_processor_maximum();

  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
    CPU[ cpu_index ].visited = false;
  }

  CPU[ ++rear ].cpu = filter_cpu;
  CPU[ _Per_CPU_Get_index( filter_cpu ) ].visited = true;

  highest_ready = _Scheduler_strong_APA_Find_highest_ready(
                    self,
                    front,
                    rear
                  );

  if ( highest_ready != filter ) {
    /*
     * Backtrack on the path from
     * filter_cpu to highest_ready, shifting along every task.
     */

    node = _Scheduler_strong_APA_Node_downcast( highest_ready );
    /*
     * Highest ready is not just directly reachable from the victim cpu
     * So there is need for task shifting.
     */
    while ( node->cpu_to_preempt != filter_cpu ) {
      Thread_Control *next_node_idle;

      curr_node = &node->Base.Base;
      next_node = _Scheduler_strong_APA_Get_scheduled(
                    self,
                    node->cpu_to_preempt
                  );
      next_node_idle = _Scheduler_Release_idle_thread_if_necessary(
        next_node,
        _Scheduler_strong_APA_Release_idle,
        context
      );

      (void) _Scheduler_SMP_Preempt(
               context,
               curr_node,
               next_node,
               next_node_idle,
               _Scheduler_strong_APA_Allocate_processor
             );

      if ( curr_node == highest_ready ) {
        _Scheduler_strong_APA_Move_from_ready_to_scheduled( context, curr_node );
      }

      node = _Scheduler_strong_APA_Node_downcast( next_node );
    }
    /*
     * To save the last node so that the caller SMP_* function
     * can do the allocation
     */
      curr_node = &node->Base.Base;
      highest_ready = curr_node;
      _Scheduler_strong_APA_Move_from_scheduled_to_ready( context, curr_node );
    }

  return highest_ready;
}

/*
 * Checks the lowest scheduled directly reachable task
 */
static inline Scheduler_Node *_Scheduler_strong_APA_Get_lowest_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *filter_base
)
{
  uint32_t	                cpu_max;
  uint32_t	                cpu_index;
  Scheduler_Node               *curr_node;
  Scheduler_Node               *lowest_scheduled = NULL;
  Priority_Control              max_priority_num;
  Priority_Control              curr_priority;
  Scheduler_strong_APA_Node    *filter_strong_node;
  Scheduler_strong_APA_Context *self;

  self = _Scheduler_strong_APA_Get_self( context );
  max_priority_num = 0;    /* Max (Lowest) priority encountered so far */
  filter_strong_node = _Scheduler_strong_APA_Node_downcast( filter_base );

  /* lowest_scheduled is NULL if affinity of a node is 0 */
  _Assert( !_Processor_mask_Is_zero( &filter_strong_node->Affinity ) );
  cpu_max = _SMP_Get_processor_maximum();

  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
    /* Checks if the CPU is in the affinity set of filter_strong_node */
    if ( _Processor_mask_Is_set( &filter_strong_node->Affinity, cpu_index ) ) {
      Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );

      if ( _Per_CPU_Is_processor_online( cpu ) ) {
        curr_node = _Scheduler_strong_APA_Get_scheduled( self, cpu );
        curr_priority = _Scheduler_Node_get_priority( curr_node );
        curr_priority = SCHEDULER_PRIORITY_PURIFY( curr_priority );

        if ( curr_priority > max_priority_num ) {
          lowest_scheduled = curr_node;
          max_priority_num = curr_priority;
        }
      }
    }
  }

  _Assert( lowest_scheduled != NULL );
  return lowest_scheduled;
}

static inline void _Scheduler_strong_APA_Extract_from_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
)
{
  Scheduler_strong_APA_Context *self;
  Scheduler_strong_APA_Node    *node;

  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( node_to_extract );

  _Scheduler_SMP_Extract_from_scheduled( &self->Base.Base, &node->Base.Base );
  /* Not removing it from Ready since the node could go in the READY state */
}

static inline void _Scheduler_strong_APA_Extract_from_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
)
{
  Scheduler_strong_APA_Node    *node;

  node = _Scheduler_strong_APA_Node_downcast( node_to_extract );

  if( !_Chain_Is_node_off_chain( &node->Ready_node ) ) {
    _Chain_Extract_unprotected( &node->Ready_node );
    _Chain_Set_off_chain( &node->Ready_node );
  }

}

static inline Scheduler_Node* _Scheduler_strong_APA_Get_lowest_reachable(
  Scheduler_strong_APA_Context *self,
  uint32_t                      front,
  uint32_t                      rear,
  Per_CPU_Control             **cpu_to_preempt
)
{
  Scheduler_Node              *lowest_reachable = NULL;
  Priority_Control             max_priority_num;
  uint32_t	               cpu_max;
  uint32_t	               cpu_index;
  Thread_Control              *curr_thread;
  Per_CPU_Control             *curr_CPU;
  Priority_Control             curr_priority;
  Scheduler_Node              *curr_node;
  Scheduler_strong_APA_Node   *curr_strong_node;
  Scheduler_strong_APA_CPU    *CPU;

  /* Max (Lowest) priority encountered so far */
  max_priority_num = 0;
  CPU = self->CPU;
  cpu_max = _SMP_Get_processor_maximum();

  while ( front <= rear ) {
    curr_CPU = CPU[ front ].cpu;
    front = front + 1;

    curr_node = _Scheduler_strong_APA_Get_scheduled( self, curr_CPU );
    curr_thread = curr_node->user;

    curr_priority = _Scheduler_Node_get_priority( curr_node );
    curr_priority = SCHEDULER_PRIORITY_PURIFY( curr_priority );

    curr_strong_node = _Scheduler_strong_APA_Node_downcast( curr_node );

    if ( curr_priority > max_priority_num ) {
      lowest_reachable = curr_node;
      max_priority_num = curr_priority;
      *cpu_to_preempt = curr_CPU;
    }

    if ( !curr_thread->is_idle ) {
      for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
        if ( _Processor_mask_Is_set( &curr_strong_node->Affinity, cpu_index ) ) {
          /* Checks if the thread_CPU is in the affinity set of the node */
          Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );
          if (
            _Per_CPU_Is_processor_online( cpu ) &&
            CPU[ cpu_index ].visited == false )
          {
            rear = rear + 1;
            CPU[ rear ].cpu = cpu;
            CPU[ cpu_index ].visited = true;
            CPU[ cpu_index ].preempting_node = curr_node;
          }
        }
      }
    }
  }
  /*
   * Since it is not allowed for a task to have an empty affinity set,
   * there would always be a lowest_reachable task, hence it would not be NULL
   */
  _Assert( lowest_reachable != NULL );

  return lowest_reachable;
}

static inline bool _Scheduler_strong_APA_Do_enqueue(
  Scheduler_Context *context,
  Scheduler_Node    *lowest_reachable,
  Scheduler_Node    *node,
  Priority_Control  insert_priority,
  Per_CPU_Control  *cpu_to_preempt
)
{
  bool                          needs_help;
  Priority_Control              node_priority;
  Priority_Control              lowest_priority;
  Scheduler_strong_APA_CPU     *CPU;
  Scheduler_Node               *curr_node;
  /* The first node that gets removed from the cpu */
  Scheduler_Node               *first_node;
  Scheduler_strong_APA_Node    *curr_strong_node;
  Per_CPU_Control              *curr_CPU;
  Scheduler_strong_APA_Context *self;
  Scheduler_Node               *next_node;


  self = _Scheduler_strong_APA_Get_self( context );
  CPU = self->CPU;

  _Scheduler_SMP_Node_change_state( node, SCHEDULER_SMP_NODE_READY );

  node_priority = _Scheduler_Node_get_priority( node );
  node_priority = SCHEDULER_PRIORITY_PURIFY( node_priority );

  if( lowest_reachable == NULL ) {
    /*
     * This means the affinity set of the newly arrived node
     * is empty.
     */
    lowest_priority = UINT64_MAX;
  } else {
    lowest_priority =  _Scheduler_Node_get_priority( lowest_reachable );
    lowest_priority = SCHEDULER_PRIORITY_PURIFY( lowest_priority );
  }

  if ( lowest_priority > node_priority ) {
    /*
     * Backtrack on the path from
     * _Thread_Get_CPU(lowest_reachable->user) to lowest_reachable, shifting
     * along every task
     */

    curr_node = CPU[ _Per_CPU_Get_index( cpu_to_preempt ) ].preempting_node;
    curr_strong_node = _Scheduler_strong_APA_Node_downcast( curr_node );
    curr_strong_node->cpu_to_preempt = cpu_to_preempt;

    /* Save which cpu to preempt in cpu_to_preempt value of the node */
    while ( curr_node != node ) {
      curr_CPU = _Thread_Get_CPU( curr_node->user );
      curr_node = CPU[ _Per_CPU_Get_index( curr_CPU ) ].preempting_node;
      curr_strong_node = _Scheduler_strong_APA_Node_downcast( curr_node );
      curr_strong_node->cpu_to_preempt =  curr_CPU;
     }

    curr_CPU = curr_strong_node->cpu_to_preempt;
    next_node = _Scheduler_strong_APA_Get_scheduled( self, curr_CPU );

    node_priority = _Scheduler_Node_get_priority( curr_node );
    node_priority = SCHEDULER_PRIORITY_PURIFY( node_priority );

    _Scheduler_SMP_Enqueue_to_scheduled(
      context,
      curr_node,
      node_priority,
      next_node,
      _Scheduler_SMP_Insert_scheduled,
      _Scheduler_strong_APA_Move_from_scheduled_to_ready,
      _Scheduler_strong_APA_Move_from_ready_to_scheduled,
      _Scheduler_strong_APA_Allocate_processor,
      _Scheduler_strong_APA_Get_idle,
      _Scheduler_strong_APA_Release_idle
    );

    curr_node = next_node;
    first_node = curr_node;
    curr_strong_node = _Scheduler_strong_APA_Node_downcast( curr_node );

    while ( curr_node != lowest_reachable ) {
      Thread_Control *next_node_idle;

      curr_CPU = curr_strong_node->cpu_to_preempt;
      next_node = _Scheduler_strong_APA_Get_scheduled( self, curr_CPU );
      next_node_idle = _Scheduler_Release_idle_thread_if_necessary(
        next_node,
        _Scheduler_strong_APA_Release_idle,
        context
      );
      /* curr_node preempts the next_node; */
      _Scheduler_SMP_Preempt(
        context,
        curr_node,
        next_node,
        next_node_idle,
        _Scheduler_strong_APA_Allocate_processor
      );

      if(curr_node == first_node) {
        _Scheduler_strong_APA_Move_from_ready_to_scheduled(context, first_node);
      }
      curr_node = next_node;
      curr_strong_node = _Scheduler_strong_APA_Node_downcast( curr_node );
    }

    _Scheduler_strong_APA_Move_from_scheduled_to_ready( context, lowest_reachable );

    needs_help = false;
  } else {
    needs_help = true;
  }

  /* Add it to Ready chain since it is now either scheduled or just ready. */
  _Scheduler_strong_APA_Insert_ready( context,node, insert_priority );

  return needs_help;
}

/*
 * BFS Algorithm for task arrival
 * Enqueue node either in the scheduled chain or in the ready chain.
 * node is the newly arrived node and is currently not scheduled.
 */
static inline bool _Scheduler_strong_APA_Enqueue(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{
  Scheduler_strong_APA_Context *self;
  Scheduler_strong_APA_CPU     *CPU;
  uint32_t	                cpu_max;
  uint32_t              	cpu_index;
  Per_CPU_Control              *cpu_to_preempt = NULL;
  Scheduler_Node               *lowest_reachable;
  Scheduler_strong_APA_Node    *strong_node;

  /* Denotes front and rear of the queue */
  uint32_t	front;
  uint32_t	rear;

  front = 0;
  rear = -1;

  self = _Scheduler_strong_APA_Get_self( context );
  strong_node = _Scheduler_strong_APA_Node_downcast( node );
  cpu_max = _SMP_Get_processor_maximum();
  CPU = self->CPU;

  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
    CPU[ cpu_index ].visited = false;

    /* Checks if the thread_CPU is in the affinity set of the node */
    if ( _Processor_mask_Is_set( &strong_node->Affinity, cpu_index ) ) {
      Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );

      if ( _Per_CPU_Is_processor_online( cpu ) ) {
        rear = rear + 1;
        CPU[ rear ].cpu = cpu;
        CPU[ cpu_index ].visited = true;
        CPU[ cpu_index ].preempting_node = node;
      }
    }
  }

  lowest_reachable = _Scheduler_strong_APA_Get_lowest_reachable(
                       self,
                       front,
                       rear,
                       &cpu_to_preempt
                     );
  /*
   * Since it is not allowed for a task to have an empty affinity set,
   * there would always be a lowest_reachable task, hence cpu_to_preempt
   * would not be NULL.
   */
  _Assert( cpu_to_preempt != NULL );
  return _Scheduler_strong_APA_Do_enqueue(
           context,
           lowest_reachable,
           node,
           insert_priority,
           cpu_to_preempt
         );
}

static inline void _Scheduler_strong_APA_Enqueue_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{
  _Scheduler_SMP_Enqueue_scheduled(
    context,
    node,
    insert_priority,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_strong_APA_Allocate_processor,
    _Scheduler_strong_APA_Get_idle,
    _Scheduler_strong_APA_Release_idle
  );
}

static inline bool _Scheduler_strong_APA_Do_ask_for_help(
  Scheduler_Context *context,
  Thread_Control    *the_thread,
  Scheduler_Node    *node
)
{
  return _Scheduler_SMP_Ask_for_help(
    context,
    the_thread,
    node,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_strong_APA_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_strong_APA_Move_from_scheduled_to_ready,
    _Scheduler_strong_APA_Get_lowest_scheduled,
    _Scheduler_strong_APA_Allocate_processor,
    _Scheduler_strong_APA_Release_idle
  );
}

static inline  void  _Scheduler_strong_APA_Do_set_affinity(
  Scheduler_Context *context,
  Scheduler_Node    *node_base,
  void              *arg
)
{
  Scheduler_strong_APA_Node *node;

  node = _Scheduler_strong_APA_Node_downcast( node_base );
  node->Affinity = *( (const Processor_mask *) arg );
}

void _Scheduler_strong_APA_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_strong_APA_Context *self =
      _Scheduler_strong_APA_Get_context( scheduler );

  _Scheduler_SMP_Initialize( &self->Base );
  _Chain_Initialize_empty( &self->Ready );
}

void _Scheduler_strong_APA_Yield(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Yield(
    context,
    thread,
    node,
    _Scheduler_strong_APA_Extract_from_scheduled,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Enqueue,
    _Scheduler_strong_APA_Enqueue_scheduled
  );
}

void _Scheduler_strong_APA_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  /*
   * Needed in case the node is scheduled node, since _SMP_Block only extracts
   * from the SMP scheduled chain and from the Strong APA Ready_chain
   * when the node is ready. But the Strong APA Ready_chain stores both
   * ready and scheduled nodes.
   */
  _Scheduler_strong_APA_Extract_from_ready(context, node);

  _Scheduler_SMP_Block(
    context,
    thread,
    node,
    _Scheduler_strong_APA_Extract_from_scheduled,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_strong_APA_Allocate_processor,
    _Scheduler_strong_APA_Get_idle
  );
}

void _Scheduler_strong_APA_Unblock(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Unblock(
    context,
    thread,
    node,
    _Scheduler_strong_APA_Do_update,
    _Scheduler_strong_APA_Enqueue,
    _Scheduler_strong_APA_Release_idle
  );
}

void _Scheduler_strong_APA_Update_priority(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Update_priority(
    context,
    thread,
    node,
    _Scheduler_strong_APA_Extract_from_scheduled,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Do_update,
    _Scheduler_strong_APA_Enqueue,
    _Scheduler_strong_APA_Enqueue_scheduled,
    _Scheduler_strong_APA_Do_ask_for_help
  );
}

bool _Scheduler_strong_APA_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_strong_APA_Do_ask_for_help(
    context,
    the_thread,
    node
  );
}

void _Scheduler_strong_APA_Reconsider_help_request(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Reconsider_help_request(
    context,
    the_thread,
    node,
    _Scheduler_strong_APA_Extract_from_ready
  );
}

void _Scheduler_strong_APA_Withdraw_node(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  Thread_Scheduler_state   next_state
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Withdraw_node(
    context,
    the_thread,
    node,
    next_state,
    _Scheduler_strong_APA_Extract_from_scheduled,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_strong_APA_Allocate_processor,
    _Scheduler_strong_APA_Get_idle
  );
}

void _Scheduler_strong_APA_Make_sticky(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  _Scheduler_SMP_Make_sticky(
    scheduler,
    the_thread,
    node,
    _Scheduler_strong_APA_Do_update,
    _Scheduler_strong_APA_Enqueue
  );
}

void _Scheduler_strong_APA_Clean_sticky(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  _Scheduler_SMP_Clean_sticky(
    scheduler,
    the_thread,
    node,
    _Scheduler_SMP_Extract_from_scheduled,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_strong_APA_Allocate_processor,
    _Scheduler_strong_APA_Get_idle,
    _Scheduler_strong_APA_Release_idle
  );
}

static inline void _Scheduler_strong_APA_Register_idle(
  Scheduler_Context *context,
  Scheduler_Node    *idle_base,
  Per_CPU_Control   *cpu
)
{
  Scheduler_strong_APA_Context *self;
  self = _Scheduler_strong_APA_Get_self( context );

  _Scheduler_strong_APA_Set_scheduled( self, idle_base, cpu );
}

void _Scheduler_strong_APA_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Add_processor(
    context,
    idle,
    _Scheduler_strong_APA_Has_ready,
    _Scheduler_strong_APA_Enqueue_scheduled,
    _Scheduler_strong_APA_Register_idle
  );
}

void _Scheduler_strong_APA_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle,
  Per_CPU_Control         *cpu
)
{
  Scheduler_Context *context;

  context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Do_start_idle(
    context,
    idle,
    cpu,
    _Scheduler_strong_APA_Register_idle
  );
}

Thread_Control *_Scheduler_strong_APA_Remove_processor(
  const Scheduler_Control *scheduler,
  Per_CPU_Control         *cpu
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_SMP_Remove_processor(
    context,
    cpu,
    _Scheduler_strong_APA_Extract_from_scheduled,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Enqueue,
    _Scheduler_strong_APA_Get_idle,
    _Scheduler_strong_APA_Release_idle
  );
}

void _Scheduler_strong_APA_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
  Scheduler_SMP_Node *smp_node;
  Scheduler_strong_APA_Node *strong_node;

  smp_node = _Scheduler_SMP_Node_downcast( node );
  strong_node = _Scheduler_strong_APA_Node_downcast( node );

  _Scheduler_SMP_Node_initialize( scheduler, smp_node, the_thread, priority );

  _Processor_mask_Assign(
    &strong_node->Affinity,
   _SMP_Get_online_processors()
  );
}

Status_Control _Scheduler_strong_APA_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  const Processor_mask    *affinity
)
{
  Scheduler_Context         *context;
  Scheduler_strong_APA_Node *node;
  Processor_mask             local_affinity;

  context = _Scheduler_Get_context( scheduler );
  _Processor_mask_And( &local_affinity, &context->Processors, affinity );

  if ( _Processor_mask_Is_zero( &local_affinity ) ) {
    return STATUS_INVALID_NUMBER;
  }

  node = _Scheduler_strong_APA_Node_downcast( node_base );

  if ( _Processor_mask_Is_equal( &node->Affinity, affinity ) )
    return STATUS_SUCCESSFUL;	/* Nothing to do. Return true. */

 _Processor_mask_Assign( &node->Affinity, &local_affinity );

 _Scheduler_SMP_Set_affinity(
   context,
   thread,
   node_base,
   &local_affinity,
   _Scheduler_strong_APA_Do_set_affinity,
   _Scheduler_strong_APA_Extract_from_scheduled,
   _Scheduler_strong_APA_Extract_from_ready,
   _Scheduler_strong_APA_Get_highest_ready,
   _Scheduler_strong_APA_Move_from_ready_to_scheduled,
   _Scheduler_strong_APA_Enqueue,
   _Scheduler_strong_APA_Allocate_processor,
   _Scheduler_strong_APA_Get_idle,
   _Scheduler_strong_APA_Release_idle
 );

  return STATUS_SUCCESSFUL;
}
