/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/core/context.h>
#include <rtems/core/interr.h>
#include <rtems/core/intthrd.h>
#include <rtems/core/isr.h>
#include <rtems/core/object.h>
#include <rtems/core/priority.h>
#include <rtems/core/states.h>
#include <rtems/core/thread.h>
#include <rtems/core/threadq.h>
#include <rtems/core/userext.h>
#include <rtems/core/wkspace.h>

/*PAGE
 *
 *  _Thread_Handler_initialization
 *
 *  This routine initializes all thread manager related data structures.
 *
 *  Input parameters:
 *    ticks_per_timeslice - clock ticks per quantum
 *    maximum_proxies     - number of proxies to initialize
 *
 *  Output parameters:  NONE
 */

void _Thread_Handler_initialization(
  unsigned32   ticks_per_timeslice,
  unsigned32   maximum_extensions,
  unsigned32   maximum_proxies
)
{
  unsigned32 index;

  _Context_Switch_necessary = FALSE;
  _Thread_Executing         = NULL;
  _Thread_Heir              = NULL;
  _Thread_Allocated_fp      = NULL;

  _Thread_Maximum_extensions = maximum_extensions;

  _Thread_Ticks_remaining_in_timeslice = ticks_per_timeslice;
  _Thread_Ticks_per_timeslice          = ticks_per_timeslice;

  _Thread_Ready_chain = _Workspace_Allocate_or_fatal_error(
    (PRIORITY_MAXIMUM + 1) * sizeof(Chain_Control)
  );

  for ( index=0; index <= PRIORITY_MAXIMUM ; index++ )
    _Chain_Initialize_empty( &_Thread_Ready_chain[ index ] );

  _Thread_MP_Handler_initialization( maximum_proxies );
}

/*PAGE
 *
 *  _Thread_Start_multitasking
 *
 *  This kernel routine readies the requested thread, the thread chain
 *  is adjusted.  A new heir thread may be selected.
 *
 *  Input parameters:
 *    system_thread - pointer to system initialization thread control block
 *    idle_thread   - pointer to idle thread control block
 *
 *  Output parameters:  NONE
 *
 *  NOTE:  This routine uses the "blocking" heir selection mechanism.
 *         This insures the correct heir after a thread restart.
 *
 *  INTERRUPT LATENCY:
 *    ready chain
 *    select heir
 */

void _Thread_Start_multitasking(
  Thread_Control *system_thread,
  Thread_Control *idle_thread
)
{

   _Thread_Executing  =
   _Thread_Heir       =
   _Thread_MP_Receive = system_thread;

   /*
    *  Scheduling will not work "correctly" until the above
    *  statements have been executed.
    */

   _Thread_Ready( system_thread );
   _Thread_Ready( idle_thread );

   _Context_Switch_necessary = FALSE;

   _Context_Switch( &_Thread_BSP_context, &system_thread->Registers );

}

/*PAGE
 *
 *  _Thread_Dispatch
 *
 *  This kernel routine determines if a dispatch is needed, and if so
 *  dispatches to the heir thread.  Once the heir is running an attempt
 *  is made to dispatch any ASRs.
 *
 *  ALTERNATE ENTRY POINTS:
 *    void _Thread_Enable_dispatch();
 *
 *  Input parameters:  NONE
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    dispatch thread
 *    no dispatch thread
 */

#if ( CPU_INLINE_ENABLE_DISPATCH == FALSE )
void _Thread_Enable_dispatch( void )
{
  if ( --_Thread_Dispatch_disable_level )
    return;
  _Thread_Dispatch();
}
#endif

