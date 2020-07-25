/**
 * @file
 *
 * @ingroup RTEMSScoreSchedulerStrongAPA
 *
 * @brief Strong APA Scheduler Implementation
 */
 
/*	
 * Copyright (c) 2013, 2016 embedded brains GmbH.  All rights reserved.	
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/schedulerstrongapa.h>
#include <rtems/score/schedulersmpimpl.h>

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
/**
 * @brief Initializes the Strong_APA scheduler.
 *
 * Sets the chain containing all the nodes to empty 
 * and initializes the SMP scheduler.
 *
 * @param scheduler used to get 
 * reference to Strong APA scheduler context 
 * @retval void
 * @see _Scheduler_strong_APA_Node_initialize()
 *
 */  
void _Scheduler_strong_APA_Initialize( const Scheduler_Control *scheduler )
{
  Scheduler_strong_APA_Context *self =
    _Scheduler_strong_APA_Get_context( scheduler );

  _Scheduler_SMP_Initialize( &self->Base );
  _Chain_Initialize_empty( &self->allNodes );
}

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
)
{
  Scheduler_SMP_Node *smp_node;
  
  smp_node = _Scheduler_SMP_Node_downcast( node );
  _Scheduler_SMP_Node_initialize( scheduler, smp_node, the_thread, priority );
}

/**
 * @brief Helper function for /a update entry function
 * 
 * Calls _Scheduler_SMP_Node_update_priority()
 *
 * @param scheduler The scheduler context.
 * @param[out] node The node to update the priority of.
 * @param new_priority Node's new priority.
 * @see _Scheduler_strong_APA_Do_update()
 */
