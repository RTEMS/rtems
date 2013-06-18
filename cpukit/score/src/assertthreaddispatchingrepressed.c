/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/assert.h>
#include <rtems/score/isr.h>
#include <rtems/score/threaddispatch.h>

#if defined( RTEMS_DEBUG )
  void _Assert_Thread_dispatching_repressed( void )
  {
    _Assert( !_Thread_Dispatch_is_enabled() || _ISR_Get_level() != 0 );
  }
#endif