void _Thread_Dispatch( void )
{
  Thread_Control   *executing;
  Thread_Control   *heir;
  ISR_Level         level;

  executing   = _Thread_Executing;
  _ISR_Disable( level );
  while ( _Context_Switch_necessary == TRUE ) {
    heir = _Thread_Heir;
    _Thread_Dispatch_disable_level = 1;
    _Context_Switch_necessary = FALSE;
    _Thread_Executing = heir;
    _ISR_Enable( level );

    _User_extensions_Thread_switch( executing, heir );

    _Thread_Ticks_remaining_in_timeslice = _Thread_Ticks_per_timeslice;

    /*
     *  If the CPU has hardware floating point, then we must address saving
     *  and restoring it as part of the context switch.
     *
     *  The second conditional compilation section selects the algorithm used
     *  to context switch between floating point tasks.  The deferred algorithm
     *  can be significantly better in a system with few floating point tasks
     *  because it reduces the total number of save and restore FP context
     *  operations.  However, this algorithm can not be used on all CPUs due
     *  to unpredictable use of FP registers by some compilers for integer
     *  operations.
     */

#if ( CPU_HARDWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
    if ( (heir->fp_context != NULL) && !_Thread_Is_allocated_fp( heir ) ) {
      if ( _Thread_Allocated_fp != NULL )
        _Context_Save_fp( &_Thread_Allocated_fp->fp_context );
      _Context_Restore_fp( &heir->fp_context );
      _Thread_Allocated_fp = heir;
    }
#else
    if ( executing->fp_context != NULL )
      _Context_Save_fp( &executing->fp_context );

    if ( heir->fp_context != NULL )
      _Context_Restore_fp( &heir->fp_context );
#endif
#endif

    _Context_Switch( &executing->Registers, &heir->Registers );

    executing = _Thread_Executing;

    _ISR_Disable( level );
  }

  _Thread_Dispatch_disable_level = 0;

  _ISR_Enable( level );

  _User_extensions_Thread_post_switch( executing );
 
}

/*PAGE
 *
 *  _Thread_Initialize
 *
 *  XXX
 */

boolean _Thread_Initialize(
  Objects_Information *information,
  Thread_Control      *the_thread,
  void                *stack_area,    /* NULL if to be allocated */
  unsigned32           stack_size,    /* insure it is >= min */
  boolean              is_fp,         /* TRUE if thread uses FP */
  Priority_Control     priority,
  boolean              is_preemptible,
  boolean              is_timeslice,
  unsigned32           isr_level,
  Objects_Name         name
  
)
{
  unsigned32           actual_stack_size;
  void                *stack;
  void                *fp_area;
  void                *extensions_area;

  /*
   *  Allocate and Initialize the stack for this thread.
   */

  if ( !_Stack_Is_enough( stack_size ) )
    actual_stack_size = STACK_MINIMUM_SIZE;
  else
    actual_stack_size = stack_size;

  actual_stack_size = _Stack_Adjust_size( actual_stack_size );
  stack             = stack_area;

  if ( !stack ) {
    stack = _Workspace_Allocate( actual_stack_size );
 
    if ( !stack ) 
      return FALSE;

    the_thread->Start.stack = stack;
  } else
    the_thread->Start.stack = NULL;

  _Stack_Initialize(
     &the_thread->Start.Initial_stack,
     stack,
     actual_stack_size
  );

  /*
   *  Allocate the floating point area for this thread
   */
  
  if ( is_fp ) {

    fp_area = _Workspace_Allocate( CONTEXT_FP_SIZE );
    if ( !fp_area ) {
      if ( the_thread->Start.stack )
        (void) _Workspace_Free( the_thread->Start.stack );
      return FALSE;
    }
    fp_area = _Context_Fp_start( fp_area, 0 );

  } else
    fp_area = NULL;

  the_thread->fp_context       = fp_area;
  the_thread->Start.fp_context = fp_area;
  

  /*
   *  Allocate the floating point area for this thread
   */

  if ( _Thread_Maximum_extensions ) {
    extensions_area = _Workspace_Allocate(
      (_Thread_Maximum_extensions + 1) * sizeof( void * )
    );

    if ( !extensions_area ) {
      if ( fp_area )
        (void) _Workspace_Free( fp_area );

      if ( the_thread->Start.stack )
        (void) _Workspace_Free( the_thread->Start.stack );

      return FALSE;
    }
  } else 
    extensions_area = NULL;
  
  the_thread->extensions = extensions_area;

  /*
   *  General initialization
   */

  the_thread->Start.is_preemptible = is_preemptible;
  the_thread->Start.is_timeslice   = is_timeslice;
  the_thread->Start.isr_level      = isr_level;

  the_thread->current_state          = STATES_DORMANT;
  the_thread->resource_count         = 0;
  the_thread->real_priority          = priority;
  the_thread->Start.initial_priority = priority;
 
  _Thread_Set_priority( the_thread, priority );

  /*
   *  Open the object
   */

  _Objects_Open( information, &the_thread->Object, name );

  /*
   *  Invoke create extensions
   */

  if ( !_User_extensions_Thread_create( the_thread ) ) {

    if ( extensions_area )
      (void) _Workspace_Free( extensions_area );

    if ( fp_area )
      (void) _Workspace_Free( fp_area );

    if ( the_thread->Start.stack )
      (void) _Workspace_Free( the_thread->Start.stack );

    return FALSE;
  }

  return TRUE;
   
}

