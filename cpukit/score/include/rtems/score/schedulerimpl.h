/**
 * @file
 *
 * @brief Inlined Routines Associated with the Manipulation of the Scheduler
 *
 * This inline file contains all of the inlined routines associated with
 * the manipulation of the scheduler.
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *  Copyright (c) 2014-2015 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERIMPL_H
#define _RTEMS_SCORE_SCHEDULERIMPL_H

#include <rtems/score/scheduler.h>
#include <rtems/score/cpusetimpl.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threadimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreScheduler
 */
/**@{**/

/**
 *  @brief Initializes the scheduler to the policy chosen by the user.
 *
 *  This routine initializes the scheduler to the policy chosen by the user
 *  through confdefs, or to the priority scheduler with ready chains by
 *  default.
 */
void _Scheduler_Handler_initialization( void );

RTEMS_INLINE_ROUTINE Scheduler_Context *_Scheduler_Get_context(
  const Scheduler_Control *scheduler
)
{
  return scheduler->context;
}

RTEMS_INLINE_ROUTINE const Scheduler_Control *_Scheduler_Get(
  const Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  return the_thread->Scheduler.control;
#else
  (void) the_thread;

  return &_Scheduler_Table[ 0 ];
#endif
}

RTEMS_INLINE_ROUTINE const Scheduler_Control *_Scheduler_Get_own(
  const Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  return the_thread->Scheduler.own_control;
#else
  (void) the_thread;

  return &_Scheduler_Table[ 0 ];
#endif
}

RTEMS_INLINE_ROUTINE const Scheduler_Control *_Scheduler_Get_by_CPU_index(
  uint32_t cpu_index
)
{
#if defined(RTEMS_SMP)
  return _Scheduler_Assignments[ cpu_index ].scheduler;
#else
  (void) cpu_index;

  return &_Scheduler_Table[ 0 ];
#endif
}

RTEMS_INLINE_ROUTINE const Scheduler_Control *_Scheduler_Get_by_CPU(
  const Per_CPU_Control *cpu
)
{
  uint32_t cpu_index = _Per_CPU_Get_index( cpu );

  return _Scheduler_Get_by_CPU_index( cpu_index );
}

RTEMS_INLINE_ROUTINE Scheduler_Node *_Scheduler_Thread_get_own_node(
  const Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  return the_thread->Scheduler.own_node;
#else
  return the_thread->Scheduler.node;
#endif
}

#if defined(RTEMS_SMP)
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Node_get_user(
  const Scheduler_Node *node
)
{
  return node->user;
}
#endif

/**
 * The preferred method to add a new scheduler is to define the jump table
 * entries and add a case to the _Scheduler_Initialize routine.
 *
 * Generic scheduling implementations that rely on the ready queue only can
 * be found in the _Scheduler_queue_XXX functions.
 */

/*
 * Passing the Scheduler_Control* to these functions allows for multiple
 * scheduler's to exist simultaneously, which could be useful on an SMP
 * system.  Then remote Schedulers may be accessible.  How to protect such
 * accesses remains an open problem.
 */