void _Scheduler_strong_APA_Do_update(
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

/**
 * @brief Checks if scheduler has a ready node
 * 
 * Iterates through all the nodes in /a allNodes to 
 * look for a ready node
 *
 * @param scheduler The scheduler context.
 * @retval true if scheduler has a ready node available
 * @retval false if scheduler has no ready nodes available
 */

bool _Scheduler_strong_APA_Has_ready( Scheduler_Context *context )
{
  Scheduler_strong_APA_Context *self = _Scheduler_strong_APA_Get_self( context );
	
  bool 			ret;
  const Chain_Node      *tail;
  Chain_Node 		*next;
  
  tail = _Chain_Immutable_tail( &self->allNodes );
  next = _Chain_First( &self->allNodes );
  
  ret=false;
  
  while ( next != tail ) {
     Scheduler_strong_APA_Node *node;
       
     node = (Scheduler_strong_APA_Node *) next;
     
     if( _Scheduler_SMP_Node_state( &node->Base.Base ) 
     == SCHEDULER_SMP_NODE_READY ) {
       ret=true;
       break;
     }
  }
  
  return ret;
}

/**
 * @brief Checks the next highest node ready on run
 * on the CPU on which @filter node was running on 
 *
 */
Scheduler_Node *_Scheduler_strong_APA_Get_highest_ready(
  Scheduler_Context *context,
  Scheduler_Node    *filter
) //TODO
{
  //Plan for this function: (Pseudo Code):
  Scheduler_strong_APA_Context *self=_Scheduler_strong_APA_Get_self( context );
	
  CPU			*Qcpu;
  Thread_Control 	*thread;
  Per_CPU_Control 	*thread_cpu;
  Per_CPU_Control 	*curr_CPU;
  Per_CPU_Control	*assigned_cpu;
  Scheduler_Node	*ret;
  Priority_Control	max_priority;
  Priority_Control	curr_priority;
  Chain_Control 	Queue;
  bool			visited[10];	//Temporary Compilation Fix
  
  
  
  thread = filter->user;	
  thread_cpu = _Thread_Get_CPU( thread );
	
  //Implement the BFS Algorithm for task departure
  //to get the highest ready task for a particular CPU
  
  
  max_priority = _Scheduler_Node_get_priority( filter );
  max_priority = SCHEDULER_PRIORITY_PURIFY( max_priority );

  ret=filter;

  const Chain_Node          *tail;
  Chain_Node                *next;
  
  _Chain_Initialize_empty(&Queue);
  
  Qcpu = malloc( sizeof(CPU) );
  assert (Qcpu != NULL);  //Should it be NULL?
  Qcpu->cpu=thread_cpu;
  
  _Chain_Initialize_node( &Qcpu->node );
  _Chain_Append_unprotected( &Queue, &Qcpu->node ); //Insert thread_CPU in the Queue 
  visited[ _Per_CPU_Get_index( thread_cpu ) ]=true;	
  
  
   while( !_Chain_Is_empty( &Queue) ) {
     Qcpu = (CPU*) _Chain_Get_first_unprotected( &Queue );
     curr_CPU = Qcpu->cpu;
     tail = _Chain_Immutable_tail( &self->allNodes );
     next = _Chain_First( &self->allNodes );
  
     while ( next != tail ) {
       Scheduler_strong_APA_Node *node;
       node = (Scheduler_strong_APA_Node *) next;
    
       if( node->affinity & (1 << _Per_CPU_Get_index( curr_CPU ) ) ) {
       //Checks if the thread_CPU is in the affinity set of the node
           
         if(_Scheduler_SMP_Node_state( &node->Base.Base ) 
            == SCHEDULER_SMP_NODE_SCHEDULED) {
             
            assigned_cpu = _Thread_Get_CPU( node->Base.Base.user );
            
            if(visited[ _Per_CPU_Get_index( assigned_cpu ) ] == false) {
              Qcpu = malloc( sizeof(CPU) );
	      assert (Qcpu != NULL); //Should it be NULL?
	      Qcpu->cpu=*assigned_cpu;
	      
	      _Chain_Initialize_node( &Qcpu->node );
	      _Chain_Append_unprotected( &Queue, &Qcpu->node ); 
	      //Insert thread_CPU in the Queue 
	      visited[ _Per_CPU_Get_index (assigned_cpu) ]=true;
            } 
          }
          else if(_Scheduler_SMP_Node_state( &node->Base.Base ) 
           == SCHEDULER_SMP_NODE_READY) {
            curr_priority = _Scheduler_Node_get_priority( (Scheduler_Node *) next );
  	    curr_priority = SCHEDULER_PRIORITY_PURIFY( curr_priority );
  		
            if(curr_priority<max_priority) {
              max_priority=curr_priority;
  	      ret=&node->Base.Base;
  	    }
          }
    	}
       next = _Chain_Next( next );
     }
  }
  
  if( ret != filter)
  {
    //Backtrack on the path from
    //thread_cpu to ret, shifting along every task.

    //After this, thread_cpu receives the ret task
    // So the ready task ret gets scheduled as well.  
  }
  
  return ret; 
}
/**
 * @brief Checks the lowest scheduled node 
 * running on a processor on which the 
 * @filter_base node could be running on
 *
 * @param context The scheduler context instance.
 * @param filter_base The node which wants to get scheduled.
 *
 * @retval node The lowest scheduled node that can be 
 * replaced by @filter_base
 */
Scheduler_Node *_Scheduler_strong_APA_Get_lowest_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *filter_base
)
{	
  //Idea: BFS Algorithm for task arrival
	
  uint32_t	cpu_max;
  uint32_t	cpu_index;
  CPU		*Qcpu;
  
  Per_CPU_Control	*curr_CPU;
  Thread_Control	*curr_thread;
  Scheduler_Node    	*curr_node;
  Scheduler_Node    	*ret;
  Chain_Control		Queue;
  Priority_Control	max_priority;
  Priority_Control	curr_priority;
  bool			visited[10];	//Temporary Compilation Fix
  
  Scheduler_strong_APA_Node	*Scurr_node; //Current Strong_APA_Node
  Scheduler_strong_APA_Node	*filter_node;
       
  filter_node = _Scheduler_strong_APA_Node_downcast( filter_base );
  
  max_priority = 300;//Max (Lowest) priority encountered so far.
  
  cpu_max = _SMP_Get_processor_maximum();
  _Chain_Initialize_empty(&Queue);
  
  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) { 
    if( ( filter_node->affinity & (1<<cpu_index) && visited[ cpu_index ] == false ) ) { 
          //Checks if the thread_CPU is in the affinity set of the node
      Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );
      if( _Per_CPU_Is_processor_online( cpu ) ) {
        Qcpu = malloc( sizeof(CPU) );
        assert (Qcpu != NULL);	//Should it be NULL?
        Qcpu->cpu=*cpu;
  
        _Chain_Initialize_node( &Qcpu->node );
        _Chain_Append_unprotected( &Queue, &Qcpu->node );
         //Insert cpu in the Queue
        visited[ cpu_index ]=true;
      }
    }
  }
  
  while( !_Chain_Is_empty( &Queue) ) {
    Qcpu = (CPU*) _Chain_Get_first_unprotected( &Queue );
    curr_CPU = &Qcpu->cpu;
    curr_thread = curr_CPU->executing;

    curr_node = (Scheduler_Node *) _Chain_First( &curr_thread->Scheduler.Scheduler_nodes );
    
    //How to check if the thread is not participating
    //in helping on this processor?
  
    curr_priority = _Scheduler_Node_get_priority( curr_node );
    curr_priority = SCHEDULER_PRIORITY_PURIFY( curr_priority );   
      
    if(curr_priority < max_priority) {
      ret = curr_node;
      max_priority = curr_priority;
    }
    
    Scurr_node = _Scheduler_strong_APA_Node_downcast( curr_node ); 
    if( !curr_thread->is_idle ) {
      for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
        if( ( Scurr_node->affinity & (1<<cpu_index) ) ) { 
          //Checks if the thread_CPU is in the affinity set of the node
          Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );
          if( _Per_CPU_Is_processor_online( cpu ) && visited[ cpu_index ] == false ) {
          
            Qcpu = malloc( sizeof(CPU) );
	    assert (Qcpu != NULL);  //Should it be NULL?
	    Qcpu->cpu=*cpu;
	      
	    _Chain_Initialize_node( &Qcpu->node );
	    _Chain_Append_unprotected( &Queue, &Qcpu->node ); 
	    //Insert the cpu in the affinty set of curr_thread in the Queue
            visited[ cpu_index ]=true;
          }
        }  
      }
    }
  }
  
  Priority_Control  filter_priority;
  filter_priority = _Scheduler_Node_get_priority( filter_base );
  filter_priority = SCHEDULER_PRIORITY_PURIFY( filter_priority );   
 
  if( ret->Priority.value < filter_priority ) {
    //Lowest priority task found has higher priority 
    // than filter_base.
    //So, filter_base remains unassigned
    //No task shifting. 
  }  
 
  else {
    //Backtrack on the path from
    //_Thread_Get_CPU(ret->user) to ret, shifting along every task
  }

  return ret;
}

