/*
 *  NOTE: Each task has an interrupt semaphore associated with it.
 *        No matter which interrupt occurs that it has registered, 
 *        the same semaphore is used.
 *
 *  This whole interrupt scheme may have been eliminated in a later draft.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <intr.h>
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/coresem.h>
#include <rtems/score/thread.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/intr.h>
#include <rtems/posix/time.h>
#include <rtems/posix/threadsup.h>

/*
 *  _POSIX_Interrupt_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _POSIX_Interrupt_Manager_initialization( 
  unsigned32  maximum_interrupt_handlers
)
{
  unsigned32                index;
  POSIX_Interrupt_Control  *the_vector;

  _Objects_Initialize_information(
    &_POSIX_Interrupt_Handlers_Information,
    OBJECTS_POSIX_INTERRUPTS,
    FALSE,
    maximum_interrupt_handlers,
    sizeof( POSIX_Interrupt_Handler_control ),
    FALSE,
    0,
    FALSE
  );

  for ( index=0 ; index < CPU_INTERRUPT_NUMBER_OF_VECTORS ; index++ ) {
    the_vector = &_POSIX_Interrupt_Information[ index ];
    
    the_vector->number_installed = 0;
    the_vector->lock_count       = 0;
    the_vector->deferred_count   = 0;
    _Chain_Initialize_empty( &the_vector->Handlers );
  }
}

/*PAGE
 *
 *  22.3.1 Associate a User-Written ISR with an Interrupt, P1003.4b/D8, p. 74
 */

int intr_capture(
  intr_t          intr,
  int           (*intr_handler)( void *area ),
  volatile void   *area,
  size_t           areasize
)
{
  POSIX_Interrupt_Handler_control   *the_intr;
  POSIX_Interrupt_Control           *the_vector;
  POSIX_API_Thread_Support_Control  *thread_support;
  proc_ptr                           old_handler;

  if ( !_ISR_Is_vector_number_valid( intr ) || 
       !_ISR_Is_valid_user_handler( intr_handler ) )
    return EINVAL;

  _Thread_Disable_dispatch();

  the_intr = _POSIX_Interrupt_Allocate();
 
  if ( !the_intr ) {
    _Thread_Enable_dispatch();
    return ENOMEM;
  }

  the_vector = &_POSIX_Interrupt_Information[ intr ];

  the_intr->vector         = intr;
  the_intr->handler        = intr_handler;
  the_intr->user_data_area = area;
  the_intr->server         = _Thread_Executing;
  the_intr->is_active      = TRUE;

  thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
  thread_support->interrupts_installed++;

/* XXX should we malloc the semaphore on the fly???  if so we probably need to
   release it when the thread has released all interrupts and keep
   a count of how many it has installed.  CURRENTLY NO.. ALLOCATED w/TCB
*/

  /*
   *  This is sufficient to have the handlers invoked in the opposite
   *  order of installation.  The loop invoking them can then go from
   *  the front of the list to the end.
   */

  _Chain_Prepend( &the_vector->Handlers, &the_intr->Object.Node );

  if ( !the_vector->number_installed++ )
    _ISR_Install_vector(
      intr,
      (proc_ptr) _POSIX_Interrupt_Handler, 
      &old_handler
    );

  _Objects_Open( &_POSIX_Interrupt_Handlers_Information, &the_intr->Object, 0 );
 
  /*
   *  Normally, an Id would be returned here.
   */
 
  _Thread_Enable_dispatch();
 
  return 0;
}

/*PAGE
 *
 *  22.3.1 Associate a User-Written ISR with an Interrupt, P1003.4b/D8, p. 74
 */

