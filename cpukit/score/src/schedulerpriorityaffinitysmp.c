/**
 * @file
 *
 * @brief Deterministic Priority Affinity SMP Scheduler Implementation
 *
 * @ingroup RTEMSScoreSchedulerPriorityAffinitySMP
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerpriorityaffinitysmp.h>
#include <rtems/score/schedulerpriorityimpl.h>
#include <rtems/score/schedulersmpimpl.h>
#include <rtems/score/schedulerprioritysmpimpl.h>
#include <rtems/score/priority.h>

/*
 * The following methods which initially were static in schedulerprioritysmp.c
 * are shared with this scheduler. They are now public so they can be shared.
 *
 *  + _Scheduler_priority_SMP_Get_self
 *  + _Scheduler_priority_SMP_Insert_ready_fifo
 *  + _Scheduler_priority_SMP_Insert_ready_lifo
 *  + _Scheduler_priority_SMP_Thread_get_node
 *  + _Scheduler_priority_SMP_Move_from_scheduled_to_ready
 *  + _Scheduler_priority_SMP_Move_from_ready_to_scheduled
 *  + _Scheduler_priority_SMP_Extract_from_ready
 *  + _Scheduler_priority_SMP_Do_update
 */

static bool _Scheduler_priority_affinity_SMP_Priority_less_equal(
  const void       *to_insert,
  const Chain_Node *next
)
{
  return next != NULL
    && _Scheduler_SMP_Priority_less_equal( to_insert, next );
}

static Scheduler_priority_affinity_SMP_Node *
_Scheduler_priority_affinity_SMP_Node_downcast(
  Scheduler_Node *node
)
{
  return (Scheduler_priority_affinity_SMP_Node *) node;
}

/*
 * This method initializes the scheduler control information for
 * this scheduler instance.
 */
void _Scheduler_priority_affinity_SMP_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
  Scheduler_priority_affinity_SMP_Node *the_node;

  _Scheduler_priority_SMP_Node_initialize( scheduler, node, the_thread, priority );

  /*
   *  All we add is affinity information to the basic SMP node.
   */
  the_node = _Scheduler_priority_affinity_SMP_Node_downcast( node );
  _Processor_mask_Assign( &the_node->Affinity, _SMP_Get_online_processors() );
}

/*
 * This method is unique to this scheduler because it takes into
 * account affinity as it determines the highest ready thread.
 * Since this is used to pick a new thread to replace the victim,
 * the highest ready thread must have affinity such that it can
 * be executed on the victim's processor.
 */
static Scheduler_Node *_Scheduler_priority_affinity_SMP_Get_highest_ready(
  Scheduler_Context *context,
  Scheduler_Node    *victim
)
{
  Scheduler_priority_SMP_Context       *self =
    _Scheduler_priority_SMP_Get_self( context );
  Priority_Control                      index;
  Scheduler_Node                       *highest = NULL;
  Thread_Control                       *victim_thread;
  uint32_t                              victim_cpu_index;
  Scheduler_priority_affinity_SMP_Node *node;

  /*
   * This is done when we need to check if reevaluations are needed.
   */
  if ( victim == NULL ) {
    node = (Scheduler_priority_affinity_SMP_Node *)
      _Scheduler_priority_Ready_queue_first(
        &self->Bit_map,
        &self->Ready[ 0 ]
      );

    return &node->Base.Base.Base;
  }

  victim_thread = _Scheduler_Node_get_owner( victim );
  victim_cpu_index = _Per_CPU_Get_index( _Thread_Get_CPU( victim_thread ) );

  /**
   * @todo The deterministic priority scheduler structure is optimized
   * for insertion, extraction, and finding the highest priority
   * thread. Scanning the list of ready threads is not a purpose
   * for which it was optimized. There are optimizations to be
   * made in this loop.
   *
   * + by checking the major bit, we could potentially skip entire
   *   groups of 16.
   *
   * When using this scheduler as implemented, the application's
   * choice of numeric priorities and their distribution can have
   * an impact on performance.
   */
  for ( index = _Priority_bit_map_Get_highest( &self->Bit_map ) ;
        index <= PRIORITY_MAXIMUM;
        index++ )
  {
    Chain_Control   *chain =  &self->Ready[index];
    Chain_Node      *chain_node;
    for ( chain_node = _Chain_First( chain );
          chain_node != _Chain_Immutable_tail( chain ) ;
          chain_node = _Chain_Next( chain_node ) )
    {
      node = (Scheduler_priority_affinity_SMP_Node *) chain_node;

      /*
       * Can this thread run on this CPU?
       */
      if ( _Processor_mask_Is_set( &node->Affinity, victim_cpu_index ) ) {
        highest = &node->Base.Base.Base;
        break;
      }
    }
    if ( highest )
      break;
  }

  _Assert( highest != NULL );

  return highest;
}