/*
 *  _Thread_Start
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
boolean _Thread_Start(
  Thread_Control       *the_thread,
  Thread_Start_types    the_prototype,
  void                 *entry_point,
  void                 *pointer_argument,
  unsigned32            numeric_argument
)
{
  if ( _States_Is_dormant( the_thread->current_state ) ) {
 
    the_thread->Start.entry_point      = entry_point;
   
    the_thread->Start.prototype        = the_prototype;
    the_thread->Start.pointer_argument = pointer_argument;
    the_thread->Start.numeric_argument = numeric_argument;
 
    _Thread_Load_environment( the_thread );
 
    _Thread_Ready( the_thread );
 
    _User_extensions_Thread_start( the_thread );
 
    return TRUE;
  }
 
  return FALSE;
 
}

/*
 *  _Thread_Restart
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
boolean _Thread_Restart(
  Thread_Control      *the_thread,
  void                *pointer_argument,
  unsigned32           numeric_argument
)
{
  if ( !_States_Is_dormant( the_thread->current_state ) ) {
 
    _Thread_Set_transient( the_thread );
    the_thread->resource_count = 0;
    the_thread->is_preemptible = the_thread->Start.is_preemptible;
    the_thread->is_timeslice   = the_thread->Start.is_timeslice;

    the_thread->Start.pointer_argument = pointer_argument;
    the_thread->Start.numeric_argument = numeric_argument;
 
    if ( !_Thread_queue_Extract_with_proxy( the_thread ) ) {
 
      if ( _Watchdog_Is_active( &the_thread->Timer ) )
        (void) _Watchdog_Remove( &the_thread->Timer );
    }

    if ( the_thread->current_priority != the_thread->Start.initial_priority ) {
      the_thread->real_priority = the_thread->Start.initial_priority;
      _Thread_Set_priority( the_thread, the_thread->Start.initial_priority );
    }
 
    _Thread_Load_environment( the_thread );
 
    _Thread_Ready( the_thread );
 
    _User_extensions_Thread_restart( the_thread );
 
    if ( _Thread_Is_executing ( the_thread ) )
      _Thread_Restart_self();
 
    return TRUE;
  }
 
  return FALSE;
}

/*
 *  _Thread_Close
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
void _Thread_Close(
  Objects_Information  *information,
  Thread_Control       *the_thread
)
{
  _Objects_Close( information, &the_thread->Object );
 
  _Thread_Set_state( the_thread, STATES_TRANSIENT );
 
  if ( !_Thread_queue_Extract_with_proxy( the_thread ) ) {
 
    if ( _Watchdog_Is_active( &the_thread->Timer ) )
      (void) _Watchdog_Remove( &the_thread->Timer );
  }

  _User_extensions_Thread_delete( the_thread );
 
#if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
  if ( _Thread_Is_allocated_fp( the_thread ) )
    _Thread_Deallocate_fp();
#endif
  the_thread->fp_context = NULL;

  (void) _Workspace_Free( the_thread->Start.fp_context );

  if ( the_thread->Start.stack )
    (void) _Workspace_Free( the_thread->Start.stack );

  if ( the_thread->extensions )
    (void) _Workspace_Free( the_thread->extensions );
}

/*PAGE
 *
 *  _Thread_Ready
 *
 *  This kernel routine readies the requested thread, the thread chain
 *  is adjusted.  A new heir thread may be selected.
 *
 *  Input parameters:
 *    the_thread - pointer to thread control block
 *
 *  Output parameters:  NONE
 *
 *  NOTE:  This routine uses the "blocking" heir selection mechanism.
 *         This insures the correct heir after a thread restart.
 *
 *  INTERRUPT LATENCY:
 *    ready chain
 *    select heir
 */

void _Thread_Ready(
  Thread_Control *the_thread
)
{
  ISR_Level              level;
  Thread_Control *heir;

  _ISR_Disable( level );

  the_thread->current_state = STATES_READY;

  _Priority_Add_to_bit_map( &the_thread->Priority_map );

  _Chain_Append_unprotected( the_thread->ready, &the_thread->Object.Node );

  _ISR_Flash( level );

  _Thread_Calculate_heir();

  heir = _Thread_Heir;

  if ( !_Thread_Is_executing( heir ) && _Thread_Executing->is_preemptible ) 
    _Context_Switch_necessary = TRUE;

  _ISR_Enable( level );
}

