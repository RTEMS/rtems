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
#include <rtems/support.h>
#include <rtems/attr.h>
#include <rtems/config.h>
#include <rtems/isr.h>
#include <rtems/object.h>
#include <rtems/options.h>
#include <rtems/sem.h>
#include <rtems/states.h>
#include <rtems/thread.h>
#include <rtems/threadq.h>
#include <rtems/mpci.h>

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
}

/*PAGE
 *
 *  rtems_semaphore_create
 *
 *  This directive creates a semaphore and sets the initial value based
 *  on the given count.  A semaphore id is returned.
 *
 *  Input parameters:
 *    name          - user defined semaphore name
 *    count         - initial count of semaphore
 *    attribute_set - semaphore attributes
 *    id            - pointer to semaphore id
 *
 *  Output parameters:
 *    id       - semaphore id
 *    RTEMS_SUCCESSFUL - if successful
 *    error code - if unsuccessful
 */

rtems_status_code rtems_semaphore_create(
  rtems_name            name,
  unsigned32            count,
  rtems_attribute       attribute_set,
  rtems_task_priority   priority_ceiling,
  Objects_Id           *id
)
{
  register Semaphore_Control *the_semaphore;

  if ( !rtems_is_name_valid( name ) )
    return ( RTEMS_INVALID_NAME );

  if ( _Attributes_Is_global( attribute_set ) ) {

    if ( !_Configuration_Is_multiprocessing() )
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
       !( _Objects_MP_Allocate_and_open( &_Semaphore_Information, name,
                            the_semaphore->Object.id, FALSE ) ) ) {
    _Semaphore_Free( the_semaphore );
    _Thread_Enable_dispatch();
    return( RTEMS_TOO_MANY );
  }

  the_semaphore->attribute_set = attribute_set;
  the_semaphore->count         = count;

  if ( _Attributes_Is_binary_semaphore( attribute_set ) && count == 0 ) {
    the_semaphore->nest_count = 1;
    the_semaphore->holder     = _Thread_Executing;
    the_semaphore->holder_id  = _Thread_Executing->Object.id;
    _Thread_Executing->resource_count++;
  } else {
    the_semaphore->nest_count = 0;
    the_semaphore->holder     = NULL;
    the_semaphore->holder_id  = 0;
  }

  _Thread_queue_Initialize(
    &the_semaphore->Wait_queue,
    OBJECTS_RTEMS_SEMAPHORES,
    _Attributes_Is_priority( attribute_set ) ? 
       THREAD_QUEUE_DISCIPLINE_PRIORITY : THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_SEMAPHORE,
    _Semaphore_MP_Send_extract_proxy
  );

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
  rtems_name    name,
  unsigned32    node,
  Objects_Id   *id
)
{
  return( _Objects_Name_to_id( &_Semaphore_Information, &name, node, id ) );
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
      if ( _Attributes_Is_binary_semaphore( the_semaphore->attribute_set) &&
                       ( the_semaphore->count == 0 ) ) {
        _Thread_Enable_dispatch();
        return( RTEMS_RESOURCE_IN_USE );
      }

      _Objects_Close( &_Semaphore_Information, &the_semaphore->Object );

      _Thread_queue_Flush(
        &the_semaphore->Wait_queue,
        _Semaphore_MP_Send_object_was_deleted
      );

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
  Objects_Id        id,
  unsigned32        option_set,
  rtems_interval timeout
)
{
  register Semaphore_Control *the_semaphore;
  Objects_Locations           location;

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
      if ( !_Semaphore_Seize( the_semaphore, option_set ) ) {
        if ( _Attributes_Is_inherit_priority( the_semaphore->attribute_set ) &&
             the_semaphore->holder->current_priority >
               _Thread_Executing->current_priority ) {
            _Thread_Change_priority(
              the_semaphore->holder, _Thread_Executing->current_priority );
         }
        _Thread_queue_Enqueue( &the_semaphore->Wait_queue, timeout );
      }
      _Thread_Enable_dispatch();
      return( _Thread_Executing->Wait.return_code );
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
  Thread_Control             *the_thread;

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
      if ( _Attributes_Is_binary_semaphore( the_semaphore->attribute_set)) {

        if ( !_Objects_Are_ids_equal(
               _Thread_Executing->Object.id, the_semaphore->holder_id ) ) {
          _Thread_Enable_dispatch();
          return( RTEMS_NOT_OWNER_OF_RESOURCE );
        }

        the_semaphore->nest_count--;

        if ( the_semaphore->nest_count != 0 ) {
          _Thread_Enable_dispatch();
          return( RTEMS_SUCCESSFUL );
        }

        _Thread_Executing->resource_count--;
        the_semaphore->holder    = NULL;
        the_semaphore->holder_id = 0;

        /*
         *  Whether or not someone is waiting for the semaphore, an
         *  inherited priority must be lowered if this is the last
         *  semaphore (i.e. resource) this task has.
         */

        if ( _Attributes_Is_inherit_priority(the_semaphore->attribute_set) &&
             _Thread_Executing->resource_count == 0 &&
             _Thread_Executing->real_priority !=
                _Thread_Executing->current_priority ) {
           _Thread_Change_priority(
              _Thread_Executing, _Thread_Executing->real_priority );
        }

      }

      if ( (the_thread = _Thread_queue_Dequeue(&the_semaphore->Wait_queue)) ) {

        if ( !_Objects_Is_local_id( the_thread->Object.id ) ) {
          the_thread->receive_packet->return_code = RTEMS_SUCCESSFUL;

          if ( _Attributes_Is_binary_semaphore(the_semaphore->attribute_set) ) {
            the_semaphore->holder    = NULL;
            the_semaphore->holder_id = the_thread->Object.id;
            the_semaphore->nest_count = 1;
          }

          _Semaphore_MP_Send_response_packet(
            SEMAPHORE_MP_OBTAIN_RESPONSE,
            id,
            the_thread
          );
        } else {

          if ( _Attributes_Is_binary_semaphore(the_semaphore->attribute_set) ) {
            the_semaphore->holder    = the_thread;
            the_semaphore->holder_id = the_thread->Object.id;
            the_thread->resource_count++;
            the_semaphore->nest_count = 1;
          }

          /*
           *  No special action for priority inheritance because the_thread
           *  is guaranteed to be the highest priority thread waiting for
           *  the semaphore.
           */
        }
      } else
        the_semaphore->count += 1;

      _Thread_Enable_dispatch();
      return( RTEMS_SUCCESSFUL );
  }

  return( RTEMS_INTERNAL_ERROR );   /* unreached - only to remove warnings */
}