/**
 * @brief Extracts a node from the scheduled node's list
 *
 * Calls _Scheduler_SMP_Extract_from_scheduled()
 *
 * @param scheduler The scheduler context
 * @param[in] node_to_extract The node to extract.
 */
void _Scheduler_strong_APA_Extract_from_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
)
{
  Scheduler_strong_APA_Context     	*self;
  Scheduler_strong_APA_Node        	*node;

  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( node_to_extract );

  _Scheduler_SMP_Extract_from_scheduled( &self->Base.Base, &node->Base.Base );
  //Not removing it from allNodes since the node could go in the ready state.
}

/**
 * @brief Extracts a node from the ready queue
 *
 * Removes the node from /a allNodes chain.
 *
 * @param scheduler The scheduler context
 * @param[in] node_to_extract The node to extract.
 */
void _Scheduler_strong_APA_Extract_from_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_to_extract
)
{
  Scheduler_strong_APA_Context     *self;
  Scheduler_strong_APA_Node        *node;

  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( node_to_extract );
 
  _Assert( _Chain_Is_empty(self->allNodes) == false );
  _Assert( _Chain_Is_node_off_chain( &node->Node ) == false );
   
   _Chain_Extract_unprotected( &node->Node );	//Removed from allNodes
   _Chain_Set_off_chain( &node->Node );
}