/*PAGE
 *
 *  _Thread_Clear_state
 *
 *  This kernel routine clears the appropriate states in the
 *  requested thread.  The thread ready chain is adjusted if
 *  necessary and the Heir thread is set accordingly.
 *
 *  Input parameters:
 *    the_thread - pointer to thread control block
 *    state      - state set to clear
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    priority map
 *    select heir
 */

void _Thread_Clear_state(
  Thread_Control *the_thread,
  States_Control  state
)
{
  ISR_Level level;

  _ISR_Disable( level );
    the_thread->current_state =
      _States_Clear( state, the_thread->current_state );

    if ( _States_Is_ready( the_thread->current_state ) ) {

      _Priority_Add_to_bit_map( &the_thread->Priority_map );

      _Chain_Append_unprotected( the_thread->ready, &the_thread->Object.Node );

      _ISR_Flash( level );

      if ( the_thread->current_priority < _Thread_Heir->current_priority ) {
        _Thread_Heir = the_thread;
        if ( _Thread_Executing->is_preemptible ||
             the_thread->current_priority == 0 )
          _Context_Switch_necessary = TRUE;
      }
    }
  _ISR_Enable( level );
}

/*PAGE
 *
 * _Thread_Set_state
 *
 * This kernel routine sets the requested state in the THREAD.  The
 * THREAD chain is adjusted if necessary.
 *
 * Input parameters:
 *   the_thread   - pointer to thread control block
 *   state - state to be set
 *
 * Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    ready chain
 *    select map
 */

void _Thread_Set_state(
  Thread_Control *the_thread,
  States_Control         state
)
{
  ISR_Level             level;
  Chain_Control *ready;

  ready = the_thread->ready;
  _ISR_Disable( level );
  if ( !_States_Is_ready( the_thread->current_state ) ) {
    the_thread->current_state =
       _States_Set( state, the_thread->current_state );
    _ISR_Enable( level );
    return;
  }

  the_thread->current_state = state;

  if ( _Chain_Has_only_one_node( ready ) ) {

    _Chain_Initialize_empty( ready );
    _Priority_Remove_from_bit_map( &the_thread->Priority_map );

  } else
    _Chain_Extract_unprotected( &the_thread->Object.Node );

  _ISR_Flash( level );

  if ( _Thread_Is_heir( the_thread ) )
     _Thread_Calculate_heir();

  if ( _Thread_Is_executing( the_thread ) )
    _Context_Switch_necessary = TRUE;

  _ISR_Enable( level );
}

/*PAGE
 *
 *  _Thread_Set_transient
 *
 *  This kernel routine places the requested thread in the transient state
 *  which will remove it from the ready queue, if necessary.  No
 *  rescheduling is necessary because it is assumed that the transient
 *  state will be cleared before dispatching is enabled.
 *
 *  Input parameters:
 *    the_thread - pointer to thread control block
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

void _Thread_Set_transient(
  Thread_Control *the_thread
)
{
  ISR_Level             level;
  unsigned32            old_state;
  Chain_Control *ready;

  ready = the_thread->ready;
  _ISR_Disable( level );

  old_state = the_thread->current_state;
  the_thread->current_state = _States_Set( STATES_TRANSIENT, old_state );

  if ( _States_Is_ready( old_state ) ) {
    if ( _Chain_Has_only_one_node( ready ) ) {

      _Chain_Initialize_empty( ready );
      _Priority_Remove_from_bit_map( &the_thread->Priority_map );

    } else
      _Chain_Extract_unprotected( &the_thread->Object.Node );
  }

  _ISR_Enable( level );

}

/*PAGE
 *
 *  _Thread_Reset_timeslice
 *
 *  This routine will remove the running thread from the ready chain
 *  and place it immediately at the rear of this chain and then the
 *  timeslice counter is reset.  The heir THREAD will be updated if
 *  the running is also the currently the heir.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    ready chain
 *    select heir
 */

