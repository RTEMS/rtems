/**
 * @file
 *
 * @brief Object ID to Name
 * @ingroup ScoreObject
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>

#include <string.h>

#if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
Objects_Name_or_id_lookup_errors _Objects_Name_to_id_string(
  Objects_Information *information,
  const char          *name,
  Objects_Id          *id
)
{
  Objects_Control           *the_object;
  uint32_t                   index;

  /* ASSERT: information->is_string == true */

  if ( !id )
    return OBJECTS_INVALID_ADDRESS;

  if ( !name )
    return OBJECTS_INVALID_NAME;

  if ( information->maximum != 0 ) {

    for ( index = 1; index <= information->maximum; index++ ) {
      the_object = information->local_table[ index ];
      if ( !the_object )
        continue;

      if ( !the_object->name.name_p )
        continue;

      if (!strncmp( name, the_object->name.name_p, information->name_length)) {
        *id = the_object->id;
        return OBJECTS_NAME_OR_ID_LOOKUP_SUCCESSFUL;
      }
    }
  }

  return OBJECTS_INVALID_NAME;
}
#endif