/**
 * @brief General scheduling decision.
 *
 * This kernel routine implements the scheduling decision logic for
 * the scheduler. It does NOT dispatch.
 *
 * @param[in] the_thread The thread which state changed previously.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Schedule( Thread_Control *the_thread )
{
  const Scheduler_Control *scheduler = _Scheduler_Get( the_thread );

  ( *scheduler->Operations.schedule )( scheduler, the_thread );
}

#if defined(RTEMS_SMP)
typedef struct {
  Thread_Control *needs_help;
  Thread_Control *next_needs_help;
} Scheduler_Ask_for_help_context ;

RTEMS_INLINE_ROUTINE bool _Scheduler_Ask_for_help_visitor(
  Resource_Node *resource_node,
  void          *arg
)
{
  bool done;
  Scheduler_Ask_for_help_context *help_context = arg;
  Thread_Control *previous_needs_help = help_context->needs_help;
  Thread_Control *next_needs_help;
  Thread_Control *offers_help =
    THREAD_RESOURCE_NODE_TO_THREAD( resource_node );
  const Scheduler_Control *scheduler = _Scheduler_Get_own( offers_help );

  next_needs_help = ( *scheduler->Operations.ask_for_help )(
    scheduler,
    offers_help,
    previous_needs_help
  );

  done = next_needs_help != previous_needs_help;

  if ( done ) {
    help_context->next_needs_help = next_needs_help;
  }

  return done;
}

/**
 * @brief Ask threads depending on resources owned by the thread for help.
 *
 * A thread is in need for help if it lost its assigned processor due to
 * pre-emption by a higher priority thread or it was not possible to assign it
 * a processor since its priority is to low on its current scheduler instance.
 *
 * The run-time of this function depends on the size of the resource tree of
 * the thread needing help and other resource trees in case threads in need for
 * help are produced during this operation.
 *
 * @param[in] needs_help The thread needing help.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Ask_for_help(
  Thread_Control *needs_help
)
{
  do {
    const Scheduler_Control *scheduler = _Scheduler_Get_own( needs_help );

    needs_help = ( *scheduler->Operations.ask_for_help )(
      scheduler,
      needs_help,
      needs_help
    );

    if ( needs_help != NULL ) {
      Scheduler_Ask_for_help_context help_context = { needs_help, NULL };

      _Resource_Iterate(
        &needs_help->Resource_node,
        _Scheduler_Ask_for_help_visitor,
        &help_context
      );

      needs_help = help_context.next_needs_help;
    }
  } while ( needs_help != NULL );
}

RTEMS_INLINE_ROUTINE void _Scheduler_Ask_for_help_if_necessary(
  Thread_Control *needs_help
)
{
  if (
    needs_help != NULL
      && _Resource_Node_owns_resources( &needs_help->Resource_node )
  ) {
    Scheduler_Node *node = _Scheduler_Thread_get_own_node( needs_help );

    if (
      node->help_state != SCHEDULER_HELP_ACTIVE_RIVAL
        || _Scheduler_Node_get_user( node ) != needs_help
    ) {
      _Scheduler_Ask_for_help( needs_help );
    }
  }
}
#endif

/**
 * @brief Scheduler yield with a particular thread.
 *
 * This routine is invoked when a thread wishes to voluntarily transfer control
 * of the processor to another thread.
 *
 * @param[in] the_thread The yielding thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Yield( Thread_Control *the_thread )
{
  const Scheduler_Control *scheduler = _Scheduler_Get( the_thread );
#if defined(RTEMS_SMP)
  Thread_Control *needs_help;

  needs_help =
#endif
  ( *scheduler->Operations.yield )( scheduler, the_thread );

#if defined(RTEMS_SMP)
  _Scheduler_Ask_for_help_if_necessary( needs_help );
#endif
}

/**
 * @brief Blocks a thread with respect to the scheduler.
 *
 * This routine removes @a the_thread from the scheduling decision for
 * the scheduler. The primary task is to remove the thread from the
 * ready queue.  It performs any necessary schedulering operations
 * including the selection of a new heir thread.
 *
 * @param[in] the_thread The thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Block( Thread_Control *the_thread )
{
  const Scheduler_Control *scheduler = _Scheduler_Get( the_thread );

  ( *scheduler->Operations.block )( scheduler, the_thread );
}

/**
 * @brief Unblocks a thread with respect to the scheduler.
 *
 * This routine adds @a the_thread to the scheduling decision for
 * the scheduler.  The primary task is to add the thread to the
 * ready queue per the schedulering policy and update any appropriate
 * scheduling variables, for example the heir thread.
 *
 * @param[in] the_thread The thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Unblock( Thread_Control *the_thread )
{
  const Scheduler_Control *scheduler = _Scheduler_Get( the_thread );
#if defined(RTEMS_SMP)
  Thread_Control *needs_help;

  needs_help =
#endif
  ( *scheduler->Operations.unblock )( scheduler, the_thread );

#if defined(RTEMS_SMP)
  _Scheduler_Ask_for_help_if_necessary( needs_help );
#endif
}

/**
 * @brief Propagates a priority change of a thread to the scheduler.
 *
 * The caller must ensure that the thread is in the ready state.  The caller
 * must ensure that the priority value actually changed and is not equal to the
 * current priority value.
 *
 * The operation must update the heir and thread dispatch necessary variables
 * in case the set of scheduled threads changes.
 *
 * @param[in] the_thread The thread changing its priority.
 * @param[in] new_priority The new thread priority.
 * @param[in] prepend_it In case this is true, then enqueue the thread as the
 * first of its priority group, otherwise enqueue the thread as the last of its
 * priority group.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Change_priority(
  Thread_Control          *the_thread,
  Priority_Control         new_priority,
  bool                     prepend_it
)
{
  const Scheduler_Control *scheduler = _Scheduler_Get_own( the_thread );
#if defined(RTEMS_SMP)
  Thread_Control *needs_help;

  needs_help =
#endif
  ( *scheduler->Operations.change_priority )(
    scheduler,
    the_thread,
    new_priority,
    prepend_it
  );

#if defined(RTEMS_SMP)
  _Scheduler_Ask_for_help_if_necessary( needs_help );
#endif
}

/**
 * @brief Initializes a scheduler node.
 *
 * The scheduler node contains arbitrary data on function entry.  The caller
 * must ensure that _Scheduler_Node_destroy() will be called after a
 * _Scheduler_Node_initialize() before the memory of the scheduler node is
 * destroyed.
 *
 * @param[in] scheduler The scheduler instance.
 * @param[in] the_thread The thread containing the scheduler node.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Node_initialize(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  return ( *scheduler->Operations.node_initialize )( scheduler, the_thread );
}

/**
 * @brief Destroys a scheduler node.
 *
 * The caller must ensure that _Scheduler_Node_destroy() will be called only
 * after a corresponding _Scheduler_Node_initialize().
 *
 * @param[in] scheduler The scheduler instance.
 * @param[in] the_thread The thread containing the scheduler node.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Node_destroy(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
  ( *scheduler->Operations.node_destroy )( scheduler, the_thread );
}

/**
 * @brief Updates the scheduler about a priority change of a not ready thread.
 *
 * @param[in] the_thread The thread.
 * @param[in] new_priority The new priority of the thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Update_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority
)
{
  const Scheduler_Control *scheduler = _Scheduler_Get( the_thread );

  ( *scheduler->Operations.update_priority )(
    scheduler,
    the_thread,
    new_priority
  );
}

/**
 * @brief Compares two priority values.
 *
 * @param[in] scheduler The scheduler instance.
 * @param[in] p1 The first priority value.
 * @param[in] p2 The second priority value.
 *
 * @retval negative The value @a p1 encodes a lower priority than @a p2 in the
 * intuitive sense of priority.
 * @retval 0 The priorities @a p1 and @a p2 are equal.
 * @retval positive The value @a p1 encodes a higher priority than @a p2 in the
 * intuitive sense of priority.
 *
 * @see _Scheduler_Is_priority_lower_than() and
 * _Scheduler_Is_priority_higher_than().
 */
