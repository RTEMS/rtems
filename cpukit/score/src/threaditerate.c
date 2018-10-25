/*
 * Copyright (c) 2016 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>

void _Thread_Iterate(
  Thread_Visitor  visitor,
  void           *arg
)
{
  int api_index;

  for ( api_index = 1 ; api_index <= OBJECTS_APIS_LAST ; ++api_index ) {
    const Objects_Information *information;
    Objects_Maximum            i;

    if ( _Objects_Information_table[ api_index ] == NULL ) {
      continue;
    }

    information = _Objects_Information_table[ api_index ][ 1 ];

    if ( information == NULL ) {
      continue;
    }

    for ( i = 1 ; i <= information->maximum ; ++i ) {
      Thread_Control *the_thread;

      the_thread = (Thread_Control *) information->local_table[ i ];

      if ( the_thread != NULL ) {
        bool done;

        done = (* visitor )( the_thread, arg );

        if ( done ) {
          return;
        }
      }
    }
  }
}
