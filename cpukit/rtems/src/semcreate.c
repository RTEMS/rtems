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
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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
#include <rtems/rtems/attr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/sem.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/coresem.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif
#include <rtems/score/sysstate.h>

#include <rtems/score/interr.h>

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
    return RTEMS_INVALID_NAME;

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Attributes_Is_global( attribute_set ) ) {

    if ( !_System_state_Is_multiprocessing )
      return RTEMS_MP_NOT_CONFIGURED;

    if ( _Attributes_Is_inherit_priority( attribute_set ) )
      return RTEMS_NOT_DEFINED;

  } else 
#endif

  if ( _Attributes_Is_inherit_priority( attribute_set ) || 
              _Attributes_Is_priority_ceiling( attribute_set ) ) {

    if ( ! ( (_Attributes_Is_binary_semaphore( attribute_set ) ||
              _Attributes_Is_simple_binary_semaphore( attribute_set )) &&
             
             _Attributes_Is_priority( attribute_set ) ) )
      return RTEMS_NOT_DEFINED;

  }

  if ( !_Attributes_Is_counting_semaphore( attribute_set ) && ( count > 1 ) )
    return RTEMS_INVALID_NUMBER;

  _Thread_Disable_dispatch();             /* prevents deletion */

  the_semaphore = _Semaphore_Allocate();

  if ( !the_semaphore ) {
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Attributes_Is_global( attribute_set ) &&
       ! ( _Objects_MP_Allocate_and_open( &_Semaphore_Information, name,
                            the_semaphore->Object.id, FALSE ) ) ) {
    _Semaphore_Free( the_semaphore );
    _Thread_Enable_dispatch();
    return RTEMS_TOO_MANY;
  }
#endif

  the_semaphore->attribute_set = attribute_set;

  /*
   *  If it is not a counting semaphore, then it is either a 
   *  simple binary semaphore or a more powerful mutex style binary
   *  semaphore.
   */

  if ( !_Attributes_Is_counting_semaphore( attribute_set ) ) {
    if ( _Attributes_Is_inherit_priority( attribute_set ) )
      the_mutex_attributes.discipline = CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT;
    else if ( _Attributes_Is_priority_ceiling( attribute_set ) )
      the_mutex_attributes.discipline = CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING;
    else if ( _Attributes_Is_priority( attribute_set ) )
      the_mutex_attributes.discipline = CORE_MUTEX_DISCIPLINES_PRIORITY;
    else
      the_mutex_attributes.discipline = CORE_MUTEX_DISCIPLINES_FIFO;


    if ( _Attributes_Is_binary_semaphore( attribute_set ) ) {
      the_mutex_attributes.lock_nesting_behavior = CORE_MUTEX_NESTING_ACQUIRES;

      switch ( the_mutex_attributes.discipline ) {
        case CORE_MUTEX_DISCIPLINES_FIFO:
        case CORE_MUTEX_DISCIPLINES_PRIORITY:
          the_mutex_attributes.only_owner_release = FALSE;
          break;
        case CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING:
        case CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT:
          the_mutex_attributes.only_owner_release = TRUE;
          break;
      }
    } else {
      the_mutex_attributes.lock_nesting_behavior = CORE_MUTEX_NESTING_BLOCKS;
      the_mutex_attributes.only_owner_release = FALSE;
    }

    the_mutex_attributes.priority_ceiling = priority_ceiling;

    if ( count == 1 )
      lock = CORE_MUTEX_UNLOCKED;
    else
      lock = CORE_MUTEX_LOCKED;

    _CORE_mutex_Initialize(
      &the_semaphore->Core_control.mutex,
      &the_mutex_attributes,
      lock
    );
  } else {
    if ( _Attributes_Is_priority( attribute_set ) )
      the_semaphore_attributes.discipline = CORE_SEMAPHORE_DISCIPLINES_PRIORITY;
    else
      the_semaphore_attributes.discipline = CORE_SEMAPHORE_DISCIPLINES_FIFO;

    /*
     *  This effectively disables limit checking.
     */

    the_semaphore_attributes.maximum_count = 0xFFFFFFFF;

    /*
     *  The following are just to make Purify happy.
     */

    the_mutex_attributes.lock_nesting_behavior = CORE_MUTEX_NESTING_ACQUIRES;
    the_mutex_attributes.priority_ceiling = PRIORITY_MINIMUM;

    _CORE_semaphore_Initialize(
      &the_semaphore->Core_control.semaphore,
      &the_semaphore_attributes,
      count
    );
  }

  _Objects_Open(
    &_Semaphore_Information,
    &the_semaphore->Object,
    (Objects_Name) name
  );

  *id = the_semaphore->Object.id;

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Attributes_Is_global( attribute_set ) )
    _Semaphore_MP_Send_process_packet(
      SEMAPHORE_MP_ANNOUNCE_CREATE,
      the_semaphore->Object.id,
      name,
      0                          /* Not used */
    );
#endif
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}