RTEMS_INLINE_ROUTINE int _Scheduler_Priority_compare(
  const Scheduler_Control *scheduler,
  Priority_Control         p1,
  Priority_Control         p2
)
{
  return ( *scheduler->Operations.priority_compare )( p1, p2 );
}

/**
 * @brief Releases a job of a thread with respect to the scheduler.
 *
 * @param[in] the_thread The thread.
 * @param[in] length The period length.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Release_job(
  Thread_Control *the_thread,
  uint32_t        length
)
{
  const Scheduler_Control *scheduler = _Scheduler_Get( the_thread );

  ( *scheduler->Operations.release_job )( scheduler, the_thread, length );
}

/**
 * @brief Scheduler method invoked at each clock tick.
 *
 * This method is invoked at each clock tick to allow the scheduler
 * implementation to perform any activities required.  For the
 * scheduler which support standard RTEMS features, this includes
 * time-slicing management.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Tick( void )
{
  uint32_t cpu_count = _SMP_Get_processor_count();
  uint32_t cpu_index;

  for ( cpu_index = 0 ; cpu_index < cpu_count ; ++cpu_index ) {
    const Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );
    const Scheduler_Control *scheduler = _Scheduler_Get_by_CPU( cpu );
    Thread_Control *executing = cpu->executing;

    if ( scheduler != NULL && executing != NULL ) {
      ( *scheduler->Operations.tick )( scheduler, executing );
    }
  }
}

/**
 * @brief Starts the idle thread for a particular processor.
 *
 * @param[in] scheduler The scheduler instance.
 * @param[in,out] the_thread The idle thread for the processor.
 * @param[in,out] cpu The processor for the idle thread.
 *
 * @see _Thread_Create_idle().
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Per_CPU_Control         *cpu
)
{
  ( *scheduler->Operations.start_idle )( scheduler, the_thread, cpu );
}

#if defined(RTEMS_SMP)
RTEMS_INLINE_ROUTINE const Scheduler_Assignment *_Scheduler_Get_assignment(
  uint32_t cpu_index
)
{
  return &_Scheduler_Assignments[ cpu_index ];
}

RTEMS_INLINE_ROUTINE bool _Scheduler_Is_mandatory_processor(
  const Scheduler_Assignment *assignment
)
{
  return (assignment->attributes & SCHEDULER_ASSIGN_PROCESSOR_MANDATORY) != 0;
}

RTEMS_INLINE_ROUTINE bool _Scheduler_Should_start_processor(
  const Scheduler_Assignment *assignment
)
{
  return assignment->scheduler != NULL;
}
#endif /* defined(RTEMS_SMP) */

RTEMS_INLINE_ROUTINE bool _Scheduler_Has_processor_ownership(
  const Scheduler_Control *scheduler,
  uint32_t cpu_index
)
{
#if defined(RTEMS_SMP)
  const Scheduler_Assignment *assignment =
    _Scheduler_Get_assignment( cpu_index );

  return assignment->scheduler == scheduler;
#else
  (void) scheduler;
  (void) cpu_index;

  return true;
#endif
}

RTEMS_INLINE_ROUTINE void _Scheduler_Set(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread
)
{
#if defined(RTEMS_SMP)
  const Scheduler_Control *current_scheduler = _Scheduler_Get( the_thread );

  if ( current_scheduler != scheduler ) {
    _Thread_Set_state( the_thread, STATES_MIGRATING );
    _Scheduler_Node_destroy( current_scheduler, the_thread );
    the_thread->Scheduler.own_control = scheduler;
    the_thread->Scheduler.control = scheduler;
    _Scheduler_Node_initialize( scheduler, the_thread );
    _Scheduler_Update_priority( the_thread, the_thread->current_priority );
    _Thread_Clear_state( the_thread, STATES_MIGRATING );
  }
#else
  (void) scheduler;
#endif
}

#if defined(__RTEMS_HAVE_SYS_CPUSET_H__)

RTEMS_INLINE_ROUTINE void _Scheduler_Get_processor_set(
  const Scheduler_Control *scheduler,
  size_t                   cpusetsize,
  cpu_set_t               *cpuset
)
{
  uint32_t cpu_count = _SMP_Get_processor_count();
  uint32_t cpu_index;

  CPU_ZERO_S( cpusetsize, cpuset );

  for ( cpu_index = 0 ; cpu_index < cpu_count ; ++cpu_index ) {
#if defined(RTEMS_SMP)
    if ( _Scheduler_Has_processor_ownership( scheduler, cpu_index ) ) {
      CPU_SET_S( (int) cpu_index, cpusetsize, cpuset );
    }
#else
    (void) scheduler;

    CPU_SET_S( (int) cpu_index, cpusetsize, cpuset );
#endif
  }
}