/*
 * This method is very similar to _Scheduler_priority_affinity_SMP_Block
 * but has the difference that is invokes this scheduler's
 * get_highest_ready() support method.
 */
void _Scheduler_priority_affinity_SMP_Block(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Block(
    context,
    thread,
    node,
    _Scheduler_SMP_Extract_from_scheduled,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_affinity_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_exact
  );

  /*
   * Since this removed a single thread from the scheduled set
   * and selected the most appropriate thread from the ready
   * set to replace it, there should be no need for thread
   * migrations.
   */
}

/*
 * This method is unique to this scheduler because it must take into
 * account affinity as it searches for the lowest priority scheduled
 * thread. It ignores those which cannot be replaced by the filter
 * thread because the potential victim thread does not have affinity
 * for that processor.
 */
static Scheduler_Node * _Scheduler_priority_affinity_SMP_Get_lowest_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *filter_base
)
{
  Scheduler_SMP_Context *self = _Scheduler_SMP_Get_self( context );
  Scheduler_Node *lowest_scheduled = NULL;
  Chain_Control   *scheduled = &self->Scheduled;
  Chain_Node      *chain_node;
  Scheduler_priority_affinity_SMP_Node *filter =
    _Scheduler_priority_affinity_SMP_Node_downcast( filter_base );

  for ( chain_node = _Chain_Last( scheduled );
        chain_node != _Chain_Immutable_head( scheduled ) ;
        chain_node = _Chain_Previous( chain_node ) ) {
    Scheduler_priority_affinity_SMP_Node *node;
    Thread_Control                       *thread;
    uint32_t                              cpu_index;

    node = (Scheduler_priority_affinity_SMP_Node *) chain_node;

    /* cpu_index is the processor number thread is executing on */
    thread = _Scheduler_Node_get_owner( &node->Base.Base.Base );
    cpu_index = _Per_CPU_Get_index( _Thread_Get_CPU( thread ) );

    if ( _Processor_mask_Is_set( &filter->Affinity, cpu_index ) ) {
      lowest_scheduled = &node->Base.Base.Base;
      break;
    }

  }

  return lowest_scheduled;
}

/*
 * This method is unique to this scheduler because it must pass
 * _Scheduler_priority_affinity_SMP_Get_lowest_scheduled into
 * _Scheduler_SMP_Enqueue.
 */
static bool _Scheduler_priority_affinity_SMP_Enqueue_fifo(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{
  return _Scheduler_SMP_Enqueue(
    context,
    node,
    insert_priority,
    _Scheduler_priority_affinity_SMP_Priority_less_equal,
    _Scheduler_priority_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_priority_SMP_Move_from_scheduled_to_ready,
    _Scheduler_priority_affinity_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_exact
  );
}

/*
 * This method is invoked at the end of certain scheduling operations
 * to ensure that the highest priority ready thread cannot be scheduled
 * to execute. When we schedule with affinity, there is the possibility
 * that we need to migrate a thread to another core to ensure that the
 * highest priority ready threads are in fact scheduled.
 */
static void _Scheduler_priority_affinity_SMP_Check_for_migrations(
  Scheduler_Context *context
)
{
  Scheduler_priority_SMP_Context *self;
  Scheduler_Node                 *lowest_scheduled;
  Scheduler_Node                 *highest_ready;

  self = _Scheduler_priority_SMP_Get_self( context );

  while (1) {
    Priority_Control lowest_scheduled_priority;
    Priority_Control insert_priority;

    if ( _Priority_bit_map_Is_empty( &self->Bit_map ) ) {
      /* Nothing to do */
      break;
    }

    highest_ready =
      _Scheduler_priority_affinity_SMP_Get_highest_ready( context, NULL );

    lowest_scheduled =
      _Scheduler_priority_affinity_SMP_Get_lowest_scheduled(
        context,
        highest_ready
      );

    /*
     * If we can't find a thread to displace from the scheduled set,
     * then we have placed all the highest priority threads possible
     * in the scheduled set.
     *
     * We found the absolute highest priority thread without
     * considering affinity. But now we have to consider that thread's
     * affinity as we look to place it.
     */

    if ( lowest_scheduled == NULL )
      break;

    lowest_scheduled_priority =
      _Scheduler_SMP_Node_priority( lowest_scheduled );

    if (
      _Scheduler_SMP_Priority_less_equal(
        &lowest_scheduled_priority,
        &highest_ready->Node.Chain
      )
    ) {
      break;
    }

    /*
     * But if we found a thread which is lower priority than one
     * in the ready set, then we need to swap them out.
     */

    _Scheduler_priority_SMP_Extract_from_ready( context, highest_ready );
    insert_priority = _Scheduler_SMP_Node_priority( highest_ready );
    insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );
    _Scheduler_SMP_Enqueue_to_scheduled(
      context,
      highest_ready,
      insert_priority,
      lowest_scheduled,
      _Scheduler_SMP_Insert_scheduled,
      _Scheduler_priority_SMP_Move_from_scheduled_to_ready,
      _Scheduler_SMP_Allocate_processor_exact
    );
  }
}