void _Thread_Reset_timeslice( void )
{
  ISR_Level              level;
  Thread_Control *executing;
  Chain_Control  *ready;

  executing = _Thread_Executing;
  ready     = executing->ready;
  _ISR_Disable( level );
    if ( _Chain_Has_only_one_node( ready ) ) {
      _Thread_Ticks_remaining_in_timeslice = _Thread_Ticks_per_timeslice;
      _ISR_Enable( level );
      return;
    }
    _Chain_Extract_unprotected( &executing->Object.Node );
    _Chain_Append_unprotected( ready, &executing->Object.Node );

  _ISR_Flash( level );

    if ( _Thread_Is_heir( executing ) )
      _Thread_Heir = (Thread_Control *) ready->first;

    _Context_Switch_necessary = TRUE;

  _ISR_Enable( level );
}

/*PAGE
 *
 *  _Thread_Tickle_timeslice
 *
 *  This scheduler routine determines if timeslicing is enabled
 *  for the currently executing thread and, if so, updates the
 *  timeslice count and checks for timeslice expiration.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _Thread_Tickle_timeslice( void )
{
  if ( !_Thread_Executing->is_timeslice  ||
       !_Thread_Executing->is_preemptible ||
       !_States_Is_ready( _Thread_Executing->current_state ) ) 
    return;

  if ( --_Thread_Ticks_remaining_in_timeslice == 0 ) {
      _Thread_Reset_timeslice();
  }
}

/*PAGE
 *
 *  _Thread_Yield_processor
 *
 *  This kernel routine will remove the running THREAD from the ready chain
 *  and place it immediatly at the rear of this chain.  Reset timeslice
 *  and yield the processor functions both use this routine, therefore if
 *  reset is TRUE and this is the only thread on the chain then the
 *  timeslice counter is reset.  The heir THREAD will be updated if the
 *  running is also the currently the heir.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    ready chain
 *    select heir
 */

void _Thread_Yield_processor( void )
{
  ISR_Level       level;
  Thread_Control *executing;
  Chain_Control  *ready;

  executing = _Thread_Executing;
  ready     = executing->ready;
  _ISR_Disable( level );
    if ( !_Chain_Has_only_one_node( ready ) ) {
      _Chain_Extract_unprotected( &executing->Object.Node );
      _Chain_Append_unprotected( ready, &executing->Object.Node );

      _ISR_Flash( level );

      if ( _Thread_Is_heir( executing ) )
        _Thread_Heir = (Thread_Control *) ready->first;
      _Context_Switch_necessary = TRUE;
    }
    else if ( !_Thread_Is_heir( executing ) )
      _Context_Switch_necessary = TRUE;

  _ISR_Enable( level );
}

/*PAGE
 *
 *  _Thread_Load_environment
 *
 *  Load starting environment for another thread from its start area in the
 *  thread.  Only called from t_restart and t_start.
 *
 *  Input parameters:
 *    the_thread - thread control block pointer
 *
 *  Output parameters:  NONE
 */

void _Thread_Load_environment(
  Thread_Control *the_thread
)
{
  if ( the_thread->Start.fp_context ) {
    the_thread->fp_context = the_thread->Start.fp_context;
    _Context_Initialize_fp( &the_thread->fp_context );
  }

  the_thread->is_preemptible = the_thread->Start.is_preemptible;
  the_thread->is_timeslice   = the_thread->Start.is_timeslice;

  _Context_Initialize(
    &the_thread->Registers,
    the_thread->Start.Initial_stack.area,
    the_thread->Start.Initial_stack.size,
    the_thread->Start.isr_level,
    _Thread_Handler
  );

}

/*PAGE
 *
 *  _Thread_Handler
 *
 *  This routine is the default thread exitted error handler.  It is
 *  returned to when a thread exits.  The configured fatal error handler
 *  is invoked to process the exit.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _Thread_Handler( void )
{
  Thread_Control *executing;

  executing = _Thread_Executing;

  /*
   * Take care that 'begin' extensions get to complete before
   * 'switch' extensions can run.  This means must keep dispatch
   * disabled until all 'begin' extensions complete.
   */
 
  _User_extensions_Thread_begin( executing );
 
  /*
   *  At this point, the dispatch disable level BETTER be 1.
   */

  _Thread_Enable_dispatch();
 
  switch ( executing->Start.prototype ) {
    case THREAD_START_NUMERIC:
      (*executing->Start.entry_point)( executing->Start.numeric_argument );
      break;
    case THREAD_START_POINTER:
      (*executing->Start.entry_point)( executing->Start.pointer_argument );
      break;
    case THREAD_START_BOTH_POINTER_FIRST:
      (*executing->Start.entry_point)( 
        executing->Start.pointer_argument,
        executing->Start.numeric_argument
      );
      break;
    case THREAD_START_BOTH_NUMERIC_FIRST:
      (*executing->Start.entry_point)( 
        executing->Start.numeric_argument,
        executing->Start.pointer_argument
      );
      break;
  }

  _User_extensions_Thread_exitted( executing );

  _Internal_error_Occurred(
    INTERNAL_ERROR_CORE,
    TRUE,
    INTERNAL_ERROR_THREAD_EXITTED
  );
}

