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
#include <rtems/support.h>
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
 *  _RTEMS_tasks_Manager_initialization
 *
 *  This routine initializes all Task Manager related data structures.
 *
 *  Input parameters:
 *    maximum_tasks       - number of tasks to initialize
 *
 *  Output parameters:  NONE
 */

void _RTEMS_tasks_Manager_initialization(
  unsigned32   maximum_tasks
)
{
  _Objects_Initialize_information(
    &_RTEMS_tasks_Information,
    OBJECTS_RTEMS_TASKS,
    TRUE,
    maximum_tasks,
    sizeof( Thread_Control ),
    FALSE,
    RTEMS_MAXIMUM_NAME_LENGTH,
    TRUE
  );
}

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
  rtems_name           name,
  rtems_task_priority  initial_priority,
  unsigned32           stack_size,
  rtems_mode           initial_modes,
  rtems_attribute      attribute_set,
  Objects_Id          *id
)
{
  register Thread_Control *the_thread;
  Objects_MP_Control      *the_global_object = NULL;
  boolean                  is_fp;
  boolean                  is_global;
  rtems_attribute          the_attribute_set;
  Priority_Control         core_priority;
 

  if ( !rtems_is_name_valid( name ) )
    return ( RTEMS_INVALID_NAME );

  /* 
   *  Core Thread Initialize insures we get the minimum amount of
   *  stack space.
   */

#if 0
  if ( !_Stack_Is_enough( stack_size ) )
    return( RTEMS_INVALID_SIZE );
#endif

  /*
   *  Validate the RTEMS API priority and convert it to the core priority range.
   */

  if ( !_Priority_Is_valid( initial_priority ) )
    return( RTEMS_INVALID_PRIORITY );

  core_priority = _RTEMS_Tasks_Priority_to_Core( initial_priority );

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

  if ( _Attributes_Is_floating_point( the_attribute_set ) )
    is_fp = TRUE;
  else
    is_fp = FALSE;

  if ( _Attributes_Is_global( the_attribute_set ) ) {

    is_global = TRUE;

    if ( !_Configuration_Is_multiprocessing() )
      return( RTEMS_MP_NOT_CONFIGURED );

  } else
    is_global = FALSE;

  /*
   *  Make sure system is MP if this task is global
   */

  /*
   *  Disable dispatch for protection
   */ 

  _Thread_Disable_dispatch();

  /*
   *  Allocate the thread control block and -- if the task is global --
   *  allocate a global object control block.
   *
   *  NOTE:  This routine does not use the combined allocate and open
   *         global object routine because this results in a lack of
   *         control over when memory is allocated and can be freed in
   *         the event of an error.
   */

  the_thread = _RTEMS_tasks_Allocate();

  if ( !the_thread ) {
    _Thread_Enable_dispatch();
    return( RTEMS_TOO_MANY );
  }

  if ( is_global ) {
    the_global_object = _Objects_MP_Allocate_global_object();

    if ( _Objects_MP_Is_null_global_object( the_global_object ) ) {
      _RTEMS_tasks_Free( the_thread );
      _Thread_Enable_dispatch();
      return( RTEMS_TOO_MANY );
    }
  }

#if 0
  /*
   *  Allocate and initialize the RTEMS API specific information
   */

  the_thread->RTEMS_API = _Workspace_Allocate( sizeof( RTEMS_API_Control ) );

  if ( !the_thread->RTEMS_API ) {
    _RTEMS_tasks_Free( the_thread );
    if ( is_global )
    _Objects_MP_Free_global_object( the_global_object );
    _Thread_Enable_dispatch();
    return( RTEMS_UNSATISFIED );
  }

  the_thread->RTEMS_API->pending_events         = EVENT_SETS_NONE_PENDING;
  _ASR_Initialize( &the_thread->RTEMS_API->Signal );
#endif

  /*
   *  Initialize the core thread for this task.
   */

/* XXX normalize mode */

  if ( !_Thread_Initialize( &_RTEMS_tasks_Information, the_thread, 
         NULL, stack_size, is_fp, core_priority, initial_modes, &name ) ) {
    if ( is_global )
      _Objects_MP_Free_global_object( the_global_object );
    _RTEMS_tasks_Free( the_thread );
    _Thread_Enable_dispatch();
    return( RTEMS_UNSATISFIED );
  }

  *id = the_thread->Object.id;

  if ( is_global ) {

    the_thread->RTEMS_API->is_global = TRUE;

    _Objects_MP_Open(
      &_RTEMS_tasks_Information,
      the_global_object,
      name,
      the_thread->Object.id
    );

    _RTEMS_tasks_MP_Send_process_packet(
      RTEMS_TASKS_MP_ANNOUNCE_CREATE,
      the_thread->Object.id,
      name
    );

   }

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
  rtems_name    name,
  unsigned32    node,
  Objects_Id   *id
)
{
  if ( name != OBJECTS_ID_OF_SELF )
    return( _Objects_Name_to_id( &_RTEMS_tasks_Information, &name, node, id ) );

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
  rtems_id         id,
  rtems_task_entry entry_point,
  unsigned32       argument
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
      if ( _Thread_Start(
             the_thread, THREAD_START_NUMERIC, entry_point, NULL, argument ) ) {
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
  Objects_Locations        location;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return( RTEMS_ILLEGAL_ON_REMOTE_OBJECT );
    case OBJECTS_LOCAL:
      if ( _Thread_Restart( the_thread, NULL, argument ) ) {

        /*  XXX until these are in an API extension they are too late. */
        _ASR_Initialize( &the_thread->RTEMS_API->Signal );
        the_thread->RTEMS_API->pending_events = EVENT_SETS_NONE_PENDING;

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
      _Thread_Close( &_RTEMS_tasks_Information, the_thread );

      /* XXX */
      (void) _Workspace_Free( the_thread->RTEMS_API );

      _RTEMS_tasks_Free( the_thread );

      if ( _Attributes_Is_global( the_thread->RTEMS_API->is_global ) ) {

        _Objects_MP_Close( &_RTEMS_tasks_Information, the_thread->Object.id );

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
  Objects_Id           id,
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
      *note = _Thread_Executing->RTEMS_API->Notepads[ notepad ];
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
      *note= the_thread->RTEMS_API->Notepads[ notepad ];
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
      _Thread_Executing->RTEMS_API->Notepads[ notepad ] = note;
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
      the_thread->RTEMS_API->Notepads[ notepad ] = note;
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
