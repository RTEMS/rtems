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

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/assert.h>
#include <rtems/score/threaddispatch.h>

#if defined( RTEMS_DEBUG )
  bool _Debug_Is_thread_dispatching_allowed( void )
  {
    bool dispatch_allowed;
    ISR_Level level;
    Per_CPU_Control *per_cpu;

    _ISR_Disable_without_giant( level );
    per_cpu = _Per_CPU_Get_by_index( _SMP_Get_current_processor() );
    dispatch_allowed = per_cpu->thread_dispatch_disable_level == 0;
    _ISR_Enable_without_giant( level );

    return dispatch_allowed;
  }
#endif
