/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
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

#include <rtems/score/assert.h>
#include <rtems/score/threaddispatch.h>

#if defined( RTEMS_DEBUG )
  bool _Debug_Is_thread_dispatching_allowed( void )
  {
    bool dispatch_allowed;
    ISR_Level level;
    Per_CPU_Control *cpu_self;

    _ISR_Local_disable( level );
    cpu_self = _Per_CPU_Get_snapshot();
    dispatch_allowed = cpu_self->thread_dispatch_disable_level == 0;
    _ISR_Local_enable( level );

    return dispatch_allowed;
  }
#endif