RTEMS_INLINE_ROUTINE bool _Scheduler_default_Get_affinity_body(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  size_t                   cpusetsize,
  cpu_set_t               *cpuset
)
{
  (void) the_thread;

  _Scheduler_Get_processor_set( scheduler, cpusetsize, cpuset );

  return true;
}

bool _Scheduler_Get_affinity(
  Thread_Control *the_thread,
  size_t          cpusetsize,
  cpu_set_t      *cpuset
);

RTEMS_INLINE_ROUTINE bool _Scheduler_default_Set_affinity_body(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  size_t                   cpusetsize,
  const cpu_set_t         *cpuset
)
{
  uint32_t cpu_count = _SMP_Get_processor_count();
  uint32_t cpu_index;
  bool     ok = true;

  for ( cpu_index = 0 ; cpu_index < cpu_count ; ++cpu_index ) {
#if defined(RTEMS_SMP)
    const Scheduler_Control *scheduler_of_cpu =
      _Scheduler_Get_by_CPU_index( cpu_index );

    ok = ok
      && ( CPU_ISSET_S( (int) cpu_index, cpusetsize, cpuset )
        || ( !CPU_ISSET_S( (int) cpu_index, cpusetsize, cpuset )
          && scheduler != scheduler_of_cpu ) );
#else
    (void) scheduler;

    ok = ok && CPU_ISSET_S( (int) cpu_index, cpusetsize, cpuset );
#endif
  }

  return ok;
}

bool _Scheduler_Set_affinity(
  Thread_Control          *the_thread,
  size_t                   cpusetsize,
  const cpu_set_t         *cpuset
);

#endif /* defined(__RTEMS_HAVE_SYS_CPUSET_H__) */

RTEMS_INLINE_ROUTINE void _Scheduler_Update_heir(
  Thread_Control *new_heir,
  bool            force_dispatch
)
{
  Thread_Control *heir = _Thread_Heir;

  if ( heir != new_heir && ( heir->is_preemptible || force_dispatch ) ) {
    _Thread_Heir = new_heir;
    _Thread_Dispatch_necessary = true;
  }
}

RTEMS_INLINE_ROUTINE void _Scheduler_Generic_block(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  void                  ( *extract )(
                             const Scheduler_Control *,
                             Thread_Control * ),
  void                  ( *schedule )(
                             const Scheduler_Control *,
                             Thread_Control *,
                             bool )
)
{
  ( *extract )( scheduler, the_thread );

  /* TODO: flash critical section? */

  if ( _Thread_Is_executing( the_thread ) || _Thread_Is_heir( the_thread ) ) {
    ( *schedule )( scheduler, the_thread, true );
  }
}

/**
 * @brief Returns true if @a p1 encodes a lower priority than @a p2 in the
 * intuitive sense of priority.
 */
RTEMS_INLINE_ROUTINE bool _Scheduler_Is_priority_lower_than(
  const Scheduler_Control *scheduler,
  Priority_Control         p1,
  Priority_Control         p2
)
{
  return _Scheduler_Priority_compare( scheduler, p1,  p2 ) < 0;
}

/**
 * @brief Returns true if @a p1 encodes a higher priority than @a p2 in the
 * intuitive sense of priority.
 */
RTEMS_INLINE_ROUTINE bool _Scheduler_Is_priority_higher_than(
  const Scheduler_Control *scheduler,
  Priority_Control         p1,
  Priority_Control         p2
)
{
  return _Scheduler_Priority_compare( scheduler, p1,  p2 ) > 0;
}

RTEMS_INLINE_ROUTINE uint32_t _Scheduler_Get_processor_count(
  const Scheduler_Control *scheduler
)
{
#if defined(RTEMS_SMP)
  return _Scheduler_Get_context( scheduler )->processor_count;
#else
  (void) scheduler;

  return 1;
#endif
}

RTEMS_INLINE_ROUTINE Objects_Id _Scheduler_Build_id( uint32_t scheduler_index )
{
  return _Objects_Build_id(
    OBJECTS_FAKE_OBJECTS_API,
    OBJECTS_FAKE_OBJECTS_SCHEDULERS,
    _Objects_Local_node,
    scheduler_index + 1
  );
}

RTEMS_INLINE_ROUTINE uint32_t _Scheduler_Get_index_by_id( Objects_Id id )
{
  uint32_t minimum_id = _Scheduler_Build_id( 0 );

  return id - minimum_id;
}

RTEMS_INLINE_ROUTINE bool _Scheduler_Get_by_id(
  Objects_Id                id,
  const Scheduler_Control **scheduler_p
)
{
  uint32_t index = _Scheduler_Get_index_by_id( id );
  const Scheduler_Control *scheduler = &_Scheduler_Table[ index ];

  *scheduler_p = scheduler;

  return index < _Scheduler_Count
    && _Scheduler_Get_processor_count( scheduler ) > 0;
}

