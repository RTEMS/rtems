/**
 * @file
 *
 * @ingroup ClassicClassInfo Object Class Information
 *
 * @brief Set Name of Object
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/object.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/objectimpl.h>

/*
 *  This method will set the object name based upon the user string.
 *  If the object class uses 32-bit names, then only the first 4 bytes
 *  of the string will be used.
 */
rtems_status_code rtems_object_set_name(
  rtems_id       id,
  const char    *name
)
{
  Objects_Information *information;
  Objects_Control     *the_object;
  Objects_Id           tmpId;

  if ( !name )
    return RTEMS_INVALID_ADDRESS;

  tmpId = (id == OBJECTS_ID_OF_SELF) ? rtems_task_self() : id;

  information  = _Objects_Get_information_id( tmpId );
  if ( !information )
    return RTEMS_INVALID_ID;

  _Objects_Allocator_lock();
  the_object = _Objects_Get_no_protection( tmpId, information );

  if ( the_object == NULL ) {
    _Objects_Allocator_unlock();
    return RTEMS_INVALID_ID;
  }

  _Objects_Set_name( information, the_object, name );
  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}
