/*
 *  RTEMS Task Manager
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
#include <rtems/modes.h>
#include <rtems/object.h>
#include <rtems/stack.h>
#include <rtems/states.h>
#include <rtems/tasks.h>
#include <rtems/thread.h>
#include <rtems/threadq.h>
#include <rtems/tod.h>
#include <rtems/userext.h>
#include <rtems/wkspace.h>

/*PAGE
 *
 *  rtems_task_create
 *
 *  This directive creates a thread by allocating and initializing a
 *  thread control block and a stack.  The newly created thread is
 *  placed in the dormant state.
 *
 *  Input parameters:
 *    name             - user defined thread name
 *    initial_priority - thread priority
 *    stack_size       - stack size in bytes
 *    initial_modes    - initial thread mode
 *    attribute_set    - thread attributes
 *    id               - pointer to thread id
 *
 *  Output parameters:
 *    id                - thread id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_task_create(
  Objects_Name         name,
  rtems_task_priority  initial_priority,
  unsigned32           stack_size,
  rtems_mode           initial_modes,
  rtems_attribute      attribute_set,
  Objects_Id          *id
)
{
  register Thread_Control *the_thread;
  unsigned32               actual_stack_size;
  unsigned32               memory_needed;
  void                    *memory;
  rtems_attribute          the_attribute_set;

  if ( !_Objects_Is_name_valid( name ) )
    return ( RTEMS_INVALID_NAME );

#if 0
  if ( !_Stack_Is_enough( stack_size ) )
    return( RTEMS_INVALID_SIZE );
#endif

  if ( !_Stack_Is_enough( stack_size ) )
    actual_stack_size = RTEMS_MINIMUM_STACK_SIZE;
  else
    actual_stack_size = stack_size;

  if ( !_Priority_Is_valid( initial_priority ) )
    return( RTEMS_INVALID_PRIORITY );

  /*
   *  Fix the attribute set to match the attributes which
   *  this processor (1) requires and (2) is able to support.
   *  First add in the required flags for attribute_set
   *  Typically this might include FP if the platform
   *  or application required all tasks to be fp aware.
   *  Then turn off the requested bits which are not supported.
   */

  the_attribute_set = _Attributes_Set( attribute_set, ATTRIBUTES_REQUIRED );
  the_attribute_set =
    _Attributes_Clear( the_attribute_set, ATTRIBUTES_NOT_SUPPORTED );

  if ( _Attributes_Is_global( the_attribute_set ) &&
       !_Configuration_Is_multiprocessing() )
    return( RTEMS_MP_NOT_CONFIGURED );

  _Thread_Disable_dispatch();          /* to prevent deletion */

  the_thread = _RTEMS_tasks_Allocate();

  if ( !the_thread ) {
    _Thread_Enable_dispatch();
    return( RTEMS_TOO_MANY );
  }

  actual_stack_size = _Stack_Adjust_size( actual_stack_size );
  memory_needed = actual_stack_size;

  if ( _Attributes_Is_floating_point( the_attribute_set ) )
    memory_needed += CONTEXT_FP_SIZE;

  memory = _Workspace_Allocate( memory_needed );

  if ( !memory ) {
    _RTEMS_tasks_Free( the_thread );
    _Thread_Enable_dispatch();
    return( RTEMS_UNSATISFIED );
  }

  /*
   *  Stack is put in the lower address regions of the allocated memory.
   *  The optional floating point context area goes into the higher part
   *  of the allocated memory.
   */

  _Stack_Initialize(
     &the_thread->Start.Initial_stack, memory, actual_stack_size );

  if ( _Attributes_Is_floating_point( the_attribute_set ) )
    the_thread->fp_context = _Context_Fp_start( memory, actual_stack_size );
  else
    the_thread->fp_context = NULL;

  the_thread->Start.fp_context = the_thread->fp_context;

  if ( _Attributes_Is_global( the_attribute_set ) &&
       !( _Objects_MP_Open( &_Thread_Information, name,
                            the_thread->Object.id, FALSE ) ) ) {
    _RTEMS_tasks_Free( the_thread );
    (void) _Workspace_Free( memory );
    _Thread_Enable_dispatch();
    return( RTEMS_TOO_MANY );
  }

  the_thread->name                   = name;
  the_thread->attribute_set          = the_attribute_set;
  the_thread->current_state          = STATES_DORMANT;
  the_thread->current_modes          = initial_modes;
  the_thread->pending_events         = EVENT_SETS_NONE_PENDING;
  the_thread->resource_count         = 0;
  the_thread->real_priority          = initial_priority;
  the_thread->Start.initial_priority = initial_priority;
  the_thread->Start.initial_modes    = initial_modes;

  _Thread_Set_priority( the_thread, initial_priority );

  _ASR_Initialize( &the_thread->Signal );

  _Objects_Open( &_Thread_Information, &the_thread->Object, name );

  *id = the_thread->Object.id;

  _User_extensions_Task_create( the_thread );

  if ( _Attributes_Is_global( the_attribute_set ) )
    _RTEMS_tasks_MP_Send_process_packet(
      RTEMS_TASKS_MP_ANNOUNCE_CREATE,
      the_thread->Object.id,
      name
    );

  _Thread_Enable_dispatch();
  return( RTEMS_SUCCESSFUL );
}

