/*
 *  Semaphore Manager
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the Semaphore Manager.
 *  This manager utilizes standard Dijkstra counting semaphores to provide
 *  synchronization and mutual exclusion capabilities.
 *
 *  Directives provided are:
 *
 *     + create a semaphore
 *     + get an ID of a semaphore
 *     + delete a semaphore
 *     + acquire a semaphore
 *     + release a semaphore
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
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/attr.h>
#include <rtems/core/isr.h>
#include <rtems/core/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/sem.h>
#include <rtems/core/coremutex.h>
#include <rtems/core/coresem.h>
#include <rtems/core/states.h>
#include <rtems/core/thread.h>
#include <rtems/core/threadq.h>
#include <rtems/core/mpci.h>
#include <rtems/sysstate.h>

#include <rtems/core/interr.h>

/*PAGE
 *
 *  _Semaphore_Manager_initialization
 *
 *  This routine initializes all semaphore manager related data structures.
 *
 *  Input parameters:
 *    maximum_semaphores - maximum configured semaphores
 *
 *  Output parameters:  NONE
 */

void _Semaphore_Manager_initialization(
  unsigned32 maximum_semaphores
)
{
  _Objects_Initialize_information(
    &_Semaphore_Information,
    OBJECTS_RTEMS_SEMAPHORES,
    TRUE,
    maximum_semaphores,
    sizeof( Semaphore_Control ),
    FALSE,
    RTEMS_MAXIMUM_NAME_LENGTH,
    FALSE
  );
 
  /*
   *  Register the MP Process Packet routine.
   */
 
  _MPCI_Register_packet_processor(
    MP_PACKET_SEMAPHORE,
    _Semaphore_MP_Process_packet
  );

}