RTEMS_INLINE_ROUTINE bool _Scheduler_Is_id_valid( Objects_Id id )
{
  const Scheduler_Control *scheduler;
  bool ok = _Scheduler_Get_by_id( id, &scheduler );

  (void) scheduler;

  return ok;
}

RTEMS_INLINE_ROUTINE uint32_t _Scheduler_Get_index(
  const Scheduler_Control *scheduler
)
{
  return (uint32_t) (scheduler - &_Scheduler_Table[ 0 ]);
}

RTEMS_INLINE_ROUTINE Scheduler_Node *_Scheduler_Thread_get_node(
  const Thread_Control *the_thread
)
{
  return the_thread->Scheduler.node;
}

RTEMS_INLINE_ROUTINE void _Scheduler_Node_do_initialize(
  Scheduler_Node *node,
  Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  node->user = the_thread;
  node->help_state = SCHEDULER_HELP_YOURSELF;
  node->owner = the_thread;
  node->idle = NULL;
  node->accepts_help = the_thread;
#else
  (void) node;
  (void) the_thread;
#endif
}

#if defined(RTEMS_SMP)
/**
 * @brief Gets an idle thread from the scheduler instance.
 *
 * @param[in] context The scheduler instance context.
 *
 * @retval idle An idle thread for use.  This function must always return an
 * idle thread.  If none is available, then this is a fatal error.
 */
typedef Thread_Control *( *Scheduler_Get_idle_thread )(
  Scheduler_Context *context
);

/**
 * @brief Releases an idle thread to the scheduler instance for reuse.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] idle The idle thread to release
 */
typedef void ( *Scheduler_Release_idle_thread )(
  Scheduler_Context *context,
  Thread_Control    *idle
);

RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Node_get_owner(
  const Scheduler_Node *node
)
{
  return node->owner;
}

RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Node_get_idle(
  const Scheduler_Node *node
)
{
  return node->idle;
}

RTEMS_INLINE_ROUTINE void _Scheduler_Node_set_user(
  Scheduler_Node *node,
  Thread_Control *user
)
{
  node->user = user;
}

RTEMS_INLINE_ROUTINE void _Scheduler_Thread_set_node(
  Thread_Control *the_thread,
  Scheduler_Node *node
)
{
  the_thread->Scheduler.node = node;
}

RTEMS_INLINE_ROUTINE void _Scheduler_Thread_set_scheduler_and_node(
  Thread_Control       *the_thread,
  Scheduler_Node       *node,
  const Thread_Control *previous_user_of_node
)
{
  const Scheduler_Control *scheduler =
    _Scheduler_Get_own( previous_user_of_node );

  the_thread->Scheduler.control = scheduler;
  _Scheduler_Thread_set_node( the_thread, node );
}

extern const bool _Scheduler_Thread_state_valid_state_changes[ 3 ][ 3 ];

RTEMS_INLINE_ROUTINE void _Scheduler_Thread_change_state(
  Thread_Control         *the_thread,
  Thread_Scheduler_state  new_state
)
{
  _Assert(
    _Scheduler_Thread_state_valid_state_changes
      [ the_thread->Scheduler.state ][ new_state ]
  );

  the_thread->Scheduler.state = new_state;
}

/**
 * @brief Changes the scheduler help state of a thread.
 *
 * @param[in] the_thread The thread.
 * @param[in] new_help_state The new help state.
 *
 * @return The previous help state.
 */
RTEMS_INLINE_ROUTINE Scheduler_Help_state _Scheduler_Thread_change_help_state(
  Thread_Control       *the_thread,
  Scheduler_Help_state  new_help_state
)
{
  Scheduler_Node *node = _Scheduler_Thread_get_own_node( the_thread );
  Scheduler_Help_state previous_help_state = node->help_state;

  node->help_state = new_help_state;

  return previous_help_state;
}

/**
 * @brief Changes the resource tree root of a thread.
 *
 * For each node of the resource sub-tree specified by the top thread the
 * scheduler asks for help.  So the root thread gains access to all scheduler
 * nodes corresponding to the resource sub-tree.  In case a thread previously
 * granted help is displaced by this operation, then the scheduler asks for
 * help using its remaining resource tree.
 *
 * The run-time of this function depends on the size of the resource sub-tree
 * and other resource trees in case threads in need for help are produced
 * during this operation.
 *
 * @param[in] top The thread specifying the resource sub-tree top.
 * @param[in] root The thread specifying the new resource sub-tree root.
 */
void _Scheduler_Thread_change_resource_root(
  Thread_Control *top,
  Thread_Control *root
);

RTEMS_INLINE_ROUTINE void _Scheduler_Set_idle_thread(
  Scheduler_Node *node,
  Thread_Control *idle
)
{
  _Assert(
    node->help_state == SCHEDULER_HELP_ACTIVE_OWNER
      || node->help_state == SCHEDULER_HELP_ACTIVE_RIVAL
  );
  _Assert( _Scheduler_Node_get_idle( node ) == NULL );
  _Assert(
    _Scheduler_Node_get_owner( node ) == _Scheduler_Node_get_user( node )
  );

  _Scheduler_Thread_set_node( idle, node );

  _Scheduler_Node_set_user( node, idle );
  node->idle = idle;
}