/*PAGE
 *
 *  _Semaphore_Seize
 *
 *  This routine attempts to allocate a semaphore to the calling thread.
 *
 *  Input parameters:
 *    the_semaphore - pointer to semaphore control block
 *    option_set    - acquire semaphore options
 *
 *  Output parameters:
 *    TRUE  - if semaphore allocated
 *    FALSE - if semaphore NOT allocated
 *
 *  INTERRUPT LATENCY:
 *    available
 *    wait
 */

boolean _Semaphore_Seize(
  Semaphore_Control *the_semaphore,
  rtems_option    option_set
)
{
  Thread_Control *executing;
  ISR_Level       level;

  executing = _Thread_Executing;
  executing->Wait.return_code = RTEMS_SUCCESSFUL;
  _ISR_Disable( level );
  if ( the_semaphore->count != 0 ) {
    the_semaphore->count -= 1;
    if ( _Attributes_Is_binary_semaphore( the_semaphore->attribute_set ) ) {
      the_semaphore->holder     = executing;
      the_semaphore->holder_id  = executing->Object.id;
      the_semaphore->nest_count = 1;
      executing->resource_count++;
    }
    _ISR_Enable( level );
    return( TRUE );
  }

  if ( _Options_Is_no_wait( option_set ) ) {
    _ISR_Enable( level );
    executing->Wait.return_code = RTEMS_UNSATISFIED;
    return( TRUE );
  }

  if ( _Attributes_Is_binary_semaphore( the_semaphore->attribute_set ) ) {
    if ( _Objects_Are_ids_equal(
            _Thread_Executing->Object.id, the_semaphore->holder_id ) ) {
      the_semaphore->nest_count++;
      _ISR_Enable( level );
      return( TRUE );
    }
  }

  the_semaphore->Wait_queue.sync = TRUE;
  executing->Wait.queue      = &the_semaphore->Wait_queue;
  executing->Wait.id         = the_semaphore->Object.id;
  executing->Wait.option_set = option_set;
  _ISR_Enable( level );
  return( FALSE );
}