/*PAGE
 *
 *  rtems_task_ident
 *
 *  This directive returns the system ID associated with
 *  the thread name.
 *
 *  Input parameters:
 *    name - user defined thread name
 *    node - node(s) to be searched
 *    id   - pointer to thread id
 *
 *  Output parameters:
 *    *id               - thread id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_task_ident(
  Objects_Name  name,
  unsigned32    node,
  Objects_Id   *id
)
{
  if ( name != OBJECTS_ID_OF_SELF )
    return( _Objects_Name_to_id( &_Thread_Information, name, node, id ) );

  *id = _Thread_Executing->Object.id;
  return( RTEMS_SUCCESSFUL );
}

/*PAGE
 *
 *  rtems_task_start
 *
 *  This directive readies the thread identified by the "id"
 *  based on its current priorty, to await execution.  A thread
 *  can be started only from the dormant state.
 *
 *  Input parameters:
 *    id          - thread id
 *    entry_point - start execution address of thread
 *    argument    - thread argument
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_task_start(
  Objects_Id   id,
  rtems_task_entry entry_point,
  unsigned32   argument
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;

  if ( entry_point == NULL )
    return( RTEMS_INVALID_ADDRESS );

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return( RTEMS_ILLEGAL_ON_REMOTE_OBJECT );
    case OBJECTS_LOCAL:
      if ( _States_Is_dormant( the_thread->current_state ) ) {

        the_thread->Start.entry_point      = entry_point;
        the_thread->Start.initial_argument = argument;

        _Thread_Load_environment( the_thread );

        _Thread_Ready( the_thread );

        _User_extensions_Task_start( the_thread );

        _Thread_Enable_dispatch();
        return( RTEMS_SUCCESSFUL );
      }
      _Thread_Enable_dispatch();
      return( RTEMS_INCORRECT_STATE );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_task_restart
 *
 *  This directive readies the specified thread.  It restores
 *  the thread environment to the original values established
 *  at thread creation and start time.  A thread can be restarted
 *  from any state except the dormant state.
 *
 *  Input parameters:
 *    id       - thread id
 *    argument - thread argument
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_task_restart(
  Objects_Id id,
  unsigned32 argument
)
{
  register Thread_Control *the_thread;
  Objects_Locations               location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return( RTEMS_ILLEGAL_ON_REMOTE_OBJECT );
    case OBJECTS_LOCAL:
      if ( !_States_Is_dormant( the_thread->current_state ) ) {

        _Thread_Set_transient( the_thread );
        _ASR_Initialize( &the_thread->Signal );
        the_thread->pending_events = EVENT_SETS_NONE_PENDING;
        the_thread->resource_count = 0;
        the_thread->current_modes  = the_thread->Start.initial_modes;
        the_thread->Start.initial_argument = argument;

        _RTEMS_tasks_Cancel_wait( the_thread );

        if ( the_thread->current_priority !=
                         the_thread->Start.initial_priority ) {
          the_thread->real_priority = the_thread->Start.initial_priority;
          _Thread_Set_priority( the_thread,
             the_thread->Start.initial_priority );
        }

        _Thread_Load_environment( the_thread );

        _Thread_Ready( the_thread );

        _User_extensions_Task_restart( the_thread );

        if ( _Thread_Is_executing ( the_thread ) )
          _Thread_Restart_self();

        _Thread_Enable_dispatch();
        return( RTEMS_SUCCESSFUL );
      }
      _Thread_Enable_dispatch();
      return( RTEMS_INCORRECT_STATE );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_task_delete
 *
 *  This directive allows a thread to delete itself or the thread
 *  identified in the id field.  The executive halts execution
 *  of the thread and frees the thread control block.
 *
 *  Input parameters:
 *    id - thread id
 *
 *  Output parameters:
 *    nothing           - if id is the requesting thread (always succeeds)
 *    RTEMS_SUCCESSFUL - if successful and id is
 *                           not the requesting thread
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_task_delete(
  Objects_Id id
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return( RTEMS_ILLEGAL_ON_REMOTE_OBJECT );
    case OBJECTS_LOCAL:
      _Objects_Close( &_Thread_Information, &the_thread->Object );

      _Thread_Set_state( the_thread, STATES_TRANSIENT );

      _User_extensions_Task_delete( the_thread );

#if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
      if ( _Thread_Is_allocated_fp( the_thread ) )
        _Thread_Deallocate_fp();
#endif
      the_thread->fp_context = NULL;

      _RTEMS_tasks_Cancel_wait( the_thread );

      (void) _Workspace_Free( the_thread->Start.Initial_stack.area );

      _RTEMS_tasks_Free( the_thread );

      if ( _Attributes_Is_global( the_thread->attribute_set ) ) {

        _Objects_MP_Close( &_Thread_Information, the_thread->Object.id );

        _RTEMS_tasks_MP_Send_process_packet(
          RTEMS_TASKS_MP_ANNOUNCE_DELETE,
          the_thread->Object.id,
          0                                /* Not used */
        );
      }

      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_task_suspend
 *
 *  This directive will place the specified thread in the "suspended"
 *  state.  Note that the suspended state can be in addition to
 *  other waiting states.
 *
 *  Input parameters:
 *    id - thread id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_task_suspend(
  Objects_Id id
)
{
  register Thread_Control *the_thread;
  Objects_Locations               location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      return(
        _RTEMS_tasks_MP_Send_request_packet(
          RTEMS_TASKS_MP_SUSPEND_REQUEST,
          id,
          0,          /* Not used */
          0,          /* Not used */
          0           /* Not used */
        )
      );
    case OBJECTS_LOCAL:
      if ( !_States_Is_suspended( the_thread->current_state ) ) {
        _Thread_Set_state( the_thread, STATES_SUSPENDED );
        _Thread_Enable_dispatch();
        return( RTEMS_SUCCESSFUL );
      }
      _Thread_Enable_dispatch();
      return( RTEMS_ALREADY_SUSPENDED );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_task_resume
 *
 *  This directive will remove the specified thread
 *  from the suspended state.
 *
 *  Input parameters:
 *    id - thread id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_task_resume(
  Objects_Id id
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      return(
        _RTEMS_tasks_MP_Send_request_packet(
          RTEMS_TASKS_MP_RESUME_REQUEST,
          id,
          0,          /* Not used */
          0,          /* Not used */
          0           /* Not used */
        )
      );
    case OBJECTS_LOCAL:
      if ( _States_Is_suspended( the_thread->current_state ) ) {
        _Thread_Resume( the_thread );
        _Thread_Enable_dispatch();
        return( RTEMS_SUCCESSFUL );
      }
      _Thread_Enable_dispatch();
      return( RTEMS_INCORRECT_STATE );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_task_set_priority
 *
 *  This directive changes the priority of the specified thread.
 *  The specified thread can be any thread in the system including
 *  the requesting thread.
 *
 *  Input parameters:
 *    id           - thread id (0 indicates requesting thread)
 *    new_priority - thread priority (0 indicates current priority)
 *    old_priority - pointer to previous priority
 *
 *  Output parameters:
 *    old_priority      - previous priority
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_task_set_priority(
  Objects_Id        id,
  rtems_task_priority  new_priority,
  rtems_task_priority *old_priority
)
{
  register Thread_Control *the_thread;
  Objects_Locations               location;

  if ( new_priority != RTEMS_CURRENT_PRIORITY &&
       !_Priority_Is_valid( new_priority ) )
    return( RTEMS_INVALID_PRIORITY );

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      _Thread_Executing->Wait.return_argument = old_priority;
      return(
        _RTEMS_tasks_MP_Send_request_packet(
          RTEMS_TASKS_MP_SET_PRIORITY_REQUEST,
          id,
          new_priority,
          0,          /* Not used */
          0           /* Not used */
        )
      );
    case OBJECTS_LOCAL:
      *old_priority = the_thread->current_priority;
      if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
        the_thread->real_priority = new_priority;
        if ( the_thread->resource_count == 0 ||
             the_thread->current_priority > new_priority )
          _Thread_Change_priority( the_thread, new_priority );
      }
      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_task_mode
 *
 *  This directive enables and disables several modes of
 *  execution for the requesting thread.
 *
 *  Input parameters:
 *    mode_set          - new mode
 *    mask              - mask
 *    previous_mode_set - address of previous mode set
 *
 *  Output:
 *    *previous_mode_set - previous mode set
 *     always returns RTEMS_SUCCESSFUL
 */

rtems_status_code rtems_task_mode(
  rtems_mode  mode_set,
  rtems_mode  mask,
  rtems_mode *previous_mode_set
)
{
  if ( _Thread_Change_mode( mode_set, mask, previous_mode_set ) )
    _Thread_Dispatch();
  return( RTEMS_SUCCESSFUL );
}

/*PAGE
 *
 *  rtems_task_get_note
 *
 *  This directive obtains the note from the specified notepad
 *  of the specified thread.
 *
 *  Input parameters:
 *    id      - thread id
 *    notepad - notepad number
 *    note    - pointer to note
 *
 *  Output parameters:
 *    note              - filled in if successful
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_task_get_note(
  Objects_Id  id,
  unsigned32  notepad,
  unsigned32 *note
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;

  /*
   *  NOTE:  There is no check for < RTEMS_NOTEPAD_FIRST because that would
   *         be checking an unsigned number for being negative.
   */

  if ( notepad > RTEMS_NOTEPAD_LAST )
    return( RTEMS_INVALID_NUMBER );

  /*
   *  Optimize the most likely case to avoid the Thread_Dispatch.
   */

  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ||
       _Objects_Are_ids_equal( id, _Thread_Executing->Object.id ) ) {
      *note = _Thread_Executing->Notepads[ notepad ];
      return( RTEMS_SUCCESSFUL );
  }

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      _Thread_Executing->Wait.return_argument = note;

      return _RTEMS_tasks_MP_Send_request_packet(
        RTEMS_TASKS_MP_GET_NOTE_REQUEST,
        id,
        0,          /* Not used */
        notepad,
        0           /* Not used */
      );
    case OBJECTS_LOCAL:
      *note= the_thread->Notepads[ notepad ];
      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 * rtems_task_set_note
 *
 *  This directive sets the specified notepad contents to the given
 *  note.
 *
 *  Input parameters:
 *    id      - thread id
 *    notepad - notepad number
 *    note    - note value
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_task_set_note(
  Objects_Id id,
  unsigned32 notepad,
  unsigned32 note
)
{
  register Thread_Control *the_thread;
  Objects_Locations        location;

  /*
   *  NOTE:  There is no check for < RTEMS_NOTEPAD_FIRST because that would
   *         be checking an unsigned number for being negative.
   */

  if ( notepad > RTEMS_NOTEPAD_LAST )
    return( RTEMS_INVALID_NUMBER );

  /*
   *  Optimize the most likely case to avoid the Thread_Dispatch.
   */

  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ||
       _Objects_Are_ids_equal( id, _Thread_Executing->Object.id ) ) {
      _Thread_Executing->Notepads[ notepad ] = note;
      return( RTEMS_SUCCESSFUL );
  }

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      return _RTEMS_tasks_MP_Send_request_packet(
        RTEMS_TASKS_MP_SET_NOTE_REQUEST,
        id,
        0,          /* Not used */
        notepad,
        note
      );

    case OBJECTS_LOCAL:
      the_thread->Notepads[ notepad ] = note;
      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_task_wake_after
 *
 *  This directive suspends the requesting thread for the given amount
 *  of ticks.
 *
 *  Input parameters:
 *    ticks - number of ticks to wait
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - always successful
 */

rtems_status_code rtems_task_wake_after(
  rtems_interval ticks
)
{
  if ( ticks == 0 ) {
    _Thread_Yield_processor();
    _Thread_Dispatch();
  } else {
    _Thread_Disable_dispatch();
      _Thread_Set_state( _Thread_Executing, STATES_DELAYING );
      _Watchdog_Initialize(
        &_Thread_Executing->Timer,
        _Thread_Delay_ended,
        _Thread_Executing->Object.id,
        NULL
      );
      _Watchdog_Insert_ticks( &_Thread_Executing->Timer,
                              ticks, WATCHDOG_ACTIVATE_NOW );
    _Thread_Enable_dispatch();
  }
  return( RTEMS_SUCCESSFUL );
}

/*PAGE
 *
 *  rtems_task_wake_when
 *
 *  This directive blocks the requesting thread until the given date and
 *  time is reached.
 *
 *  Input parameters:
 *    time_buffer - pointer to the time and date structure
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_task_wake_when(
rtems_time_of_day *time_buffer
)
{
  rtems_interval seconds;
  rtems_status_code      local_result;

  if ( !_TOD_Is_set() )
    return( RTEMS_NOT_DEFINED );

  time_buffer->ticks = 0;

  local_result = _TOD_Validate( time_buffer );

  if  ( !rtems_is_status_successful( local_result ) )
    return( local_result );

  seconds = _TOD_To_seconds( time_buffer );

  if ( seconds <= _TOD_Seconds_since_epoch )
    return( RTEMS_INVALID_CLOCK );

  _Thread_Disable_dispatch();
    _Thread_Set_state( _Thread_Executing, STATES_WAITING_FOR_TIME );
    _Watchdog_Initialize(
      &_Thread_Executing->Timer,
      _Thread_Delay_ended,
      _Thread_Executing->Object.id,
      NULL
    );
    _Watchdog_Insert_seconds( &_Thread_Executing->Timer,
            seconds - _TOD_Seconds_since_epoch, WATCHDOG_ACTIVATE_NOW );
  _Thread_Enable_dispatch();
  return( RTEMS_SUCCESSFUL );
}