int intr_release(
  intr_t    intr,
  int     (*intr_handler)( void *area )
)
{
  boolean                            found;
  POSIX_Interrupt_Handler_control   *the_intr;
  POSIX_Interrupt_Control           *the_vector;
  POSIX_API_Thread_Support_Control  *thread_support;
  Chain_Node                        *the_node;

  if ( !_ISR_Is_valid_user_handler( intr_handler ) )
    return EINVAL;

  _Thread_Disable_dispatch();

  /*
   *  Since interrupt handlers do not have a user visible id, there is
   *  no choice but to search the entire set of active interrupt handlers
   *  to find this one.
   */

  found = FALSE;

  the_vector = &_POSIX_Interrupt_Information[ intr ];

  the_node = _Chain_Head( &the_vector->Handlers );

  for ( ; !_Chain_Is_tail( &the_vector->Handlers, the_node ) ; ) {
    the_intr = (POSIX_Interrupt_Handler_control *) the_node;

    if ( the_intr->handler == intr_handler ) {
       found = TRUE;
       break;
    }
    the_node = the_node->next;
  }

  if ( !found ) {
    _Thread_Enable_dispatch();
    return EINVAL;
  }

  if ( !_Thread_Is_executing( the_intr->server ) ) {
    _Thread_Enable_dispatch();
    return EINVAL;  /* XXX should be ENOISR; */
  }
    
  /*
   *  OK now we have found the interrupt handler and can do some work.
   */

  _Chain_Extract( &the_intr->Object.Node );

  the_intr->is_active = FALSE;
 
  the_vector->number_installed -= 1;

  thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];
  thread_support->interrupts_installed--;

  /*
   *  It is unnecessary to flush the semaphore since the handler can only
   *  be "removed" by the thread which installed it.  Thus it cannot be
   *  blocked on the semaphore or it would not be executing this routine.
   */

  _Objects_Close( &_POSIX_Interrupt_Handlers_Information, &the_intr->Object );

  _POSIX_Interrupt_Free( the_intr );
 
  _Thread_Enable_dispatch();

  return 0;
}

/*PAGE
 *
 *  22.3.1 Associate a User-Written ISR with an Interrupt, P1003.4b/D8, p. 74
 */

int intr_lock(
  intr_t  intr
)
{
  POSIX_Interrupt_Control           *the_vector;

  _Thread_Disable_dispatch();

  the_vector = &_POSIX_Interrupt_Information[ intr ];

  the_vector->lock_count++;

  _Thread_Enable_dispatch();

  return 0;
}
  
/*PAGE
 *
 *  22.3.1 Associate a User-Written ISR with an Interrupt, P1003.4b/D8, p. 74
 */

int intr_unlock(
  intr_t  intr
)
{
  POSIX_Interrupt_Control           *the_vector;
 
  _Thread_Disable_dispatch();
 
  the_vector = &_POSIX_Interrupt_Information[ intr ];
 
  if ( !--the_vector->lock_count ) {
    while ( --the_vector->deferred_count ) {
      _POSIX_Interrupt_Handler( intr );
    }
  }
 
  _Thread_Enable_dispatch();
 
  return 0;
}
  
/*
 *  22.3.2 Await Interrupt Notification, P1003.4b/D8, p. 76
 */

int intr_timed_wait(
  int                     flags,
  const struct timespec  *timeout
)
{
  Watchdog_Interval                  ticks;
  POSIX_API_Thread_Support_Control  *thread_support;

  ticks = _POSIX_Timespec_to_interval( timeout );

  thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];

  _Thread_Disable_dispatch();

    _CORE_semaphore_Seize(
      &thread_support->Interrupt_Semaphore,
      0,             /* XXX does id=0 hurt in this case? */
      TRUE,
      ticks
    ); 
  _Thread_Enable_dispatch();

  return _Thread_Executing->Wait.return_code;  /* XXX should be POSIX */
}

/*PAGE
 *
 *  _POSIX_Interrupt_Handler 
 *
 */

void _POSIX_Interrupt_Handler(
  ISR_Vector_number   vector
)
{
  POSIX_Interrupt_Handler_control   *the_intr;
  POSIX_Interrupt_Control           *the_vector;
  POSIX_API_Thread_Support_Control  *thread_support;
  Chain_Node                        *the_node;
  int                                status;
 
  the_vector = &_POSIX_Interrupt_Information[ vector ];
 
  the_node = _Chain_Head( &the_vector->Handlers );
 
  for ( ; !_Chain_Is_tail( &the_vector->Handlers, the_node ) ; ) {
    the_intr = (POSIX_Interrupt_Handler_control *) the_node;
 
    status = (*the_intr->handler)( (void *) the_intr->user_data_area );

    switch ( status ) {
      case INTR_HANDLED_NOTIFY:
        thread_support = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];

        _CORE_semaphore_Surrender(
          &thread_support->Interrupt_Semaphore,
          0,                         /* XXX is id=0 a problem */
          0                          /* XXX is this a problem (mp support)*/
        );
        return;

      case INTR_HANDLED_DO_NOT_NOTIFY:
        return;

      case INTR_NOT_HANDLED:
      default:                            /* this should not happen */
        break;
    }
    the_node = the_node->next;
  }

  /* XXX
   *
   *  This is an unhandled interrupt!!!
   */
}