/**
 * @brief Moves a node from scheduled to ready state 
 *
 * Calls _Scheduler_SMP_Extract_from_scheduled() to remove it from
 * scheduled nodes list
 *
 * @param scheduler The scheduler context
 * @param[in] scheduled_to_ready The node to move.
 * @see _Scheduler_strong_APA_Insert_ready()
 *
 */
void _Scheduler_strong_APA_Move_from_scheduled_to_ready(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_to_ready
)
{
  Priority_Control insert_priority;

  _Scheduler_SMP_Extract_from_scheduled( context, scheduled_to_ready );
  insert_priority = _Scheduler_SMP_Node_priority( scheduled_to_ready );
  
  _Scheduler_strong_APA_Insert_ready(
    context,
    scheduled_to_ready,
    insert_priority
  );
}

/**
 * @brief Moves a node from ready to scheduled state.
 *
 * Calls the corresponding SMP function \a _Scheduler_SMP_Insert_scheduled()
 * with append insert_priority
 *
 * @param context The scheduler context.
 * @param ready_to_scheduled Node which moves from ready state.
 */ 
void  _Scheduler_strong_APA_Move_from_ready_to_scheduled(
  Scheduler_Context *context,
  Scheduler_Node    *ready_to_scheduled
)
{
  Priority_Control insert_priority;

  _Scheduler_strong_APA_Extract_from_ready( context, ready_to_scheduled );
  insert_priority = _Scheduler_SMP_Node_priority( ready_to_scheduled );
  insert_priority = SCHEDULER_PRIORITY_APPEND( insert_priority );
  _Scheduler_SMP_Insert_scheduled(
    context,
    ready_to_scheduled,
    insert_priority
  );
  //Note: The node still stays in the allNodes chain
}

/**
 * @brief Inserts a node in the ready queue.
 *
 * Adds it into the /a allNodes chain.
 *
 * @param context The scheduler context.
 * @param node_base Node which is inserted into the queue.
 * @param insert_priority priority at which the node is inserted
 */ 

void _Scheduler_strong_APA_Insert_ready(
  Scheduler_Context *context,
  Scheduler_Node    *node_base,
  Priority_Control   insert_priority
)
{
  Scheduler_strong_APA_Context     *self;
  Scheduler_strong_APA_Node        *node;

  self = _Scheduler_strong_APA_Get_self( context );
  node = _Scheduler_strong_APA_Node_downcast( node_base );
  
  _Assert( _Chain_Is_node_off_chain( &node->Node ) == true );
     
  _Chain_Append_unprotected( &self->allNodes, &node->Node );
}

/**
 * @brief Allocates a processor for the node.
 *
 * Calls _Scheduler_SMP_Allocate_processor_exact()
 *
 * @param context The scheduler context.
 * @param scheduled_base Node which is to be allocated the @victim_cpu.
 * @param victim_base Node which was executing earlier on @victim_cpu
 * @victim_cpu CPU on which the @scheduled_base would be allocated on
 */ 
void _Scheduler_strong_APA_Allocate_processor(
  Scheduler_Context *context,
  Scheduler_Node    *scheduled_base,
  Scheduler_Node    *victim_base,
  Per_CPU_Control   *victim_cpu
)
{
  Scheduler_strong_APA_Node        *scheduled;
 
  (void) victim_base;
  scheduled = _Scheduler_strong_APA_Node_downcast( scheduled_base );

  _Scheduler_SMP_Allocate_processor_exact(
    context,
    &(scheduled->Base.Base),
    NULL,
    victim_cpu
  );
}

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
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );
//The extract from ready automatically removes the node from allNodes chain.
  _Scheduler_SMP_Block(
    context,
    thread,
    node,
    _Scheduler_strong_APA_Extract_from_scheduled,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_strong_APA_Allocate_processor
  );
}

/**
 * @brief Enqueues a node
 *
 * 
 * @param context The scheduler context.
 * @param node node which is to be enqueued
 * @param insert_priority priority at which the node should be enqueued.
 */ 