/**
 * @brief Use an idle thread for this scheduler node.
 *
 * A thread in the SCHEDULER_HELP_ACTIVE_OWNER or SCHEDULER_HELP_ACTIVE_RIVAL
 * helping state may use an idle thread for the scheduler node owned by itself
 * in case it executes currently using another scheduler node or in case it is
 * in a blocking state.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] node The node which wants to use the idle thread.
 * @param[in] get_idle_thread Function to get an idle thread.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Use_idle_thread(
  Scheduler_Context         *context,
  Scheduler_Node            *node,
  Scheduler_Get_idle_thread  get_idle_thread
)
{
  Thread_Control *idle = ( *get_idle_thread )( context );

  _Scheduler_Set_idle_thread( node, idle );

  return idle;
}

typedef enum {
  SCHEDULER_TRY_TO_SCHEDULE_DO_SCHEDULE,
  SCHEDULER_TRY_TO_SCHEDULE_DO_IDLE_EXCHANGE,
  SCHEDULER_TRY_TO_SCHEDULE_DO_BLOCK
} Scheduler_Try_to_schedule_action;

/**
 * @brief Try to schedule this scheduler node.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] node The node which wants to get scheduled.
 * @param[in] idle A potential idle thread used by a potential victim node.
 * @param[in] get_idle_thread Function to get an idle thread.
 *
 * @retval true This node can be scheduled.
 * @retval false Otherwise.
 */
RTEMS_INLINE_ROUTINE Scheduler_Try_to_schedule_action
_Scheduler_Try_to_schedule_node(
  Scheduler_Context         *context,
  Scheduler_Node            *node,
  Thread_Control            *idle,
  Scheduler_Get_idle_thread  get_idle_thread
)
{
  Scheduler_Try_to_schedule_action action;
  Thread_Control *owner;
  Thread_Control *user;

  action = SCHEDULER_TRY_TO_SCHEDULE_DO_SCHEDULE;

  if ( node->help_state == SCHEDULER_HELP_YOURSELF ) {
    return action;
  }

  owner = _Scheduler_Node_get_owner( node );
  user = _Scheduler_Node_get_user( node );

  if ( node->help_state == SCHEDULER_HELP_ACTIVE_RIVAL) {
    if ( user->Scheduler.state == THREAD_SCHEDULER_READY ) {
      _Scheduler_Thread_set_scheduler_and_node( user, node, owner );
    } else if ( owner->Scheduler.state == THREAD_SCHEDULER_BLOCKED ) {
      if ( idle != NULL ) {
        action = SCHEDULER_TRY_TO_SCHEDULE_DO_IDLE_EXCHANGE;
      } else {
        _Scheduler_Use_idle_thread( context, node, get_idle_thread );
      }
    } else {
      _Scheduler_Node_set_user( node, owner );
    }
  } else if ( node->help_state == SCHEDULER_HELP_ACTIVE_OWNER ) {
    if ( user->Scheduler.state == THREAD_SCHEDULER_READY ) {
      _Scheduler_Thread_set_scheduler_and_node( user, node, owner );
    } else if ( idle != NULL ) {
      action = SCHEDULER_TRY_TO_SCHEDULE_DO_IDLE_EXCHANGE;
    } else {
      _Scheduler_Use_idle_thread( context, node, get_idle_thread );
    }
  } else {
    _Assert( node->help_state == SCHEDULER_HELP_PASSIVE );

    if ( user->Scheduler.state == THREAD_SCHEDULER_READY ) {
      _Scheduler_Thread_set_scheduler_and_node( user, node, owner );
    } else {
      action = SCHEDULER_TRY_TO_SCHEDULE_DO_BLOCK;
    }
  }

  return action;
}

