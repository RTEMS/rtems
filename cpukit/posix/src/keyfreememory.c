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

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/key.h>

void _POSIX_Keys_Free_memory(
  POSIX_Keys_Control *the_key
)
{
  uint32_t            the_api;

  for ( the_api = 1; the_api <= OBJECTS_APIS_LAST; the_api++ )
    _Workspace_Free( the_key->Values[ the_api ] );
}