/*PAGE
 *
 *  rtems_semaphore_create
 *
 *  This directive creates a semaphore and sets the initial value based
 *  on the given count.  A semaphore id is returned.
 *
 *  Input parameters:
 *    name             - user defined semaphore name
 *    count            - initial count of semaphore
 *    attribute_set    - semaphore attributes
 *    priority_ceiling - semaphore's ceiling priority 
 *    id               - pointer to semaphore id
 *
 *  Output parameters:
 *    id       - semaphore id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_semaphore_create(
  rtems_name           name,
  unsigned32           count,
  rtems_attribute      attribute_set,
  rtems_task_priority  priority_ceiling,
  Objects_Id          *id
)
{
  register Semaphore_Control *the_semaphore;
  CORE_mutex_Attributes       the_mutex_attributes;
  CORE_semaphore_Attributes   the_semaphore_attributes;
  unsigned32                  lock;

  if ( !rtems_is_name_valid( name ) )
    return ( RTEMS_INVALID_NAME );

  if ( _Attributes_Is_global( attribute_set ) ) {

    if ( !_System_state_Is_multiprocessing )
      return( RTEMS_MP_NOT_CONFIGURED );

    if ( _Attributes_Is_inherit_priority( attribute_set ) )
      return( RTEMS_NOT_DEFINED );

  } else if ( _Attributes_Is_inherit_priority( attribute_set ) ) {

    if ( ! ( _Attributes_Is_binary_semaphore( attribute_set ) &&
             _Attributes_Is_priority( attribute_set ) ) )
      return( RTEMS_NOT_DEFINED );

  }

  if ( _Attributes_Is_binary_semaphore( attribute_set ) && ( count > 1 ) )
    return( RTEMS_INVALID_NUMBER );

  _Thread_Disable_dispatch();             /* prevents deletion */

  the_semaphore = _Semaphore_Allocate();

  if ( !the_semaphore ) {
    _Thread_Enable_dispatch();
    return( RTEMS_TOO_MANY );
  }

  if ( _Attributes_Is_global( attribute_set ) &&
       ! ( _Objects_MP_Allocate_and_open( &_Semaphore_Information, name,
                            the_semaphore->Object.id, FALSE ) ) ) {
    _Semaphore_Free( the_semaphore );
    _Thread_Enable_dispatch();
    return( RTEMS_TOO_MANY );
  }

  the_semaphore->attribute_set = attribute_set;

  if ( _Attributes_Is_binary_semaphore( attribute_set ) ) {
    if ( _Attributes_Is_inherit_priority( attribute_set ) )
      the_mutex_attributes.discipline = CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT;
    else if (_Attributes_Is_priority_ceiling( attribute_set ) )
      the_mutex_attributes.discipline = CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING;
    else if (_Attributes_Is_priority( attribute_set ) )
      the_mutex_attributes.discipline = CORE_MUTEX_DISCIPLINES_PRIORITY;
    else
      the_mutex_attributes.discipline = CORE_MUTEX_DISCIPLINES_FIFO;

    the_mutex_attributes.allow_nesting = TRUE;

    /* Add priority ceiling code here ????? */

    if ( count == 1 )
      lock = CORE_MUTEX_UNLOCKED;
    else
      lock = CORE_MUTEX_LOCKED;

    _CORE_mutex_Initialize(
      &the_semaphore->Core_control.mutex,
      OBJECTS_RTEMS_SEMAPHORES,
      &the_mutex_attributes,
      lock,
      _Semaphore_MP_Send_extract_proxy
    );
  }
  else {
    if ( _Attributes_Is_priority( attribute_set ) )
      the_semaphore_attributes.discipline = CORE_SEMAPHORE_DISCIPLINES_PRIORITY;
    else
      the_semaphore_attributes.discipline = CORE_SEMAPHORE_DISCIPLINES_FIFO;

    _CORE_semaphore_Initialize(
      &the_semaphore->Core_control.semaphore,
      OBJECTS_RTEMS_SEMAPHORES,
      &the_semaphore_attributes,
      count,
      _Semaphore_MP_Send_extract_proxy
    );
  }

  _Objects_Open( &_Semaphore_Information, &the_semaphore->Object, &name );

  *id = the_semaphore->Object.id;

  if ( _Attributes_Is_global( attribute_set ) )
    _Semaphore_MP_Send_process_packet(
      SEMAPHORE_MP_ANNOUNCE_CREATE,
      the_semaphore->Object.id,
      name,
      0                          /* Not used */
    );
  _Thread_Enable_dispatch();
  return( RTEMS_SUCCESSFUL );
}

