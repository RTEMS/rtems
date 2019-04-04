/**
 * @file
 *
 * @brief Object ID to Name
 * @ingroup RTEMSScoreObject
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>

#include <string.h>

Objects_Control *_Objects_Get_by_name(
  const Objects_Information *information,
  const char                *name,
  size_t                    *name_length_p,
  Objects_Get_by_name_error *error
)
{
  size_t          name_length;
  size_t          max_name_length;
  Objects_Maximum maximum;
  Objects_Maximum index;

  _Assert( _Objects_Has_string_name( information ) );
  _Assert( _Objects_Allocator_is_owner() );

  if ( name == NULL ) {
    *error = OBJECTS_GET_BY_NAME_INVALID_NAME;
    return NULL;
  }

  name_length = strnlen( name, information->name_length + 1 );
  max_name_length = information->name_length;
  if ( name_length > max_name_length ) {
    *error = OBJECTS_GET_BY_NAME_NAME_TOO_LONG;
    return NULL;
  }

  if ( name_length_p != NULL ) {
    *name_length_p = name_length;
  }

  maximum = _Objects_Get_maximum_index( information );

  for ( index = 0; index < maximum; ++index ) {
    Objects_Control *the_object;

    the_object = information->local_table[ index ];

    if ( the_object == NULL )
      continue;

    if ( the_object->name.name_p == NULL )
      continue;

    if ( strncmp( name, the_object->name.name_p, max_name_length ) == 0 ) {
      return the_object;
    }
  }

  *error = OBJECTS_GET_BY_NAME_NO_OBJECT;
  return NULL;
}