bool _Scheduler_strong_APA_Enqueue(
  Scheduler_Context *context,
  Scheduler_Node    *node,
  Priority_Control   insert_priority
)
{//I'm hoping all this works on its own.
  return _Scheduler_SMP_Enqueue(
    context,
    node,
    insert_priority,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_strong_APA_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_strong_APA_Move_from_scheduled_to_ready,
    _Scheduler_strong_APA_Get_lowest_scheduled,
    _Scheduler_strong_APA_Allocate_processor
  );
}

/**
 * @brief Enqueues a node in the scheduled queue
 *
 * 
 * @param context The scheduler context.
 * @param node node which is to be enqueued
 * @param insert_priority priority at which the node should be enqueued.
 */ 
bool _Scheduler_strong_APA_Enqueue_scheduled(
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
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_strong_APA_Allocate_processor
  );
}

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
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Unblock(
    context,
    thread,
    node,
    _Scheduler_strong_APA_Do_update,
    _Scheduler_strong_APA_Enqueue
  );
}

/**
 * @brief Calls the smp Ask_for_help
 *
 *
 * @param scheduler The scheduler control instance.
 * @param thread Thread correspoding to the @node that asks for help.
 * @param node node associated with @thread
 */ 
bool _Scheduler_strong_APA_Ask_for_help(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_SMP_Ask_for_help(
    context,
    the_thread,
    node,
    _Scheduler_SMP_Priority_less_equal,
    _Scheduler_strong_APA_Insert_ready,
    _Scheduler_SMP_Insert_scheduled,
    _Scheduler_strong_APA_Move_from_scheduled_to_ready,
    _Scheduler_strong_APA_Get_lowest_scheduled,
    _Scheduler_strong_APA_Allocate_processor
  );
}

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
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Update_priority(
    context,
    thread,
    node,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Do_update,
    _Scheduler_strong_APA_Enqueue,
    _Scheduler_strong_APA_Enqueue_scheduled,
    _Scheduler_strong_APA_Ask_for_help
  );
}
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
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  _Scheduler_SMP_Withdraw_node(
    context,
    the_thread,
    node,
    next_state,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Get_highest_ready,
    _Scheduler_strong_APA_Move_from_ready_to_scheduled,
    _Scheduler_strong_APA_Allocate_processor
  );
}

/**
 * @brief To register an idle thread on a cpu
 *
 * For compatibility with SMP functions,
 * does nothing in our implementation
 */ 
void _Scheduler_strong_APA_Register_idle(
  Scheduler_Context *context,
  Scheduler_Node    *idle_base,
  Per_CPU_Control   *cpu
)
{
  (void) context;
  (void) idle_base;
  (void) cpu;
  //We do not maintain a variable to access the scheduled
  //node for a CPU. So this function does nothing.
}

/**
 * @brief Adds a processor to the scheduler instance
 *
 * and allocates an idle thread to the processor.
 *
 * @param scheduler The scheduler control instance.
 * @param idle Idle thread to be allocated to the processor
 *
 */ 
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


/**
 * @brief Removes a processor from the scheduler instance
 *
 * @param scheduler The scheduler control instance.
 * @param cpu processor that is removed
 *
 */ 
Thread_Control *_Scheduler_strong_APA_Remove_processor(
  const Scheduler_Control *scheduler,
  Per_CPU_Control         *cpu
)
{
  Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Scheduler_SMP_Remove_processor(
    context,
    cpu,
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Enqueue
  );
}


/**
 * @brief Called when a node yields the processor
 *
 * @param scheduler The scheduler control instance.
 * @param thread Thread corresponding to @node
 * @param node Node that yield the processor
 *
 */ 
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
    _Scheduler_strong_APA_Extract_from_ready,
    _Scheduler_strong_APA_Enqueue,
    _Scheduler_strong_APA_Enqueue_scheduled
  );
}

/**
 * @brief Called by _Scheduler_strong_APA_Set_affinity()
 *
 */ 
