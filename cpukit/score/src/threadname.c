/*
 * Copyright (c) 2017 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

#include <string.h>

size_t _Thread_Get_name(
  const Thread_Control *the_thread,
  char                 *buffer,
  size_t                buffer_size
)
{
  const char *name;

  name = the_thread->Join_queue.Queue.name;

  if ( name != NULL && name[ 0 ] != '\0' ) {
    return strlcpy( buffer, name, buffer_size );
  } else {
    return _Objects_Name_to_string(
      the_thread->Object.name,
      false,
      buffer,
      buffer_size
    );
  }
}
