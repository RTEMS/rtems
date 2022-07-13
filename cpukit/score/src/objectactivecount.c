/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>

Objects_Maximum _Objects_Active_count(
  const Objects_Information *information
)
{
  Objects_Maximum   active;
  Objects_Maximum   index;
  Objects_Maximum   maximum;
  Objects_Control **local_table;

  _Assert( _Objects_Allocator_is_owner() );

  active = 0;
  maximum  = _Objects_Get_maximum_index( information );
  local_table = information->local_table;

  for ( index = 0; index < maximum; ++index ) {
    if ( local_table[ index ] != NULL ) {
      ++active;
    }
  }

  return active;
}