/**
 * @brief Release an idle thread using this scheduler node.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] node The node which may have an idle thread as user.
 * @param[in] release_idle_thread Function to release an idle thread.
 *
 * @retval idle The idle thread which used this node.
 * @retval NULL This node had no idle thread as an user.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Release_idle_thread(
  Scheduler_Context             *context,
  Scheduler_Node                *node,
  Scheduler_Release_idle_thread  release_idle_thread
)
{
  Thread_Control *idle = _Scheduler_Node_get_idle( node );

  if ( idle != NULL ) {
    Thread_Control *owner = _Scheduler_Node_get_owner( node );

    node->idle = NULL;
    _Scheduler_Node_set_user( node, owner );
    _Scheduler_Thread_change_state( idle, THREAD_SCHEDULER_READY );
    _Scheduler_Thread_set_node( idle, idle->Scheduler.own_node );

    ( *release_idle_thread )( context, idle );
  }

  return idle;
}

RTEMS_INLINE_ROUTINE void _Scheduler_Exchange_idle_thread(
  Scheduler_Node *needs_idle,
  Scheduler_Node *uses_idle,
  Thread_Control *idle
)
{
  uses_idle->idle = NULL;
  _Scheduler_Node_set_user(
    uses_idle,
    _Scheduler_Node_get_owner( uses_idle )
  );
  _Scheduler_Set_idle_thread( needs_idle, idle );
}

/**
 * @brief Block this scheduler node.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] thread The thread which wants to get blocked referencing this
 *   node.  This is not necessarily the user of this node in case the node
 *   participates in the scheduler helping protocol.
 * @param[in] node The node which wants to get blocked.
 * @param[in] is_scheduled This node is scheduled.
 * @param[in] get_idle_thread Function to get an idle thread.
 *
 * @retval true Continue with the blocking operation.
 * @retval false Otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Scheduler_Block_node(
  Scheduler_Context         *context,
  Thread_Control            *thread,
  Scheduler_Node            *node,
  bool                       is_scheduled,
  Scheduler_Get_idle_thread  get_idle_thread
)
{
  Thread_Control *old_user;
  Thread_Control *new_user;

  _Scheduler_Thread_change_state( thread, THREAD_SCHEDULER_BLOCKED );

  if ( node->help_state == SCHEDULER_HELP_YOURSELF ) {
    _Assert( thread == _Scheduler_Node_get_user( node ) );

    return true;
  }

  new_user = NULL;

  if ( node->help_state == SCHEDULER_HELP_ACTIVE_OWNER ) {
    if ( is_scheduled ) {
      _Assert( thread == _Scheduler_Node_get_user( node ) );
      old_user = thread;
      new_user = _Scheduler_Use_idle_thread( context, node, get_idle_thread );
    }
  } else if ( node->help_state == SCHEDULER_HELP_ACTIVE_RIVAL ) {
    if ( is_scheduled ) {
      old_user = _Scheduler_Node_get_user( node );

      if ( thread == old_user ) {
        Thread_Control *owner = _Scheduler_Node_get_owner( node );

        if (
          thread != owner
            && owner->Scheduler.state == THREAD_SCHEDULER_READY
        ) {
          new_user = owner;
          _Scheduler_Node_set_user( node, new_user );
        } else {
          new_user = _Scheduler_Use_idle_thread( context, node, get_idle_thread );
        }
      }
    }
  } else {
    /* Not implemented, this is part of the OMIP support path. */
    _Assert(0);
  }

  if ( new_user != NULL ) {
    Per_CPU_Control *cpu = _Thread_Get_CPU( old_user );

    _Scheduler_Thread_change_state( new_user, THREAD_SCHEDULER_SCHEDULED );
    _Thread_Set_CPU( new_user, cpu );
    _Thread_Dispatch_update_heir( _Per_CPU_Get(), cpu, new_user );
  }

  return false;
}

