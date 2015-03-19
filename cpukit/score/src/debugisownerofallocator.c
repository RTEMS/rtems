/*
 * Copyright (c) 2014-2015 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/assert.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/thread.h>
#include <rtems/score/threaddispatch.h>

#if defined( RTEMS_DEBUG )
  bool _Debug_Is_owner_of_allocator( void )
  {
    API_Mutex_Control *mutex = _RTEMS_Allocator_Mutex;
    bool owner;

    /*
     * We have to synchronize with the _CORE_mutex_Surrender() operation,
     * otherwise we may observe an outdated mutex holder.
     */
    _Thread_Disable_dispatch();

    if ( mutex != NULL ) {
      owner = mutex->Mutex.holder == _Thread_Executing;
    } else {
      owner = false;
    }

    _Thread_Enable_dispatch();

    return owner;
  }
#endif