/*PAGE
 *
 *  _Thread_Delay_ended
 *
 *  This routine processes a thread whose delay period has ended.
 *  It is called by the watchdog handler.
 *
 *  Input parameters:
 *    id - thread id
 *
 *  Output parameters: NONE
 */

void _Thread_Delay_ended(
  Objects_Id  id,
  void       *ignored
)
{
  Thread_Control    *the_thread;
  Objects_Locations  location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:  /* impossible */
      break;
    case OBJECTS_LOCAL:
      _Thread_Unblock( the_thread );
      _Thread_Unnest_dispatch();
      break;
  }
}

/*PAGE
 *
 *  _Thread_Change_priority
 *
 *  This kernel routine changes the priority of the thread.  The
 *  thread chain is adjusted if necessary.
 *
 *  Input parameters:
 *    the_thread   - pointer to thread control block
 *    new_priority - ultimate priority
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    ready chain
 *    select heir
 */

void _Thread_Change_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority
)
{
  ISR_Level level;

  _Thread_Set_transient( the_thread );

  if ( the_thread->current_priority != new_priority )
    _Thread_Set_priority( the_thread, new_priority );

  _ISR_Disable( level );

  the_thread->current_state =
    _States_Clear( STATES_TRANSIENT, the_thread->current_state );

  if ( ! _States_Is_ready( the_thread->current_state ) ) {
    _ISR_Enable( level );
    return;
  }

  _Priority_Add_to_bit_map( &the_thread->Priority_map );
  _Chain_Append_unprotected( the_thread->ready, &the_thread->Object.Node );

  _ISR_Flash( level );

  _Thread_Calculate_heir();

  if ( !_Thread_Is_executing_also_the_heir() &&
       _Thread_Executing->is_preemptible )
    _Context_Switch_necessary = TRUE;

  _ISR_Enable( level );
}

/*PAGE
 *
 * _Thread_Set_priority
 *
 * This directive enables and disables several modes of
 * execution for the requesting thread.
 *
 *  Input parameters:
 *    the_thread   - pointer to thread priority
 *    new_priority - new priority
 *
 *  Output: NONE
 */

void _Thread_Set_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority
)
{
  the_thread->current_priority = new_priority;
  the_thread->ready            = &_Thread_Ready_chain[ new_priority ];

  _Priority_Initialize_information( &the_thread->Priority_map, new_priority );
}

/*PAGE
 *
 *  _Thread_Evaluate_mode
 *
 *  XXX
 */

boolean _Thread_Evaluate_mode( void )
{
  Thread_Control     *executing;

  executing = _Thread_Executing;

  if ( !_States_Is_ready( executing->current_state ) ||
       ( !_Thread_Is_heir( executing ) && executing->is_preemptible ) ) {
    _Context_Switch_necessary = TRUE;
    return TRUE;
  }

  return FALSE;
}

/*PAGE
 *
 *  _Thread_Get
 *
 *  NOTE:  If we are not using static inlines, this must be a real
 *         subroutine call.
 *
 *  NOTE:  XXX... This routine may be able to be optimized.
 */

#ifndef USE_INLINES

STATIC INLINE Thread_Control *_Thread_Get (
  Objects_Id           id,
  Objects_Locations   *location
)
{
  Objects_Classes      the_class;
  Objects_Information *information;
 
  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ) {
    _Thread_Disable_dispatch();
    *location = OBJECTS_LOCAL;
    return( _Thread_Executing );
  }
 
  the_class = _Objects_Get_class( id );
 
  if ( the_class > OBJECTS_CLASSES_LAST ) {
    *location = OBJECTS_ERROR;
    return (Thread_Control *) 0;
  } 
 
  information = _Objects_Information_table[ the_class ];
 
  if ( !information || !information->is_thread ) { 
    *location = OBJECTS_ERROR;
    return (Thread_Control *) 0;
  }
 
  return (Thread_Control *) _Objects_Get( information, id, location );
}
#endif
