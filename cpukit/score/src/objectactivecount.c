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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/objectimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>

Objects_Maximum _Objects_Active_count(
  const Objects_Information *information
)
{
  size_t inactive;
  size_t maximum;

  _Assert( _Objects_Allocator_is_owner() );

  inactive = _Chain_Node_count_unprotected( &information->Inactive );
  maximum  = information->maximum;

  return (Objects_Maximum) ( maximum - inactive );
}
