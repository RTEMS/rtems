/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>

Objects_Information *_Objects_Get_information(
  Objects_APIs   the_api,
  uint32_t       the_class
)
{
  Objects_Information *info;

  if ( !_Objects_Is_api_valid( the_api ) )
    return NULL;

  if ( !the_class || the_class > _Objects_API_maximum_class(the_api) )
    return NULL;

  info = _Objects_Information_table[ the_api ][ the_class ];
  if ( !info )
    return NULL;

  if ( info->maximum == 0 )
    return NULL;

  return info;
}