/*
 * This is the public scheduler specific Unblock operation.
 */
void _Scheduler_priority_affinity_SMP_Unblock(
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
    _Scheduler_priority_SMP_Do_update,
    _Scheduler_priority_affinity_SMP_Enqueue_fifo
  );

  /*
   * Perform any thread migrations that are needed due to these changes.
   */
  _Scheduler_priority_affinity_SMP_Check_for_migrations( context );
}

/*
 *  This is unique to this scheduler because it passes scheduler specific
 *  get_lowest_scheduled helper to _Scheduler_SMP_Enqueue.
 */
static bool _Scheduler_priority_affinity_SMP_Enqueue(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{
  return _Scheduler_SMP_Enqueue(
    context,
    node,
    insert_priority,
    _Scheduler_priority_affinity_SMP_Priority_less_equal,
    _Scheduler_priority_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_priority_SMP_Move_from_scheduled_to_ready,
    _Scheduler_priority_affinity_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_exact
  );
}

/*
 * This method is unique to this scheduler because it must
 * invoke _Scheduler_SMP_Enqueue_scheduled() with
 * this scheduler's get_highest_ready() helper.
 */
static bool _Scheduler_priority_affinity_SMP_Enqueue_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{
  return _Scheduler_SMP_Enqueue_scheduled(
    context,
    node,
    insert_priority,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_affinity_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_exact
  );
}

static bool _Scheduler_priority_affinity_SMP_Do_ask_for_help(
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
    _Scheduler_priority_SMP_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_priority_SMP_Move_from_scheduled_to_ready,
    _Scheduler_SMP_Get_lowest_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

void _Scheduler_priority_affinity_SMP_Update_priority(
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
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_SMP_Do_update,
    _Scheduler_priority_affinity_SMP_Enqueue,
    _Scheduler_priority_affinity_SMP_Enqueue_scheduled,
    _Scheduler_priority_affinity_SMP_Do_ask_for_help
  );

  /*
   * Perform any thread migrations that are needed due to these changes.
   */
  _Scheduler_priority_affinity_SMP_Check_for_migrations( context );
}

bool _Scheduler_priority_affinity_SMP_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_priority_affinity_SMP_Do_ask_for_help( context, the_thread, node );
}

void _Scheduler_priority_affinity_SMP_Reconsider_help_request(
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
    _Scheduler_priority_SMP_Extract_from_ready
  );
}

void _Scheduler_priority_affinity_SMP_Withdraw_node(
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
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_affinity_SMP_Get_highest_ready,
    _Scheduler_priority_SMP_Move_from_ready_to_scheduled,
    _Scheduler_SMP_Allocate_processor_lazy
  );
}

void _Scheduler_priority_affinity_SMP_Add_processor(
  const Scheduler_Control *scheduler,
  Thread_Control          *idle
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Add_processor(
    context,
    idle,
    _Scheduler_priority_SMP_Has_ready,
    _Scheduler_priority_affinity_SMP_Enqueue_scheduled,
    _Scheduler_SMP_Do_nothing_register_idle
  );
}

Thread_Control *_Scheduler_priority_affinity_SMP_Remove_processor(
  const Scheduler_Control *scheduler,
  Per_CPU_Control         *cpu
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_SMP_Remove_processor(
    context,
    cpu,
    _Scheduler_priority_SMP_Extract_from_ready,
    _Scheduler_priority_affinity_SMP_Enqueue
  );
}

bool _Scheduler_priority_affinity_SMP_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  const Processor_mask    *affinity
)
{
  Scheduler_Context                    *context;
  Scheduler_priority_affinity_SMP_Node *node;
  States_Control                        current_state;
  Processor_mask                        my_affinity;

  context = _Scheduler_Get_context( scheduler );
  _Processor_mask_And( &my_affinity, &context->Processors, affinity );

  if ( _Processor_mask_Count( &my_affinity ) == 0 ) {
    return false;
  }

  node = _Scheduler_priority_affinity_SMP_Node_downcast( node_base );

  /*
   * The old and new set are the same, there is no point in
   * doing anything.
   */
  if ( _Processor_mask_Is_equal( &node->Affinity, affinity ) )
    return true;

  current_state = thread->current_state;

  if ( _States_Is_ready( current_state ) ) {
    _Scheduler_priority_affinity_SMP_Block( scheduler, thread, &node->Base.Base.Base );
  }

  _Processor_mask_Assign( &node->Affinity, affinity );

  if ( _States_Is_ready( current_state ) ) {
    /*
     * FIXME: Do not ignore threads in need for help.
     */
    (void) _Scheduler_priority_affinity_SMP_Unblock( scheduler, thread, &node->Base.Base.Base );
  }

  return true;
}
