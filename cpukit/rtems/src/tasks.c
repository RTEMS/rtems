/*
 *  RTEMS Task Manager
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/object.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tod.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>

/*PAGE
 *
 *  _RTEMS_tasks_Create_extension
 *
 *  XXX
 */

boolean _RTEMS_tasks_Create_extension(
  Thread_Control *executing, 
  Thread_Control *created
)
{
  RTEMS_API_Control *api;

  api = _Workspace_Allocate( sizeof( RTEMS_API_Control ) );

  if ( !api )
    return FALSE;

  created->API_Extensions[ THREAD_API_RTEMS ] = api;
 
  api->pending_events = EVENT_SETS_NONE_PENDING;
  _ASR_Initialize( &api->Signal );
  return TRUE;
}

/*PAGE
 *
 *  _RTEMS_tasks_Start_extension
 *
 *  XXX
 */
 
User_extensions_routine _RTEMS_tasks_Start_extension(
  Thread_Control *executing,
  Thread_Control *started
)
{
  RTEMS_API_Control *api;

  api = started->API_Extensions[ THREAD_API_RTEMS ];
 
  api->pending_events = EVENT_SETS_NONE_PENDING;

  _ASR_Initialize( &api->Signal );
}

/*PAGE
 *
 *  _RTEMS_tasks_Delete_extension
 *
 *  XXX
 */
 
User_extensions_routine _RTEMS_tasks_Delete_extension(
  Thread_Control *executing, 
  Thread_Control *deleted
)
{
  (void) _Workspace_Free( deleted->API_Extensions[ THREAD_API_RTEMS ] );
 
  deleted->API_Extensions[ THREAD_API_RTEMS ] = NULL;
}

/*PAGE
 *
 *  _RTEMS_tasks_Switch_extension
 *
 *  XXX
 */
 
void _RTEMS_tasks_Switch_extension(
  Thread_Control *executing
)
{
  ISR_Level          level;
  RTEMS_API_Control *api;
  ASR_Information   *asr;
  rtems_signal_set   signal_set;
  Modes_Control      prev_mode;

  api = executing->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;
 
  _ISR_Disable( level );
    signal_set = asr->signals_posted;
    asr->signals_posted = 0;
  _ISR_Enable( level );
 
 
  if ( !signal_set ) /* similar to _ASR_Are_signals_pending( asr ) */
    return;
 
  asr->nest_level += 1;
  rtems_task_mode( asr->mode_set, RTEMS_ALL_MODE_MASKS, &prev_mode );
 
  (*asr->handler)( signal_set );

  asr->nest_level -= 1;
  rtems_task_mode( prev_mode, RTEMS_ALL_MODE_MASKS, &prev_mode );

}

API_extensions_Control _RTEMS_tasks_API_extensions = {
  { NULL, NULL },
  NULL,                                     /* predriver */
  _RTEMS_tasks_Initialize_user_tasks,       /* postdriver */
  _RTEMS_tasks_Switch_extension             /* post switch */
};

User_extensions_Control _RTEMS_tasks_User_extensions = {
  { NULL, NULL },
  { _RTEMS_tasks_Create_extension,            /* create */
    _RTEMS_tasks_Start_extension,             /* start */
    _RTEMS_tasks_Start_extension,             /* restart */
    _RTEMS_tasks_Delete_extension,            /* delete */
    NULL,                                     /* switch */
    NULL,                                     /* begin */
    NULL,                                     /* exitted */
    NULL                                      /* fatal */
  }
};

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
  unsigned32                        maximum_tasks,
  unsigned32                        number_of_initialization_tasks,
  rtems_initialization_tasks_table *user_tasks
)
{

  _RTEMS_tasks_Number_of_initialization_tasks = number_of_initialization_tasks;
  _RTEMS_tasks_User_initialization_tasks = user_tasks;

  /*
   *  There may not be any RTEMS initialization tasks configured.
   */

#if 0
  if ( user_tasks == NULL || number_of_initialization_tasks == 0 )
    _Internal_error_Occurred( INTERNAL_ERROR_RTEMS_API, TRUE, RTEMS_TOO_MANY );
#endif

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

  /*
   *  Add all the extensions for this API
   */

  _User_extensions_Add_API_set( &_RTEMS_tasks_User_extensions );

  _API_extensions_Add( &_RTEMS_tasks_API_extensions );

  /*
   *  Register the MP Process Packet routine.
   */

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Register_packet_processor(
    MP_PACKET_TASKS,
    _RTEMS_tasks_MP_Process_packet
  );
#endif

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
  boolean                  is_fp;
#if defined(RTEMS_MULTIPROCESSING)
  Objects_MP_Control      *the_global_object = NULL;
  boolean                  is_global;
#endif
  boolean                  status;
  rtems_attribute          the_attribute_set;
  Priority_Control         core_priority;
  RTEMS_API_Control       *api;
  ASR_Information         *asr;
 

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  /* 
   *  Core Thread Initialize insures we get the minimum amount of
   *  stack space.
   */

#if 0
  if ( !_Stack_Is_enough( stack_size ) )
    return RTEMS_INVALID_SIZE;
#endif

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

  /*
   *  Validate the RTEMS API priority and convert it to the core priority range.
   */

  if ( !_Attributes_Is_system_task( the_attribute_set ) ) {
    if ( !_RTEMS_tasks_Priority_is_valid( initial_priority ) )
      return RTEMS_INVALID_PRIORITY;
  }

  core_priority = _RTEMS_tasks_Priority_to_Core( initial_priority );

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Attributes_Is_global( the_attribute_set ) ) {

    is_global = TRUE;

    if ( !_System_state_Is_multiprocessing )
      return RTEMS_MP_NOT_CONFIGURED;

  } else
    is_global = FALSE;