void  _Scheduler_strong_APA_Do_set_affinity(
  Scheduler_Context *context,
  Scheduler_Node    *node_base,
  void              *arg
)
{
  Scheduler_strong_APA_Node *node;
  const Processor_mask       *affinity;

  node = _Scheduler_strong_APA_Node_downcast( node_base );
  affinity = arg;
  node->affinity = *affinity;
}

/**
 * @brief Starts an idle thread on a CPU
 *
 * @param scheduler The scheduler control instance.
 * @param idle Idle Thread
 * @param cpu processor that gets the idle thread
 *
 */ 
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

/**
 * @brief Pins a node to a cpu
 *
 * @param scheduler The scheduler control instance.
 * @param thread Thread corresponding to @node
 * @param node_base node which gets pinned
 * @param cpu processor that the node gets pinned to
 *
 */ 
void _Scheduler_strong_APA_Pin(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  struct Per_CPU_Control  *cpu
)
{
  Scheduler_strong_APA_Node 	*node;
  uint32_t                 	pin_cpu;

  (void) scheduler;
  node = _Scheduler_strong_APA_Node_downcast( node_base );
  pin_cpu = (uint32_t) _Per_CPU_Get_index( cpu );

  _Assert(
    _Scheduler_SMP_Node_state( &node->Base.Base ) == SCHEDULER_SMP_NODE_BLOCKED
  );

  node = _Scheduler_strong_APA_Node_downcast( node_base );
  
  _Processor_mask_Zero( &node->affinity );
  _Processor_mask_Set( &node->affinity, pin_cpu );
}

/**
 * @brief Unpins a node
 *
 * and sets it affinity back to normal.
 *
 * @param scheduler The scheduler control instance.
 * @param thread Thread corresponding to @node
 * @param node_base node which gets unpinned
 * @param cpu processor that the node gets unpinned from : UNUSED
 *
 */ 
void _Scheduler_strong_APA_Unpin(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  struct Per_CPU_Control  *cpu
)
{
  Scheduler_strong_APA_Node *node;

  (void) scheduler;
  (void) cpu;
  node = _Scheduler_strong_APA_Node_downcast( node_base );

  _Assert(
    _Scheduler_SMP_Node_state( &node->Base.Base ) == SCHEDULER_SMP_NODE_BLOCKED
  );

  _Processor_mask_Zero( &node->affinity );
  _Processor_mask_Assign( &node->affinity, &node->unpin_affinity );
}

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
 * @see _Scheduler_strong_APA_Do_set_affinity()
 *
 * @retval true The processor set of the scheduler is a subset of the affinity set.
 * @retval false The processor set of the scheduler is not a subset of the affinity set.
 */
bool _Scheduler_strong_APA_Set_affinity(
  const Scheduler_Control *scheduler,
  Thread_Control          *thread,
  Scheduler_Node          *node_base,
  const Processor_mask    *affinity
)
{
  Scheduler_Context     	*context;
  Scheduler_strong_APA_Node 	*node;
  Processor_mask          	local_affinity;
 
  context = _Scheduler_Get_context( scheduler );
  _Processor_mask_And( &local_affinity, &context->Processors, affinity );

  if ( _Processor_mask_Is_zero( &local_affinity ) ) {
    return false;
  }

  node = _Scheduler_strong_APA_Node_downcast( node_base );

  if ( _Processor_mask_Is_equal( &node->affinity, affinity ) )
    return true;	//Nothing to do. Return true.
    
 _Processor_mask_Zero( &node->affinity );
 _Processor_mask_Zero( &node->unpin_affinity ); 
 
 _Processor_mask_Assign( &node->affinity, &local_affinity );
 _Processor_mask_Assign( &node->unpin_affinity, &local_affinity );

 _Scheduler_SMP_Set_affinity(
   context,
   thread,
   node_base,
   &local_affinity,
   _Scheduler_strong_APA_Do_set_affinity,
   _Scheduler_strong_APA_Extract_from_ready,		
   _Scheduler_strong_APA_Get_highest_ready,	
   _Scheduler_strong_APA_Move_from_ready_to_scheduled,
   _Scheduler_strong_APA_Enqueue,
   _Scheduler_strong_APA_Allocate_processor
 );

  return true;
}