/*PAGE
 *
 *  rtems_semaphore_ident
 *
 *  This directive returns the system ID associated with
 *  the semaphore name.
 *
 *  Input parameters:
 *    name - user defined semaphore name
 *    node - node(s) to be searched
 *    id   - pointer to semaphore id
 *
 *  Output parameters:
 *    *id      - semaphore id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_semaphore_ident(
  rtems_name  name,
  unsigned32  node,
  Objects_Id *id
)
{
  Objects_Name_to_id_errors  status;
 
  status = _Objects_Name_to_id( &_Semaphore_Information, &name, node, id );
 
  return _Status_Object_name_errors_to_status[ status ];
}

/*PAGE
 *
 *  rtems_semaphore_delete
 *
 *  This directive allows a thread to delete a semaphore specified by
 *  the semaphore id.  The semaphore is freed back to the inactive
 *  semaphore chain.
 *
 *  Input parameters:
 *    id - semaphore id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_semaphore_delete(
  Objects_Id id
)
{
  register Semaphore_Control *the_semaphore;
  Objects_Locations           location;

  the_semaphore = _Semaphore_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return( RTEMS_ILLEGAL_ON_REMOTE_OBJECT );
    case OBJECTS_LOCAL:
      if ( _Attributes_Is_binary_semaphore( the_semaphore->attribute_set) ) { 
        if ( _CORE_mutex_Is_locked( &the_semaphore->Core_control.mutex ) ) {
          _Thread_Enable_dispatch();
          return( RTEMS_RESOURCE_IN_USE );
        }
        else
          _CORE_mutex_Flush(
            &the_semaphore->Core_control.mutex, 
            _Semaphore_MP_Send_object_was_deleted,
            CORE_MUTEX_WAS_DELETED
          );
      }
      else
        _CORE_semaphore_Flush(
          &the_semaphore->Core_control.semaphore, 
          _Semaphore_MP_Send_object_was_deleted,
          CORE_SEMAPHORE_WAS_DELETED
        );

      _Objects_Close( &_Semaphore_Information, &the_semaphore->Object );

      _Semaphore_Free( the_semaphore );

      if ( _Attributes_Is_global( the_semaphore->attribute_set ) ) {

        _Objects_MP_Close( &_Semaphore_Information, the_semaphore->Object.id );

        _Semaphore_MP_Send_process_packet(
          SEMAPHORE_MP_ANNOUNCE_DELETE,
          the_semaphore->Object.id,
          0,                         /* Not used */
          0                          /* Not used */
        );
      }
      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_semaphore_obtain
 *
 *  This directive allows a thread to acquire a semaphore.
 *
 *  Input parameters:
 *    id         - semaphore id
 *    option_set - wait option
 *    timeout    - number of ticks to wait (0 means wait forever)
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_semaphore_obtain(
  Objects_Id      id,
  unsigned32      option_set,
  rtems_interval  timeout
)
{
  register Semaphore_Control *the_semaphore;
  Objects_Locations           location;
  boolean                     wait;

  the_semaphore = _Semaphore_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      return _Semaphore_MP_Send_request_packet(
          SEMAPHORE_MP_OBTAIN_REQUEST,
          id,
          option_set,
          timeout
      );
    case OBJECTS_LOCAL:
      if ( _Options_Is_no_wait( option_set ) )
        wait = FALSE;
      else
        wait = TRUE;

      if ( _Attributes_Is_binary_semaphore( the_semaphore->attribute_set ) ) {
        _CORE_mutex_Seize(
          &the_semaphore->Core_control.mutex,
          id, 
          wait,
          timeout
        );
        _Thread_Enable_dispatch();
        return( _Semaphore_Translate_core_mutex_return_code( 
                  _Thread_Executing->Wait.return_code ) );
      } else {
        _CORE_semaphore_Seize(
          &the_semaphore->Core_control.semaphore,
          id,
          wait,
          timeout
        );
        _Thread_Enable_dispatch();
        return( _Semaphore_Translate_core_semaphore_return_code( 
                  _Thread_Executing->Wait.return_code ) );
      }
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  rtems_semaphore_release
 *
 *  This directive allows a thread to release a semaphore.
 *
 *  Input parameters:
 *    id - semaphore id
 *
 *  Output parameters:
 *    RTEMS_SUCCESSFUL - if successful
 *    error code        - if unsuccessful
 */

rtems_status_code rtems_semaphore_release(
  Objects_Id id
)
{
  register Semaphore_Control *the_semaphore;
  Objects_Locations           location;
  CORE_mutex_Status           mutex_status;
  CORE_semaphore_Status       semaphore_status;

  the_semaphore = _Semaphore_Get( id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      return( RTEMS_INVALID_ID );
    case OBJECTS_REMOTE:
      return(
        _Semaphore_MP_Send_request_packet(
          SEMAPHORE_MP_RELEASE_REQUEST,
          id,
          0,                               /* Not used */
          MPCI_DEFAULT_TIMEOUT
        )
      );
    case OBJECTS_LOCAL:
      if ( _Attributes_Is_binary_semaphore( the_semaphore->attribute_set ) ) {
        mutex_status = _CORE_mutex_Surrender(
                         &the_semaphore->Core_control.mutex,
                         id,
                         _Semaphore_Core_mutex_mp_support
                       );
        _Thread_Enable_dispatch();
        return( _Semaphore_Translate_core_mutex_return_code( mutex_status ) );
      }
      else
        semaphore_status = _CORE_semaphore_Surrender(
                             &the_semaphore->Core_control.semaphore,
                             id,
                             _Semaphore_Core_semaphore_mp_support
                           );
        _Thread_Enable_dispatch();
        return(
          _Semaphore_Translate_core_semaphore_return_code( semaphore_status ) );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  _Semaphore_Translate_core_mutex_return_code
 *
 *  Input parameters:
 *    the_mutex_status - mutex status code to translate
 *
 *  Output parameters:
 *    rtems status code - translated RTEMS status code
 *
 */
 
rtems_status_code _Semaphore_Translate_core_mutex_return_code (
  unsigned32 the_mutex_status
)
{
  switch ( the_mutex_status ) {
    case  CORE_MUTEX_STATUS_SUCCESSFUL:
      return( RTEMS_SUCCESSFUL );
    case CORE_MUTEX_STATUS_UNSATISFIED_NOWAIT:
      return( RTEMS_UNSATISFIED );
    case CORE_MUTEX_STATUS_NESTING_NOT_ALLOWED:
      return( RTEMS_INTERNAL_ERROR );
    case CORE_MUTEX_STATUS_NOT_OWNER_OF_RESOURCE:
      return( RTEMS_NOT_OWNER_OF_RESOURCE );
    case CORE_MUTEX_WAS_DELETED:
      return( RTEMS_OBJECT_WAS_DELETED );
    case CORE_MUTEX_TIMEOUT:
      return( RTEMS_TIMEOUT );
    case THREAD_STATUS_PROXY_BLOCKING:
      return( THREAD_STATUS_PROXY_BLOCKING );
  }
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    TRUE,
    the_mutex_status
  );
  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  _Semaphore_Translate_core_semaphore_return_code
 *
 *  Input parameters:
 *    the_semaphore_status - semaphore status code to translate
 *
 *  Output parameters:
 *    rtems status code - translated RTEMS status code
 *
 */
 
rtems_status_code _Semaphore_Translate_core_semaphore_return_code (
  unsigned32 the_semaphore_status
)
{
  switch ( the_semaphore_status ) {
    case  CORE_SEMAPHORE_STATUS_SUCCESSFUL:
      return( RTEMS_SUCCESSFUL );
    case CORE_SEMAPHORE_STATUS_UNSATISFIED_NOWAIT:
      return( RTEMS_UNSATISFIED );
    case CORE_SEMAPHORE_WAS_DELETED:
      return( RTEMS_OBJECT_WAS_DELETED );
    case CORE_SEMAPHORE_TIMEOUT:
      return( RTEMS_TIMEOUT );
    case THREAD_STATUS_PROXY_BLOCKING:
      return( THREAD_STATUS_PROXY_BLOCKING );
  }
  _Internal_error_Occurred(
    INTERNAL_ERROR_RTEMS_API,
    TRUE,
    the_semaphore_status
  );
  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  _Semaphore_Core_mutex_mp_support
 *
 *  Input parameters:
 *    the_thread - the remote thread the semaphore was surrendered to
 *    id         - id of the surrendered semaphore
 *
 *  Output parameters: NONE
 */
 
void  _Semaphore_Core_mutex_mp_support (
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  the_thread->receive_packet->return_code = RTEMS_SUCCESSFUL;
 
  _Semaphore_MP_Send_response_packet(
     SEMAPHORE_MP_OBTAIN_RESPONSE,
     id,
     the_thread
   );
}


/*PAGE
 *
 *  _Semaphore_Core_semaphore_mp_support
 *
 *  Input parameters:
 *    the_thread - the remote thread the semaphore was surrendered to
 *    id         - id of the surrendered semaphore
 *
 *  Output parameters: NONE
 */
 
void  _Semaphore_Core_semaphore_mp_support (
  Thread_Control *the_thread,
  Objects_Id      id
)
{
  the_thread->receive_packet->return_code = RTEMS_SUCCESSFUL;
 
  _Semaphore_MP_Send_response_packet(
     SEMAPHORE_MP_OBTAIN_RESPONSE,
     id,
     the_thread
   );
}
