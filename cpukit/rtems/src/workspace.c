/*
 *  Workspace Handler
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/interr.h>
#include <rtems/config.h>

#include <string.h>  /* for memset */

bool rtems_workspace_get_information(
  Heap_Information_block  *the_info
)
{
  Heap_Get_information_status status;

  status = _Heap_Get_information( &_Workspace_Area, the_info );
  if ( status == HEAP_GET_INFORMATION_SUCCESSFUL )
    return true;
  else
    return false;
}

/*
 *  _Workspace_Allocate
 */
bool rtems_workspace_allocate(
  ssize_t   bytes,
  void    **pointer
)
{
   *pointer =  _Heap_Allocate( &_Workspace_Area, bytes );
   if (!pointer)
     return false;
   else
     return true;
}

/*
 *  _Workspace_Allocate
 */
bool rtems_workspace_free(
  void *pointer
)
{
   return _Heap_Free( &_Workspace_Area, pointer );
}