#endif

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
    return RTEMS_TOO_MANY;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( is_global ) {
    the_global_object = _Objects_MP_Allocate_global_object();

    if ( _Objects_MP_Is_null_global_object( the_global_object ) ) {
      _RTEMS_tasks_Free( the_thread );
      _Thread_Enable_dispatch();
      return RTEMS_TOO_MANY;
    }
  }
#endif

  /*
   *  Initialize the core thread for this task.
   */

  status = _Thread_Initialize(
    &_RTEMS_tasks_Information,
    the_thread, 
    NULL,
    stack_size,
    is_fp,
    core_priority,
    _Modes_Is_preempt(initial_modes)   ? TRUE : FALSE,
    _Modes_Is_timeslice(initial_modes) ?
      THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE :
      THREAD_CPU_BUDGET_ALGORITHM_NONE,
    NULL,        /* no budget algorithm callout */
    _Modes_Get_interrupt_level(initial_modes),
    &name
  );

  if ( !status ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( is_global )
      _Objects_MP_Free_global_object( the_global_object );
#endif
    _RTEMS_tasks_Free( the_thread );
    _Thread_Enable_dispatch();
    return RTEMS_UNSATISFIED;
  }

  api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;
 
  asr->is_enabled = _Modes_Is_asr_disabled(initial_modes) ? FALSE : TRUE;

  *id = the_thread->Object.id;

