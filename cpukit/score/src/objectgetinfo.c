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
  Objects_APIs   api,
  uint32_t       class
)
{
  Objects_Information *info;

  if ( !_Objects_Is_api_valid( api ) )
    return NULL;

  if ( !class || class > _Objects_API_maximum_class(api) )
    return NULL;

  info = _Objects_Information_table[ api ][ class ];
  if ( !info )
    return NULL;

  if ( info->maximum == 0 )
    return NULL;

  return info;
}

