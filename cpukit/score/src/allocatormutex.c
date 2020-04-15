/*
 * Copyright (c) 2017 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/apimutex.h>

static API_Mutex_Control _RTEMS_Allocator_Mutex =
  API_MUTEX_INITIALIZER( "_Allocator" );

void _RTEMS_Lock_allocator( void )
{
  _API_Mutex_Lock( &_RTEMS_Allocator_Mutex );
}

void _RTEMS_Unlock_allocator( void )
{
  _API_Mutex_Unlock( &_RTEMS_Allocator_Mutex );
}

bool _RTEMS_Allocator_is_owner( void )
{
  return _API_Mutex_Is_owner( &_RTEMS_Allocator_Mutex );
}