#if defined(RTEMS_MULTIPROCESSING)
  if ( is_global ) {

    the_thread->is_global = TRUE;

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
#endif

  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
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
  Objects_Name_to_id_errors  status;

  if ( name == OBJECTS_ID_OF_SELF ) {
    *id = _Thread_Executing->Object.id;
    return RTEMS_SUCCESSFUL;
   }

  status = _Objects_Name_to_id( &_RTEMS_tasks_Information, &name, node, id );

  return _Status_Object_name_errors_to_status[ status ];
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
    return RTEMS_INVALID_ADDRESS;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      if ( _Thread_Start(
             the_thread, THREAD_START_NUMERIC, entry_point, NULL, argument ) ) {
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_INCORRECT_STATE;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
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

    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      if ( _Thread_Restart( the_thread, NULL, argument ) ) {
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_INCORRECT_STATE;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
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
  Objects_Information     *the_information;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      _Thread_Dispatch();
      return RTEMS_ILLEGAL_ON_REMOTE_OBJECT;
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      the_information = _Objects_Get_information( the_thread->Object.id );

      if ( !the_information ) {
        _Thread_Enable_dispatch();
        return RTEMS_INVALID_ID;
        /* This should never happen if _Thread_Get() works right */
      }
  
      _Thread_Close( the_information, the_thread );

      _RTEMS_tasks_Free( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
      if ( the_thread->is_global ) {

        _Objects_MP_Close( &_RTEMS_tasks_Information, the_thread->Object.id );

        _RTEMS_tasks_MP_Send_process_packet(
          RTEMS_TASKS_MP_ANNOUNCE_DELETE,
          the_thread->Object.id,
          0                                /* Not used */
        );
      }
#endif

      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
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

    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      return _RTEMS_tasks_MP_Send_request_packet(
        RTEMS_TASKS_MP_SUSPEND_REQUEST,
        id,
        0,          /* Not used */
        0,          /* Not used */
        0           /* Not used */
      );
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      if ( !_States_Is_suspended( the_thread->current_state ) ) {
        _Thread_Set_state( the_thread, STATES_SUSPENDED );
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_ALREADY_SUSPENDED;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
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

    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      return _RTEMS_tasks_MP_Send_request_packet(
          RTEMS_TASKS_MP_RESUME_REQUEST,
          id,
          0,          /* Not used */
          0,          /* Not used */
          0           /* Not used */
        );
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      if ( _States_Is_suspended( the_thread->current_state ) ) {
        _Thread_Resume( the_thread );
        _Thread_Enable_dispatch();
        return RTEMS_SUCCESSFUL;
      }
      _Thread_Enable_dispatch();
      return RTEMS_INCORRECT_STATE;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
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
       !_RTEMS_tasks_Priority_is_valid( new_priority ) )
    return RTEMS_INVALID_PRIORITY;

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      _Thread_Executing->Wait.return_argument = old_priority;
      return _RTEMS_tasks_MP_Send_request_packet(
          RTEMS_TASKS_MP_SET_PRIORITY_REQUEST,
          id,
          new_priority,
          0,          /* Not used */
          0           /* Not used */
      );
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      *old_priority = the_thread->current_priority;
      if ( new_priority != RTEMS_CURRENT_PRIORITY ) {
        the_thread->real_priority = new_priority;
        if ( the_thread->resource_count == 0 ||
             the_thread->current_priority > new_priority )
          _Thread_Change_priority( the_thread, new_priority, FALSE );
      }
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
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
 *     always return RTEMS_SUCCESSFUL;
 */

rtems_status_code rtems_task_mode(
  rtems_mode  mode_set,
  rtems_mode  mask,
  rtems_mode *previous_mode_set
)
{
  Thread_Control     *executing;
  RTEMS_API_Control  *api;
  ASR_Information    *asr;
  boolean             is_asr_enabled = FALSE;
  boolean             needs_asr_dispatching = FALSE;
  rtems_mode          old_mode;

  executing     = _Thread_Executing;
  api = executing->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;

  old_mode  = (executing->is_preemptible) ? RTEMS_PREEMPT : RTEMS_NO_PREEMPT;

  if ( executing->budget_algorithm == THREAD_CPU_BUDGET_ALGORITHM_NONE )
    old_mode |= RTEMS_NO_TIMESLICE;
  else
    old_mode |= RTEMS_TIMESLICE;

  old_mode |= (asr->is_enabled) ? RTEMS_ASR : RTEMS_NO_ASR;
  old_mode |= _ISR_Get_level();

  *previous_mode_set = old_mode;

  /*
   *  These are generic thread scheduling characteristics.
   */

  if ( mask & RTEMS_PREEMPT_MASK )
    executing->is_preemptible = _Modes_Is_preempt(mode_set) ? TRUE : FALSE;

  if ( mask & RTEMS_TIMESLICE_MASK ) {
    if ( _Modes_Is_timeslice(mode_set) ) 
      executing->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE;
    else
      executing->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_NONE;
  }

  /*
   *  Set the new interrupt level
   */

  if ( mask & RTEMS_INTERRUPT_MASK )
    _Modes_Set_interrupt_level( mode_set );

  /*
   *  This is specific to the RTEMS API
   */

  is_asr_enabled = FALSE;
  needs_asr_dispatching = FALSE;

  if ( mask & RTEMS_ASR_MASK ) {
    is_asr_enabled = _Modes_Is_asr_disabled( mode_set ) ? FALSE : TRUE;
    if ( is_asr_enabled != asr->is_enabled ) {
      asr->is_enabled = is_asr_enabled;
      _ASR_Swap_signals( asr );
      if ( _ASR_Are_signals_pending( asr ) ) {
        needs_asr_dispatching = TRUE;
        executing->do_post_task_switch_extension = TRUE;
      }
    }
  }

  if ( _Thread_Evaluate_mode() || needs_asr_dispatching )
    _Thread_Dispatch();

  return RTEMS_SUCCESSFUL;
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
  RTEMS_API_Control       *api;

  /*
   *  NOTE:  There is no check for < RTEMS_NOTEPAD_FIRST because that would
   *         be checking an unsigned number for being negative.
   */

  if ( notepad > RTEMS_NOTEPAD_LAST )
    return RTEMS_INVALID_NUMBER;

  /*
   *  Optimize the most likely case to avoid the Thread_Dispatch.
   */

  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ||
       _Objects_Are_ids_equal( id, _Thread_Executing->Object.id ) ) {
      api = _Thread_Executing->API_Extensions[ THREAD_API_RTEMS ];
      *note = api->Notepads[ notepad ];
      return RTEMS_SUCCESSFUL;
  }

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      _Thread_Executing->Wait.return_argument = note;

      return _RTEMS_tasks_MP_Send_request_packet(
        RTEMS_TASKS_MP_GET_NOTE_REQUEST,
        id,
        0,          /* Not used */
        notepad,
        0           /* Not used */
      );
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
      *note = api->Notepads[ notepad ];
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
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
  RTEMS_API_Control       *api;

  /*
   *  NOTE:  There is no check for < RTEMS_NOTEPAD_FIRST because that would
   *         be checking an unsigned number for being negative.
   */

  if ( notepad > RTEMS_NOTEPAD_LAST )
    return RTEMS_INVALID_NUMBER;

  /*
   *  Optimize the most likely case to avoid the Thread_Dispatch.
   */

  if ( _Objects_Are_ids_equal( id, OBJECTS_ID_OF_SELF ) ||
       _Objects_Are_ids_equal( id, _Thread_Executing->Object.id ) ) {
      api = _Thread_Executing->API_Extensions[ THREAD_API_RTEMS ];
      api->Notepads[ notepad ] = note;
      return RTEMS_SUCCESSFUL;
  }

  the_thread = _Thread_Get( id, &location );
  switch ( location ) {

    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      return _RTEMS_tasks_MP_Send_request_packet(
        RTEMS_TASKS_MP_SET_NOTE_REQUEST,
        id,
        0,          /* Not used */
        notepad,
        note
      );
#endif

    case OBJECTS_ERROR:
      return RTEMS_INVALID_ID;

    case OBJECTS_LOCAL:
      api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
      api->Notepads[ notepad ] = note;
      _Thread_Enable_dispatch();
      return RTEMS_SUCCESSFUL;
  }

  return RTEMS_INTERNAL_ERROR;   /* unreached - only to remove warnings */
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
  _Thread_Disable_dispatch();
    if ( ticks == 0 ) {
      _Thread_Yield_processor();
    } else {
      _Thread_Set_state( _Thread_Executing, STATES_DELAYING );
      _Watchdog_Initialize(
        &_Thread_Executing->Timer,
        _Thread_Delay_ended,
        _Thread_Executing->Object.id,
        NULL
      );
      _Watchdog_Insert_ticks( &_Thread_Executing->Timer, ticks );
    }
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
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
  Watchdog_Interval   seconds;

  if ( !_TOD_Is_set )
    return RTEMS_NOT_DEFINED;

  time_buffer->ticks = 0;

  if ( !_TOD_Validate( time_buffer ) )
    return RTEMS_INVALID_CLOCK;

  seconds = _TOD_To_seconds( time_buffer );

  if ( seconds <= _TOD_Seconds_since_epoch )
    return RTEMS_INVALID_CLOCK;

  _Thread_Disable_dispatch();
    _Thread_Set_state( _Thread_Executing, STATES_WAITING_FOR_TIME );
    _Watchdog_Initialize(
      &_Thread_Executing->Timer,
      _Thread_Delay_ended,
      _Thread_Executing->Object.id,
      NULL
    );
    _Watchdog_Insert_seconds(
      &_Thread_Executing->Timer,
      seconds - _TOD_Seconds_since_epoch
    );
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}

/*PAGE
 *
 *  _RTEMS_tasks_Initialize_user_tasks
 *
 *  This routine creates and starts all configured user
 *  initialzation threads.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 */

void _RTEMS_tasks_Initialize_user_tasks( void )
{
  unsigned32                        index;
  unsigned32                        maximum;
  rtems_id                          id;
  rtems_status_code                 return_value;
  rtems_initialization_tasks_table *user_tasks;

  /*
   *  NOTE:  This is slightly different from the Ada implementation.
   */

  user_tasks = _RTEMS_tasks_User_initialization_tasks;
  maximum    = _RTEMS_tasks_Number_of_initialization_tasks;

  if ( !user_tasks || maximum == 0 )
    return;

  for ( index=0 ; index < maximum ; index++ ) {
    return_value = rtems_task_create(
      user_tasks[ index ].name,
      user_tasks[ index ].initial_priority,
      user_tasks[ index ].stack_size,
      user_tasks[ index ].mode_set,
      user_tasks[ index ].attribute_set,
      &id
    );

    if ( !rtems_is_status_successful( return_value ) )
      _Internal_error_Occurred( INTERNAL_ERROR_RTEMS_API, TRUE, return_value );

    return_value = rtems_task_start(
      id,
      user_tasks[ index ].entry_point,
      user_tasks[ index ].argument
    );

    if ( !rtems_is_status_successful( return_value ) )
      _Internal_error_Occurred( INTERNAL_ERROR_RTEMS_API, TRUE, return_value );
  }
}