/**
 * @brief Unblock this scheduler node.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] the_thread The thread which wants to get unblocked.
 * @param[in] node The node which wants to get unblocked.
 * @param[in] is_scheduled This node is scheduled.
 * @param[in] release_idle_thread Function to release an idle thread.
 *
 * @retval true Continue with the unblocking operation.
 * @retval false Otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Scheduler_Unblock_node(
  Scheduler_Context             *context,
  Thread_Control                *the_thread,
  Scheduler_Node                *node,
  bool                           is_scheduled,
  Scheduler_Release_idle_thread  release_idle_thread
)
{
  bool unblock;

  if ( is_scheduled ) {
    Thread_Control *old_user = _Scheduler_Node_get_user( node );
    Per_CPU_Control *cpu = _Thread_Get_CPU( old_user );
    Thread_Control *idle = _Scheduler_Release_idle_thread(
      context,
      node,
      release_idle_thread
    );
    Thread_Control *owner = _Scheduler_Node_get_owner( node );
    Thread_Control *new_user;

    if ( node->help_state == SCHEDULER_HELP_ACTIVE_OWNER ) {
      _Assert( idle != NULL );
      new_user = the_thread;
    } else if ( idle != NULL ) {
      _Assert( node->help_state == SCHEDULER_HELP_ACTIVE_RIVAL );
      new_user = the_thread;
    } else if ( the_thread != owner ) {
      _Assert( node->help_state == SCHEDULER_HELP_ACTIVE_RIVAL );
      _Assert( old_user != the_thread );
      _Scheduler_Thread_change_state( owner, THREAD_SCHEDULER_READY );
      new_user = the_thread;
      _Scheduler_Node_set_user( node, new_user );
    } else {
      _Assert( node->help_state == SCHEDULER_HELP_ACTIVE_RIVAL );
      _Assert( old_user != the_thread );
      _Scheduler_Thread_change_state( the_thread, THREAD_SCHEDULER_READY );
      new_user = NULL;
    }

    if ( new_user != NULL ) {
      _Scheduler_Thread_change_state( new_user, THREAD_SCHEDULER_SCHEDULED );
      _Thread_Set_CPU( new_user, cpu );
      _Thread_Dispatch_update_heir( _Per_CPU_Get(), cpu, new_user );
    }

    unblock = false;
  } else {
    _Scheduler_Thread_change_state( the_thread, THREAD_SCHEDULER_READY );

    unblock = true;
  }

  return unblock;
}

/**
 * @brief Asks a ready scheduler node for help.
 *
 * @param[in] node The ready node offering help.
 * @param[in] needs_help The thread needing help.
 *
 * @retval needs_help The thread needing help.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Ask_ready_node_for_help(
  Scheduler_Node *node,
  Thread_Control *needs_help
)
{
  _Scheduler_Node_set_user( node, needs_help );

  return needs_help;
}

/**
 * @brief Asks a scheduled scheduler node for help.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] node The scheduled node offering help.
 * @param[in] offers_help The thread offering help.
 * @param[in] needs_help The thread needing help.
 * @param[in] previous_accepts_help The previous thread accepting help by this
 *   scheduler node.
 * @param[in] release_idle_thread Function to release an idle thread.
 *
 * @retval needs_help The previous thread accepting help by this scheduler node
 *   which was displaced by the thread needing help.
 * @retval NULL There are no more threads needing help.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Ask_scheduled_node_for_help(
  Scheduler_Context             *context,
  Scheduler_Node                *node,
  Thread_Control                *offers_help,
  Thread_Control                *needs_help,
  Thread_Control                *previous_accepts_help,
  Scheduler_Release_idle_thread  release_idle_thread
)
{
  Thread_Control *next_needs_help = NULL;
  Thread_Control *old_user = NULL;
  Thread_Control *new_user = NULL;

  if (
    previous_accepts_help != needs_help
      && _Scheduler_Thread_get_node( previous_accepts_help ) == node
  ) {
    Thread_Control *idle = _Scheduler_Release_idle_thread(
      context,
      node,
      release_idle_thread
    );

    if ( idle != NULL ) {
      old_user = idle;
    } else {
      _Assert( _Scheduler_Node_get_user( node ) == previous_accepts_help );
      old_user = previous_accepts_help;
    }

    if ( needs_help->Scheduler.state == THREAD_SCHEDULER_READY ) {
      new_user = needs_help;
    } else {
      _Assert(
        node->help_state == SCHEDULER_HELP_ACTIVE_OWNER
          || node->help_state == SCHEDULER_HELP_ACTIVE_RIVAL
      );
      _Assert( offers_help->Scheduler.node == offers_help->Scheduler.own_node );

      new_user = offers_help;
    }

    if ( previous_accepts_help != offers_help ) {
      next_needs_help = previous_accepts_help;
    }
  } else if ( needs_help->Scheduler.state == THREAD_SCHEDULER_READY ) {
    Thread_Control *idle = _Scheduler_Release_idle_thread(
      context,
      node,
      release_idle_thread
    );

    if ( idle != NULL ) {
      old_user = idle;
    } else {
      old_user = _Scheduler_Node_get_user( node );
    }

    new_user = needs_help;
  } else {
    _Assert( needs_help->Scheduler.state == THREAD_SCHEDULER_SCHEDULED );
  }

  if ( new_user != old_user ) {
    Per_CPU_Control *cpu_self = _Per_CPU_Get();
    Per_CPU_Control *cpu = _Thread_Get_CPU( old_user );

    _Scheduler_Thread_change_state( old_user, THREAD_SCHEDULER_READY );
    _Scheduler_Thread_set_scheduler_and_node(
      old_user,
      _Scheduler_Thread_get_own_node( old_user ),
      old_user
    );

    _Scheduler_Thread_change_state( new_user, THREAD_SCHEDULER_SCHEDULED );
    _Scheduler_Thread_set_scheduler_and_node( new_user, node, offers_help );

    _Scheduler_Node_set_user( node, new_user );
    _Thread_Set_CPU( new_user, cpu );
    _Thread_Dispatch_update_heir( cpu_self, cpu, new_user );
  }

  return next_needs_help;
}

/**
 * @brief Asks a blocked scheduler node for help.
 *
 * @param[in] context The scheduler instance context.
 * @param[in] node The scheduled node offering help.
 * @param[in] offers_help The thread offering help.
 * @param[in] needs_help The thread needing help.
 *
 * @retval true Enqueue this scheduler node.
 * @retval false Otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Scheduler_Ask_blocked_node_for_help(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Thread_Control    *offers_help,
  Thread_Control    *needs_help
)
{
  bool enqueue;

  _Assert( node->help_state == SCHEDULER_HELP_PASSIVE );

  if ( needs_help->Scheduler.state == THREAD_SCHEDULER_READY ) {
    _Scheduler_Node_set_user( node, needs_help );
    _Scheduler_Thread_set_scheduler_and_node( needs_help, node, offers_help );

    enqueue = true;
  } else {
    enqueue = false;
  }

  return enqueue;
}
#endif

ISR_LOCK_DECLARE( extern, _Scheduler_Lock )

/**
 * @brief Acquires the scheduler instance of the thread.
 *
 * @param[in] the_thread The thread.
 * @param[in] lock_context The lock context for _Scheduler_Release().
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Acquire(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  (void) the_thread;
  _ISR_lock_ISR_disable_and_acquire( &_Scheduler_Lock, lock_context );
}

/**
 * @brief Releases the scheduler instance of the thread.
 *
 * @param[in] the_thread The thread.
 * @param[in] lock_context The lock context used for _Scheduler_Acquire().
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Release(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  (void) the_thread;
  _ISR_lock_Release_and_ISR_enable( &_Scheduler_Lock, lock_context );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
